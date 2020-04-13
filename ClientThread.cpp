#include "ClientThread.h"
#include <chrono>
using namespace std::chrono_literals;


ClientThread::ClientThread(const std::string&add,unsigned port,const std::string&file,UdpSock&sock):
	client_address(add),
	file_name(file),
	client_port(port),
	running(false),
	socket(sock)
{

	th =std::thread(&ClientThread::run,this);

}

bool ClientThread::needsToWake(){
	return pendingPackets.size();
}


bool ClientThread::waitNewPacket(Tftp::Packet&packet){
	std::unique_lock<std::mutex> lk(lock);
	bool status=cv.wait_for(lk, 1s,[this]{return needsToWake();});
	if(status){
		packet=pendingPackets.front();
		pendingPackets.pop_front();
	}
	return status;
}
bool ClientThread::reachedEnd(const std::ifstream&in){
	return (in.rdstate() & std::ifstream::failbit ); 	
}
void ClientThread::run(){
	running=true;


	char buffer[512];
	int packets_sent=1;

	Tftp::sendPacket(socket,client_address,client_port,Tftp::createAckPacket(0));
	std::ifstream in(file_name, std::ifstream::binary);

	in.read(buffer,512);
	Tftp::Packet packet_to_send=Tftp::createDataPacket(buffer,in.gcount(),packets_sent);
	Tftp::Packet to_handle;

	bool need_to_send=true;


	while(need_to_send){
		Tftp::sendPacket(socket,client_address,client_port,packet_to_send);
		std::cout<<"sent packet "<<packets_sent<<std::endl;
		bool received;

		std::cout<<"wait for ack..."<<std::endl;
		do{
			received=waitNewPacket(to_handle);
			if(to_handle.getAckCode()<packets_sent)
				std::cout<<"discard ack"<<to_handle.getAckCode()<<std::endl;
		}
		while(received && to_handle.getAckCode()<packets_sent);
		if(received)
			std::cout<<"ack received "<<to_handle.getAckCode()<<std::endl;


		if(received && to_handle.getOpcode()==Tftp::Opcode::Ack &&to_handle.readInt()==packets_sent){	

			if(reachedEnd(in)){

				std::cout<<"FINISHED"<<std::endl;
				need_to_send=false;
			}

			else{
				packets_sent++;
				in.read(buffer,512);
				packet_to_send=Tftp::createDataPacket(buffer,in.gcount(),packets_sent);
			}
		}
		else{
			if(!received)
				std::cout<<"timed out,retrying"<<std::endl;
			else std::cout<<"packet was invalid,retrying"<<std::endl;
		}

	}
	running=false;
}
bool ClientThread::isRunning(){
	return running;
}


void ClientThread::handlePacket(const Tftp::Packet&packet){
	lock.lock();
	pendingPackets.push_back(packet);
	lock.unlock();
	cv.notify_one();
}
