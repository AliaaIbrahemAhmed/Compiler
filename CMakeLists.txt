cmake_minimum_required(VERSION 3.20.2)
project(Compiler)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

include_directories(.)

add_executable(Compiler
        DFAMinimization.h
        Node.cpp
        Node.h
        State.cpp
        Input.cpp LexicalRules.h LexicalRules.cpp NFA.h NFA.cpp NFATODFA.h NFATODFA.cpp
        State.h main.cpp
         Matcher.cpp Matcher.h DFAMinimization.cpp ParserGenerator/CFGBuilding.cpp ParserGenerator/CFGBuilding.h ParserGenerator/CFGBuilding.h ParserGenerator/Production.cpp ParserGenerator/Production.h ParserGenerator/Production.h utilities.cpp utilities.h)
