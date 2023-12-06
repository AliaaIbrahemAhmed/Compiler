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

using DFA_TRANSITION_TABLE = unordered_map<Node, DFA_TRANSITIONS>;
using DFA_TRANSITIONS = unordered_map<string, Node>;
#define EPSILON "\L"

class DFAMinimization {

private:
    DFA_TRANSITION_TABLE  transitionTable;

public:
    DFA_TRANSITION_TABLE  minimization(const DFA_TRANSITION_TABLE& dfa);
    /* goToSameGroup(set<set<Node>> &groups, Node a ,Node b, string s, const DFA_TRANSITION_TABLE& dfa);*/

};


#endif //COMPILER_DFAMINIMIZATION_H
