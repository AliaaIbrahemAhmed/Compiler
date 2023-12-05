//
// Created by aliaa on 12/3/23.
//

#ifndef COMPILER_NFA_H
#define COMPILER_NFA_H


#include <unordered_map>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include "Node.h"
#include <set>
#include <iostream>
#include <unordered_map>
#include "State.h"
#include "Node.h"
#include "Input.h"

using namespace std;

using TRANSITIONS = unordered_multimap<string, Node>;
using TRANSITION_TABLE = unordered_map<Node, TRANSITIONS>;
#define EPSILON "\\L"

class NFA {

public:
    int count;
    LexicalRules lexicalRules;
    TRANSITION_TABLE nfa;
    Node root;
    unordered_map<string, pair<Node, Node>> regularDefinitionsNFA;
    unordered_map<string, pair<Node, Node>> KWsNFA;
    unordered_map<string, pair<Node, Node>> punctuationsNFA;
    unordered_map<string, pair<Node, Node>> REsNFA;


    string generateNewStateName();


    pair<Node, Node> kleeneClosure(const pair<Node, Node> &se, bool isEndState, int priority);

    pair<Node, Node> positiveClosure(const pair<Node, Node> &se, bool isEndState, int priority);



    pair<Node, Node> singleNodeNfa(const string &s, bool isEndState, int priority);

    vector<pair<Node, Node>> convertStackToVector(stack<pair<Node, Node>> myStack);


    NFA(LexicalRules &lexicalRules, Node root);


    bool isRange(const string &expression);


    pair<Node, Node> range(const string &expression, bool isEndState, int priority);

    Node getRoot(const vector<pair<Node, Node>> &nodes);

    vector<pair<Node, Node>> generateNFAforKWandP(const set<string> &rules, bool which);

    vector<pair<Node, Node>> generateKWsNFA();

    vector<pair<Node, Node>> generatePunctuationsNFA();

    pair<Node, Node>
    eatBackward(string first, const pair<Node, Node> &last, stack<pair<Node, Node>> lastParsedAtStack, bool isEndState,
                int priority);

    pair<Node, Node> eatForward(const pair<Node, Node> &first, string last, bool isEndState, int priority);

    vector<pair<Node, Node>> parseRDs();

    vector<pair<Node, Node>> parseREs();

    pair<Node, Node> parse(string rule, string patternName);

    pair<Node, Node> unionOP(const vector<pair<Node, Node>> &nodes, bool isEndState, int priority, string patternName);

    pair<Node, Node> parseAND(const string &s, bool isEndState, int priority, string patternName);

    pair<Node, Node> concatenation(vector<pair<Node, Node>> nodes, bool isEndState, int priority, string patternName);

    pair<Node, Node> parseOr(const string &s, bool isEndState, int priority, string patternName);
};


#endif //COMPILER_NFA_H
