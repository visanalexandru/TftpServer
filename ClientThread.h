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

#include <chrono>
using namespace std::chrono_literals;

class ClientThread{
	private:
		const std::string client_address;
		const std::string file_name;
		const bool isReceiving;
		const unsigned client_port;
		const std::chrono::seconds packet_timeout=5s;
		const unsigned packet_timeout_limit=3;

		Tftp::Packet pendingPacket;
		bool has_pending;
		std::atomic<bool> running;
		std::atomic<int> curr_packet_index;
		UdpSock&socket;

		std::mutex lock;
		std::condition_variable cv;
		std::thread th;

		bool getResponse(Tftp::Packet&to_send,Tftp::Packet&packet);
		void run();
		void sendFile();
		void receiveFile();
		bool needsToWake();
		bool reachedEnd(const std::ifstream&in);
		bool isValid(const Tftp::Packet&to_check);
	public:
		ClientThread(const std::string&add,unsigned port,const std::string&file,UdpSock&sock,bool receive);
		~ClientThread();
		void handlePacket(const Tftp::Packet&packet);
		bool isRunning() const;
		std::string getAddress() const;
		unsigned getPort() const;
};

#endif
