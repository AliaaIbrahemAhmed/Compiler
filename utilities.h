//
// Created by Salah on 23/12/2023.
//

#ifndef COMPILER_UTILITIES_H
#define COMPILER_UTILITIES_H
#include "string"
#include "ParserGenerator/FirstAndFollow.h"
#include "ParserGenerator/CFG.h"

using namespace std;

string extract(string& str);
void removeSpaces(string &str);
void printErrorMessage(const string& message);
bool isNonTerminal(string symbol, vector<string> nonTerminalMap);
void printIntoFile(FirstAndFollow firstAndFollow, CFG cfg, string pathFile);
#endif //COMPILER_UTILITIES_H