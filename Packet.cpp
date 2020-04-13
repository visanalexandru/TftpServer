#include"Packet.h"


namespace Tftp{

	Opcode Packet::getOpcode() const{
		uint16_t opcode;
		memcpy(&opcode,data,2);
		opcode=ntohs(opcode);


		switch(opcode){
			case 1:
				return Opcode::Read;
			case 2:
				return Opcode::Write;
			case 3:
				return Opcode::Data;
			case 4:
				return Opcode::Ack;
			case 5:
				return Opcode::Err;
		}
	}
	std::string Packet::readString(){
		std::string result;

		while(data[read_cursor]!=0){
			result+=data[read_cursor];
			read_cursor++;
		}
		read_cursor++;
		return result;
	}
	unsigned Packet::getSize() const{
		return write_cursor;
	}
	unsigned Packet::readInt(){
		uint16_t to_return;
		memcpy(&to_return,data+read_cursor,2);
		read_cursor+=2;
		return ntohs(to_return);
	}

	Packet::Packet():read_cursor(2),write_cursor(0){



	}
	const char*Packet::getData() const{
		return data;
	}
	void Packet::addInt(uint16_t to_add){
		to_add=htons(to_add);
		memcpy(data+write_cursor,&to_add,2);
		write_cursor+=2;
	}
	void Packet::addBuffer(const char*buffer,unsigned size){
		memcpy(data+write_cursor,buffer,size);
		write_cursor+=size;
	}

}
