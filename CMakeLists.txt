cmake_minimum_required(VERSION 3.24)
project(Compiler)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Compiler
        Node.cpp
        Node.h
        State.cpp
        State.h main.cpp
        NFATODFA.h
        NFATODFA.cpp
        NFA.cpp
        NFA.h
        LexicalRules.cpp
        LexicalRules.h
        Input.cpp
        Input.h
        DFAMinimization.h
        DFAMinimization.cpp)
# Create or recreate the debug file
file(WRITE debug.log "Debug information")
