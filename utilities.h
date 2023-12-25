//
// Created by Salah on 23/12/2023.
//

#ifndef COMPILER_UTILITIES_H
#define COMPILER_UTILITIES_H
#include "string"

using namespace std;

string extract(string& str);
void removeSpaces(string &str);
void printErrorMessage(const string& message);
bool isNonTerminal(string symbol, set<string> nonTerminalMap);

#endif //COMPILER_UTILITIES_H
