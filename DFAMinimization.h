//
// Created by Salah on 04/12/2023.
//
#include "State.h"
#include "Node.h"
#include <unordered_map>
#include<stack>
#include<set>
#include <iostream>
#include <unordered_set>
#include <queue>

#ifndef COMPILER_DFAMINIMIZATION_H
#define COMPILER_DFAMINIMIZATION_H
using namespace std;

using TRANSITIONS = unordered_map<string, Node>;
using TRANSITION_TABLE = unordered_map<Node, TRANSITIONS>;
#define EPSILON "\L"

class DFAMinimization {

private:
    TRANSITION_TABLE transitionTable;


public:
    pair<TRANSITION_TABLE, Node> minimization(Node startNode, vector<string>& inputs);

};


#endif //COMPILER_DFAMINIMIZATION_H
