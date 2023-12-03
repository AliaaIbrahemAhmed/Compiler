cmake_minimum_required(VERSION 3.24)
project(Compiler)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Compiler
        Node.cpp
        Node.h
        State.cpp
        State.h main.cpp)
