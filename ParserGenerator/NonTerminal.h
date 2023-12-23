//
// Created by Salah on 23/12/2023.
//

#ifndef COMPILER_NONTERMINAL_H
#define COMPILER_NONTERMINAL_H
#include "string"
using namespace std;

class NonTerminal {
private:
    string key;

public:
    NonTerminal();
    NonTerminal(string valueKey);
    string getId() const; // Function to retrieve the key
};

#endif //COMPILER_NONTERMINAL_H
