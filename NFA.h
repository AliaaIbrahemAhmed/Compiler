//
// Created by aliaa on 12/3/23.
//

#ifndef COMPILER_NFA_H
#define COMPILER_NFA_H


#include <unordered_map>
#include <vector>
#include <set>


#include "Node.h"

using namespace std;
class NFA {

public:
    unordered_map<Node, int> nfa;
};


#endif //COMPILER_NFA_H
