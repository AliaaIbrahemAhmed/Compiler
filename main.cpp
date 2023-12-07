#include <set>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include "State.h"
#include "Node.h"
#include "Input.h"
#include "NFA.h"
#include "NFATODFA.h"
#include "DFAMinimization.h"

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
                cout << nextState.isEndState << " ";
                cout << nextState.priority << " ";
            }
            std::cout << "}\n";
        }
    }

};


int main() {
    Input input = *new Input("D:\\4thyear\\Compilers\\Compiler\\input.txt");
    NFA nfa = *new NFA(input.lexicalRules, *new Node({*new State(false, 0, "0")}));
    printTransitionTable(nfa.nfa);
    NFATODFA nfatodfa = *new NFATODFA(nfa.nfa);
    DFAMinimization DFAminimization;
    std::vector<string> transitions(nfa.transitionSet.begin(), nfa.transitionSet.end());
    DfaResult res = nfatodfa.nfaToDfa(nfa.root, {transitions});
    cout<<"transition table with size("<<res.DFA.size()<<")"<<res.endMap.size()<<"\n";
    DfaResult minimizedRes = DFAminimization.minimization(res);

    nfatodfa.printTransitionTable(res.DFA);
    cout<<"minimized transition table with size("<<minimizedRes.DFA.size()<<")"<<"\n";
    nfatodfa.printTransitionTable(minimizedRes.DFA);
    for (auto p : minimizedRes.endMap) {
        cout << p.first.states.begin()->name << " " << p.second << endl;
    }

}