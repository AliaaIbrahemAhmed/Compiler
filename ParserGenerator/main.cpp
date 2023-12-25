#include <set>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "ParserGenerator/CFGBuilding.h"
#include "phase1/Input.h"
#include "phase1/NFA.h"
#include "phase1/DFAMinimization.h"
#include "phase1/Matcher.h"
#include "FirstAndFollow.h"

using namespace std;


int main() {
    Input input = *new Input("/home/aliaa/CLionProjects/Compiler/input.txt");
    NFA nfa = *new NFA(input.lexicalRules, *new Node({*new State(false, 0, "0")}));
    NFATODFA nfatodfa = *new NFATODFA(nfa.nfa);
    DFAMinimization DFAminimization;
    std::vector<string> transitions(nfa.transitionSet.begin(), nfa.transitionSet.end());
    DfaResult res = nfatodfa.nfaToDfa(nfa.root, {transitions});
    cout << "transition table with size(" << res.DFA.size() << ")" << res.endMap.size() << "\n";
    DfaResult minimizedRes = DFAminimization.minimization(res);

    nfatodfa.printTransitionTable(res.DFA);
    cout << "minimized transition table with size(" << minimizedRes.DFA.size() << ")" << "\n";
    nfatodfa.printTransitionTable(minimizedRes.DFA);
    cout << res.DFA.size();
    // Get file path input from the user
    std::string filePath;
    cout << "Enter the file path where you want to create the file: ";
    getline(std::cin, filePath);
    DFAminimization.writeFile(filePath, minimizedRes);
    for (auto p: minimizedRes.endMap) {
        cout << p.first.states.begin()->name << " " << p.second << endl;
    }
    cout << res.DFA.size();
    Matcher matcher = *new Matcher();
    CFGBuilding CFGBuilding;

    matcher.set_output_file_name("/home/aliaa/CLionProjects/Compiler/output.txt");
    std::ifstream inputFile("/home/aliaa/CLionProjects/Compiler/test.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file" << std::endl;
        return 1;
    }

    vector<string> tokens;
    string line;
    while (getline(inputFile, line)) {
        istringstream iss(line);
        string token;
        while (iss >> ws >> token) {  // Use std::ws to skip leading white spaces
            // Add the token to the vector
            tokens.push_back(token);
        }
    }

    // Close the input file
    inputFile.close();
    matcher.match(tokens, minimizedRes);
    set<string> symbol_table = matcher.get_symbol_table();
    CFGBuilding.CFGBuilder("/home/aliaa/CLionProjects/Compiler/ParserGenerator/rules.txt", matcher.tokensName);
    /********/
    // Assuming productionRules is a vector<Production*>
    for (const auto &production: CFGBuilding.getProductionRules()) {
        // Get the lhs and rhs of each Production
        string lhs = production->getLHS();
        vector<vector<string>> rhs = production->getRHS();

        // Print the lhs
        std::cout << "LHS: " << lhs << std::endl;

        // Print the rhs
        std::cout << "RHS:" << std::endl;
        for (const auto &subVector: rhs) {
            for (const auto &element: subVector) {
                std::cout << element << " ";
            }
            std::cout << std::endl;
        }

        std::cout << std::endl;
    }
    cout << "Symbol Table:" << endl;
    for (const string &symbol: symbol_table) {
        cout << symbol << endl;
    }
    FirstAndFollow firstAndFollow = *new FirstAndFollow(CFGBuilding.terminalMap, CFGBuilding.nonTerminalMap,
                                                        CFGBuilding.getOrderedNonTerminal(), CFGBuilding.rulesMapping);
    firstAndFollow.generateFirstAndFollow();
}