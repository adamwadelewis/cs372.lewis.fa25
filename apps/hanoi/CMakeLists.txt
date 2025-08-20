cmake_minimum_required(VERSION 3.10)

#set the project name
project(hanoi)

set(CMAKE_CXX_STANDARD 17)

include_directories(../../include)

#add the executable
add_executable(hanoi hanoi.cpp)

add_executable(hanoiit hanoiit.cpp)


