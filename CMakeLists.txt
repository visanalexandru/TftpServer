cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
project(Tftp)


add_executable(Tftp  main.cpp UdpSock.cpp Packet.cpp Tftp.cpp ClientThread.cpp)
target_link_libraries(Tftp pthread)
