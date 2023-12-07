cmake_minimum_required(VERSION 3.20.2)
project(Compiler)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Compiler
        Node.cpp
        Node.h
        State.cpp
        Input.cpp LexicalRules.h LexicalRules.cpp NFA.h NFA.cpp NFATODFA.h NFATODFA.cpp
        State.h main.cpp
         Matcher.cpp Matcher.h )
