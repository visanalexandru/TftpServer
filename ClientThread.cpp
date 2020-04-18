#include "ClientThread.h"




ClientThread::ClientThread(const std::string&add,unsigned port,const std::string&file,UdpSock&sock,bool receive):
	client_address(add),
	file_name(file),
	client_port(port),
	running(true),
	has_pending(false),
	socket(sock),
	curr_packet_index(0),
	isReceiving(receive)
{

	th =std::thread(&ClientThread::run,this);

}
ClientThread::~ClientThread(){
	th.join();
}

bool ClientThread::needsToWake(){
	return has_pending;
}


bool ClientThread::getResponse(Tftp::Packet&to_send,Tftp::Packet&packet){
	std::unique_lock<std::mutex> lk(lock);
	has_pending=false;
	Tftp::sendPacket(socket,client_address,client_port,to_send);
	bool status=cv.wait_for(lk, packet_timeout,[this]{return needsToWake();});
	if(status){
		packet=pendingPacket;
		has_pending=false;
	}
	return status;
}
bool ClientThread::reachedEnd(const std::ifstream&in){
	return (in.rdstate() & std::ifstream::failbit ); 	
}

void ClientThread::receiveFile(){
	std::ofstream out(file_name,std::ios::binary);
	bool need_to_send=true;
	curr_packet_index=0;
	Tftp::Packet packet_to_send,to_handle;
	unsigned timed_out=0;
	char buffer[512];

	while(need_to_send){
		packet_to_send=Tftp::createAckPacket(curr_packet_index);
		bool received=getResponse(packet_to_send,to_handle);

		if(received){
			curr_packet_index++;
			unsigned data_size=to_handle.getSize()-4;
			memcpy(buffer,to_handle.getData()+4,data_size);
			out.write(buffer,data_size);
			if(data_size<512){
				Tftp::sendPacket(socket,client_address,client_port,Tftp::createAckPacket(curr_packet_index));
				std::cout<<"finished receiving file"<<std::endl;
				need_to_send=false;
			}
		}
		else{
			timed_out++;
			std::cout<<"timed out"<<std::endl;
			if(timed_out==packet_timeout_limit){
				std::cout<<"packet timeout limit reached"<<std::endl;
				need_to_send=false;
			}
		}
	}
}


void ClientThread::sendFile(){

	char buffer[512];

	std::ifstream in(file_name, std::ifstream::binary);
	in.read(buffer,512);
	Tftp::Packet packet_to_send=Tftp::createDataPacket(buffer,in.gcount(),1);
	curr_packet_index=1;
	Tftp::Packet to_handle;
	bool need_to_send=true;
	unsigned timed_out=0;
	while(need_to_send){
		bool received=getResponse(packet_to_send,to_handle);

		if(received){
			if(reachedEnd(in)){
				std::cout<<"finished sending file"<<std::endl;
				need_to_send=false;
			}
			else{
				curr_packet_index++;
				in.read(buffer,512);
				packet_to_send=Tftp::createDataPacket(buffer,in.gcount(),curr_packet_index);
			}
			timed_out=0;
		}	
		else{
			timed_out++;
			std::cout<<"timed out"<<std::endl;
			if(timed_out==packet_timeout_limit){
				std::cout<<"packet timeout limit reached"<<std::endl;
				need_to_send=false;
			}
		}

	}
}
void ClientThread::run(){
	if(isReceiving)
		receiveFile();
	else 
		sendFile();	
	running=false;
}
bool ClientThread::isRunning() const{
	return running;
}


bool ClientThread::isValid(const Tftp::Packet&to_check){
	if(isReceiving){
		return to_check.getOpcode()==Tftp::Opcode::Data && to_check.getBlockId()==curr_packet_index+1;
	}
	else{
		return to_check.getOpcode()==Tftp::Opcode::Ack && to_check.getBlockId()==curr_packet_index;
	}
}

void ClientThread::handlePacket(const Tftp::Packet&packet){


	std::unique_lock<std::mutex> lk(lock);
	if(isValid(packet)){

		pendingPacket=packet;
		has_pending=true;
		cv.notify_one();
	}
}

std::string ClientThread::getAddress() const{
	return client_address;
}
unsigned ClientThread::getPort() const{
	return client_port;
}

