#include "ClientThread.h"
#include <chrono>
using namespace std::chrono_literals;


ClientThread::ClientThread(const std::string&add,unsigned port,const std::string&file,UdpSock&sock):
	client_address(add),
	file_name(file),
	client_port(port),
	running(true),
	has_pending(false),
	socket(sock)
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
	bool status=cv.wait_for(lk, 5s,[this]{return needsToWake();});
	if(status){
		packet=pendingPacket;
		has_pending=false;
	}
	return status;
}
bool ClientThread::reachedEnd(const std::ifstream&in){
	return (in.rdstate() & std::ifstream::failbit ); 	
}
void ClientThread::run(){
	char buffer[512];
	int packets_sent=1;

	std::ifstream in(file_name, std::ifstream::binary);

	in.read(buffer,512);
	Tftp::Packet packet_to_send=Tftp::createDataPacket(buffer,in.gcount(),packets_sent);
	Tftp::Packet to_handle;

	bool need_to_send=true;


	while(need_to_send){

		bool received=getResponse(packet_to_send,to_handle);
	
		if(received){
			if(to_handle.getOpcode()==Tftp::Opcode::Ack &&to_handle.readInt()==packets_sent){
				if(reachedEnd(in)){

					std::cout<<"finished"<<std::endl;
					need_to_send=false;
				}
				else{
					packets_sent++;
					in.read(buffer,512);
					packet_to_send=Tftp::createDataPacket(buffer,in.gcount(),packets_sent);
				}
			}
			else{
				std::cout<<"invalid packet"<<std::endl;
			}
		}
		else{
			std::cout<<"timed out"<<std::endl;
			need_to_send=false;
		}

	}
	running=false;
}
bool ClientThread::isRunning() const{
	return running;
}


void ClientThread::handlePacket(const Tftp::Packet&packet){
	lock.lock();
	pendingPacket=packet;
	has_pending=true;
	lock.unlock();
	cv.notify_one();
}

std::string ClientThread::getAddress() const{
	return client_address;
}
unsigned ClientThread::getPort() const{
	return client_port;
}

