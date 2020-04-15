#include<iostream>
#include "Tftp.h"
#include<vector>
#include"ClientThread.h"
using namespace std;


std::vector<ClientThread*> client_threads;

void deleteStoppedClients(){
	std::vector<ClientThread*>::iterator it;
	for(it=client_threads.begin();it!=client_threads.end();it++){
		ClientThread*c=*it;
		if(c->isRunning() ==false){
			delete c;
			client_threads.erase(it);
			it--;
		}
	}
}

ClientThread* findClient(const std::string&address,unsigned port){
	for(ClientThread*a:client_threads){
		if(a->isRunning() && a->getAddress()==address && a->getPort()==port)
			return a;
	}
	return nullptr;
}


void server(){
	UdpSock socket;
	UdpSock::Status status=socket.bindToPort(8000);
	if(status!=UdpSock::Status::Done)
		std::cout<<"cannot bind"<<std::endl;



	Tftp::Packet received;
	std::string remote_address;
	unsigned remote_port;

	while(1){
		deleteStoppedClients();
		Tftp::receivePacket(socket,received,remote_address,remote_port);
		ClientThread*source=findClient(remote_address,remote_port);

		if(received.getOpcode()==Tftp::Opcode::Read){
			string filename=received.readString();
			cout<<"new read request "<<filename<<endl;
			if(!source)
				client_threads.push_back(new ClientThread(remote_address,remote_port,filename,socket));

			//if the client is already in the clients list, just ignore the packet,it will timeout and the destination
			//thread will resend the first packet
		}
		else{
			if(source)
				source->handlePacket(received);
		}	
	}
}


int main(int argc, char *argv[]){
	server();
}
