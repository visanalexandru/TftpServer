#ifndef PACKET_H
#define PACKET_H
#include"UdpSock.h"
#include<string.h>
#include<stdint.h>
namespace Tftp{
	enum class Opcode{
		Read,
		Write,
		Data,
		Ack,
		Err

	};
	class Packet{
		private:
			char data[512];
			unsigned read_cursor;
			unsigned write_cursor;
		public:	
			Opcode getOpcode() const;
			Packet();
			std::string readString();
			unsigned readInt();
			void addBuffer(const char*buffer,unsigned size);
			void addInt(uint16_t to_add);
			unsigned getSize() const;
			const char*getData() const;

			friend UdpSock::Status receivePacket(const UdpSock&socket,Packet&packet,std::string&address,unsigned&port);
	};

}
#endif
