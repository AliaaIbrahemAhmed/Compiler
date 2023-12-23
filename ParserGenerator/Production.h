//
// Created by Bassant on 23/12/2023.
//

#ifndef COMPILER_PRODUCTION_H
#define COMPILER_PRODUCTION_H
#include "string"
#include "vector"
#include "NonTerminal.h"

using namespace std;

class Production {
private:
    NonTerminal* lhs;
    vector<vector<string>> rhs;
public:
    Production();
    Production(NonTerminal* lhs, vector<vector<string>>& rhs);

    explicit Production(int vector);

    //NonTerminal* getLHS();
    vector<vector<string>> getRHS();
    NonTerminal* getLHS();

    void setRHS(vector<vector<string>> rhs);
    void addToRHS(vector<vector<string>> anotherRHS);

};


#endif //COMPILER_PRODUCTION_H
