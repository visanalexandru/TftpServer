#ifndef UDP_SOCK_H
#define UDP_SOCK_H

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include<string>

class UdpSock{
	public:
		enum class Status{
			SocketCreateError,
			Done,
			CannotBind,
			FailedToSend,
			FailedToReceive
		};


		UdpSock();
		bool isValid() const;
		Status bindToPort(unsigned port);
		Status send(const std::string&addr,unsigned port,const char*source,unsigned size);
		Status receive(std::string&remote_addr,unsigned&remote_port,char*destination,unsigned size,unsigned&received) const;

	private:
		Status setNewHandle();
		sockaddr_in getAddress(const std::string&addr,unsigned port);
		int handle;


};

#endif 
