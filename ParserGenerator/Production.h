//
// Created by Bassant on 23/12/2023.
//

#ifndef COMPILER_PRODUCTION_H
#define COMPILER_PRODUCTION_H
#include "string"
#include "vector"

using namespace std;

class Production {
private:
    string lhs;
    vector<vector<string>> rhs;
public:
    Production();
    Production(string lhs, vector<vector<string>> rhs);

    explicit Production(int vector);

    //NonTerminal* getLHS();
    vector<vector<string>> getRHS();
    string getLHS();

    void setRHS(vector<vector<string>> rhs);
    void addToRHS(vector<vector<string>> anotherRHS);
    string castToString();
};


#endif //COMPILER_PRODUCTION_H