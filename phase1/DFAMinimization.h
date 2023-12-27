//
// Created by Salah on 04/12/2023.
//
#include "State.h"
#include "Node.h"
#include "NFATODFA.h"
#include <unordered_map>
#include<stack>
#include<set>
#include <iostream>
#include <unordered_set>
#include <queue>

#ifndef COMPILER_DFAMINIMIZATION_H
#define COMPILER_DFAMINIMIZATION_H
using namespace std;


#define EPSILON "\\L"

class DFAMinimization {
private:
    DFA_TRANSITION_TABLE  transitionTable;

public:
    static int sizeMinimized; // Static member variable of the class
    DfaResult minimization(const DfaResult& transitonMap);
    void writeFile(const string& filePath, const DfaResult& dfaFinal);
    /* goToSameGroup(set<set<Node>> &groups, Node a ,Node b, string s, const DFA_TRANSITION_TABLE& dfa);*/

};


#endif //COMPILER_DFAMINIMIZATION_H