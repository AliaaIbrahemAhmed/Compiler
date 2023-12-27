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
#include "FirstAndFollow.h"
#include "CFG.h"
#include "phase1/Matcher.h"

using namespace std;


int main() {
    string absolutePath = "E:\\Compiler";
    Input input = *new Input("E:\\Compiler\\input.txt");
    NFA nfa = *new NFA(input.lexicalRules, *new Node({*new State(false, 0, "0")}));
    NFATODFA nfatodfa = *new NFATODFA(nfa.nfa);
    DFAMinimization DFAminimization;
    std::vector<string> transitions(nfa.transitionSet.begin(), nfa.transitionSet.end());
    DfaResult res = nfatodfa.nfaToDfa(nfa.root, {transitions});
    cout << "transition table with size(" << res.DFA.size() << ")" << res.endMap.size() << "\n";
    DfaResult minimizedRes = DFAminimization.minimization(res);

    nfatodfa.printTransitionTable(res.DFA);
//    cout << "minimized transition table with size(" << minimizedRes.DFA.size() << ")" << "\n";
//    nfatodfa.printTransitionTable(minimizedRes.DFA);
//    cout << res.DFA.size();
    DFAminimization.writeFile(absolutePath, minimizedRes);
//    for (auto p: minimizedRes.endMap) {
//        cout << p.first.states.begin()->name << " " << p.second << endl;
//    }
//    cout << res.DFA.size();
    Matcher matcher = *new Matcher();
    CFGBuilding CFGBuilding;

    matcher.set_output_file_name("E:\\Compiler\\output.txt");
    std::ifstream inputFile("E:\\Compiler\\test.txt");
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
    cout << "Symbol Table:" << endl;
    for (const string &symbol: symbol_table) {
        cout << symbol << endl;
    }
    CFGBuilding.CFGBuilder("E:\\Compiler\\ParserGenerator\\rules.txt", matcher.tokensName);
    CFG cfg = *new CFG(CFGBuilding.getProductionRules(), CFGBuilding.getOrderedNonTerminal());
    cfg.printChecked();
    cfg.printProduction();
    FirstAndFollow firstAndFollow = *new FirstAndFollow(CFGBuilding.getTerminalMap(),
                                                        cfg.newNonTerminalMap, cfg.getProductions());
    firstAndFollow.generateFirstAndFollow();
    firstAndFollow.printFirstAndFollow();

}