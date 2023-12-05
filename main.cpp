#include <set>
#include <iostream>
#include <unordered_map>
#include "State.h"
#include "Node.h"
#include "Input.h"
#include "NFA.h"
#include "NFATODFA.h"

using namespace std;


void printTransitionTable(TRANSITION_TABLE transtionTable) {
    std::cout << "\nNFA Transition Table:\n";
    for (auto &it: transtionTable) {
        for (auto it1 = it.second.begin(); it1 != it.second.end(); ++it1) {
            std::cout << "    ";
            for (const State &state: it.first.states) {
                cout << state.name << " ";
            }
            std::cout << "on Symbol: " << it1->first << " -> { ";
            for (const State &nextState: it1->second.states) {
                std::cout << nextState.name << " ";
            }
            std::cout << "}\n";
        }
    }

};


int main() {
    Input input = *new Input("/home/aliaa/input");
    NFA nfa = *new NFA(input.lexicalRules, *new Node({*new State(false, 0, "0")}));
    printTransitionTable(nfa.nfa);
    NFATODFA nfatodfa = *new NFATODFA(nfa.nfa);
    pair<DFA_TRANSITION_TABLE, Node> res = nfatodfa.nfaToDfa(nfa.root, input.inputs);
    nfatodfa.printTransitionTable(res.first);

}