cmake_minimum_required(VERSION 3.20.2)
project(Compiler)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Compiler
        phase1/Node.cpp
        phase1/Node.h
        phase1/State.cpp
        phase1/Input.cpp phase1/LexicalRules.h phase1/LexicalRules.cpp phase1/NFA.h phase1/NFA.cpp phase1/NFATODFA.h phase1/NFATODFA.cpp
        phase1/State.h
        phase1/Matcher.cpp phase1/Matcher.h phase1/DFAMinimization.cpp ParserGenerator/CFGBuilding.cpp ParserGenerator/Production.cpp utilities.cpp phase1/DFAMinimization.cpp phase1/State.cpp phase1/Input.h ParserGenerator/FirstAndFollow.h ParserGenerator/FirstAndFollow.cpp ParserGenerator/FirstAndFollow.cpp ParserGenerator/main.h ParserGenerator/main.cpp
        ParserGenerator/CFG.h
        ParserGenerator/Parser.cpp
        ParserGenerator/CFG.cpp)
