cmake_minimum_required(VERSION 3.5)
project(socket_c__)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(./)
set(SOURCE_FILES main.cpp Address.cpp CommonSocket.cpp Datagram.cpp SocketException.cpp TCP.cpp UDP.cpp)
add_executable(socket_c__ ${SOURCE_FILES})