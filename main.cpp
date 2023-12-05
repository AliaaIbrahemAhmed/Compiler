#include <set>
#include <iostream>
#include <unordered_map>
#include "State.h"
#include "Node.h"
#include "Input.h"
#include "NFA.h"

using namespace std;


int main() {
    Input input = *new Input("/home/aliaa/input");
    NFA nfa = *new NFA(input.lexicalRules, *new Node({*new State(false, 0, "0")}));
    auto t = nfa.nfa[nfa.root].find("a")->second;
    cout<<"";
    ////////////
}