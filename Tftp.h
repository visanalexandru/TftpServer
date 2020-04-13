#ifndef TFTP_H
#define TFTP_H
#include<fstream>
#include"Packet.h"

namespace Tftp{
	UdpSock::Status receivePacket(const UdpSock&socket,Packet&packet,std::string&address,unsigned&port);
	UdpSock::Status sendPacket(UdpSock&socket,const std::string&address,unsigned port,const Packet&packet);
	Packet createAckPacket(unsigned ackId);
	Packet createDataPacket(const char*data,unsigned size,unsigned packetId);
	Packet getNextChunk(std::ifstream&file);
}

#endif 
