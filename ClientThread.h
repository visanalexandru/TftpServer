#ifndef CLIENT_THREAD_H
#define CLIENT_THREAD_H

#include<mutex>
#include<thread>
#include<condition_variable>
#include"Tftp.h"
#include<iostream>
#include<fstream>
#include<atomic>
#include<deque>
#include"UdpSock.h"


class ClientThread{
	private:
		const std::string client_address;
		const std::string file_name;
		const unsigned client_port;


		Tftp::Packet pendingPacket;
		bool has_pending;
		std::atomic<bool> running;
		UdpSock&socket;

		std::mutex lock;
		std::condition_variable cv;
		std::thread th;

		bool getResponse(Tftp::Packet&to_send,Tftp::Packet&packet);
		void run();
		bool needsToWake();
		bool reachedEnd(const std::ifstream&in);
	public:
		ClientThread(const std::string&add,unsigned port,const std::string&file,UdpSock&sock);
		~ClientThread();
		void handlePacket(const Tftp::Packet&packet);
		bool isRunning() const;
		std::string getAddress() const;
		unsigned getPort() const;
};

#endif
