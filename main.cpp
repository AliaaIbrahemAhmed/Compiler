#include <set>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include "State.h"
#include "Node.h"
#include "Input.h"
#include "NFA.h"
#include "NFATODFA.h"
#include "Matcher.h"
#include <sstream>

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
    Input input = *new Input("D:\\Com\\input");
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
    cout << res.DFA.size();
    // Get file path input from the user
    std::string filePath;
    cout << "Enter the file path where you want to create the file: ";
    getline(std::cin, filePath);
    DFAminimization.writeFile(filePath, minimizedRes);
    for (auto p : minimizedRes.endMap) {
        cout << p.first.states.begin()->name << " " << p.second << endl;
    }
    cout << res.DFA.size();
    Matcher matcher=*new Matcher();
    matcher.set_output_file_name("D:\\Com\\output.txt");
    std::ifstream inputFile("D:\\Com\\test.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file" << std::endl;
        return 1;
    }

    std::vector<std::string> tokens;
    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string token;
        while (iss >> std::ws >> token) {  // Use std::ws to skip leading white spaces
            // Add the token to the vector
            tokens.push_back(token);
        }
    }

    // Close the input file
    inputFile.close();
    matcher.match(tokens, minimizedRes);
    set<string> symbol_table = matcher.get_symbol_table();
    cout << "Symbol Table:" << endl;
    for (const string& symbol : symbol_table) {
        cout << symbol << endl;
    }

}