#include <set>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include "State.h"
#include "Node.h"
#include "Input.h"
#include "NFA.h"
#include "NFATODFA.h"

using namespace std;

std::vector<std::string> parseFileCharByChar(const std::string& filename) {
    std::vector<std::string> result;

    // Open the file
    std::ifstream file(filename);

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return result;
    }

    // Read characters one by one
    char c;
    while (file.get(c)) {
        // Store each character as a string
        result.push_back(std::string(1, c));
    }

    // Close the file (not strictly necessary as it will be closed when 'file' goes out of scope)
    file.close();

    return result;
}
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
    Input input = *new Input("/home/aliaa/input");
    NFA nfa = *new NFA(input.lexicalRules, *new Node({*new State(false, 0, "0")}));
    printTransitionTable(nfa.nfa);
    NFATODFA nfatodfa = *new NFATODFA(nfa.nfa);
    pair<DFA_TRANSITION_TABLE, Node> res = nfatodfa.nfaToDfa(nfa.root, {"a","b"});
    nfatodfa.printTransitionTable(res.first);

}