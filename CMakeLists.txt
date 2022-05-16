cmake_minimum_required(VERSION 3.14)
project(Programming_Assignment_II)

set(CMAKE_CXX_STANDARD 14)

add_executable(server server/server.cpp)

add_executable(client client/client.cpp)