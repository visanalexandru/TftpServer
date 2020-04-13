#include "UdpSock.h"
#include<iostream>

UdpSock::UdpSock():handle(-1){


}

sockaddr_in UdpSock::getAddress(const std::string&addr,unsigned port){


	struct sockaddr_in     servaddr; 
	memset(&servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr =inet_addr(addr.c_str());
	return servaddr;
}


UdpSock::Status UdpSock::setNewHandle(){

	handle= socket(AF_INET, SOCK_DGRAM, 0);
	if (handle < 0 ) {
		return Status::SocketCreateError;
	}
	return Status::Done;
}

bool UdpSock::isValid() const{
	return handle!=-1;
}

UdpSock::Status UdpSock::bindToPort(unsigned port){

	if(!isValid()){
		Status stat=setNewHandle();
		if(stat!=Status::Done)
			return stat;
	}
	sockaddr_in servaddr;
	servaddr.sin_family    = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(port); 


	if ( bind(handle, (const struct sockaddr *)&servaddr,
				sizeof(servaddr)) < 0 ) 
	{
		return Status::CannotBind;
	}
	return Status::Done;
}


UdpSock::Status UdpSock::send(const std::string&addr,unsigned port,const char*source,unsigned size){

	if(!isValid()){
		Status stat=setNewHandle();
		if(stat!=Status::Done)
			return stat;
	}


	sockaddr_in servaddr=getAddress(addr,port);
	int sent = sendto(handle,source, size, 0,(struct sockaddr *)&servaddr, sizeof(servaddr));

	if(sent<0){
		return Status::FailedToSend;
	}
	return Status::Done;
}

UdpSock::Status UdpSock::receive(std::string&remote_addr,unsigned&remote_port,char*destination,
		unsigned size,unsigned&received) const{

	struct sockaddr_in    cliaddr; 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	unsigned int len=sizeof(cliaddr);

	received = recvfrom(handle,destination, size, 0,(struct sockaddr *)&cliaddr,&len);

	if(received<0){
		received=0;
		return Status::FailedToReceive;
	}

	remote_addr= inet_ntoa(cliaddr.sin_addr);
	remote_port    = ntohs(cliaddr.sin_port);
	return Status::Done;
}



