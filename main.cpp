#include<iostream>
#include "Tftp.h"
#include"ClientThread.h"
using namespace std;


void server(){
	UdpSock socket;
	UdpSock::Status status=socket.bindToPort(8000);
	if(status!=UdpSock::Status::Done)
		std::cout<<"cannot bind"<<std::endl;

	Tftp::Packet received;
	ClientThread*th;
	std::string remote_address;
	unsigned remote_port;

	while(1){
		Tftp::receivePacket(socket,received,remote_address,remote_port);
		if(received.getOpcode()==Tftp::Opcode::Read){
			cout<<"read request"<<endl;
			th=new ClientThread(remote_address,remote_port,received.readString(),socket);
		}
		else th->handlePacket(received);
	
	}
}


int main(int argc, char *argv[]){
	server();
}
