#include"Tftp.h"
#include<iostream>

namespace Tftp{

	UdpSock::Status receivePacket(const UdpSock&socket,Packet&packet,std::string&address,unsigned&port){
		unsigned received;
		
		UdpSock::Status status=socket.receive(address,port,packet.data,512,received);
		packet.write_cursor=received;
		packet.read_cursor=2;
		return status;

	}

	UdpSock::Status sendPacket(UdpSock&socket,const std::string&address,unsigned port,const Packet&packet){
		return socket.send(address,port,packet.getData(),packet.getSize());
	}
	Packet createAckPacket(unsigned ackId){
		Packet to_return;	
		to_return.addInt(4);
		to_return.addInt(0);
		return to_return;
	
	}
	Packet createDataPacket(const char*data,unsigned size,unsigned packetId){
		Packet to_return;
		to_return.addInt(3);
		to_return.addInt(packetId);
		to_return.addBuffer(data,size);
		return to_return;

	}	
}
