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


		std::deque<Tftp::Packet> pendingPackets;
		std::atomic<bool> running;
		UdpSock&socket;

		std::mutex lock;
		std::condition_variable cv;
		std::thread th;

		bool waitNewPacket(Tftp::Packet&packet);
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
