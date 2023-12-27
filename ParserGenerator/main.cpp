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
#include "Parser.h"
#include "utilities.h"

using namespace std;


int main() {
    string absolutePath = "E:\\Compiler";
    Input input = *new Input(absolutePath+"\\input.txt");
    NFA nfa = *new NFA(input.lexicalRules, *new Node({*new State(false, 0, "0")}));
    NFATODFA nfatodfa = *new NFATODFA(nfa.nfa);
    DFAMinimization DFAminimization;
    std::vector<string> transitions(nfa.transitionSet.begin(), nfa.transitionSet.end());
    DfaResult res = nfatodfa.nfaToDfa(nfa.root, {transitions});
    cout << "transition table with size(" << res.DFA.size() << ")" << res.endMap.size() << "\n";
    DfaResult minimizedRes = DFAminimization.minimization(res);
    cout << "minimized transition table with size(" << minimizedRes.DFA.size() << ")" << "\n";
    cout << res.DFA.size();
    DFAminimization.writeFile(absolutePath, minimizedRes);

    Matcher matcher = *new Matcher();
    CFGBuilding CFGBuilding;

    matcher.set_output_file_name(absolutePath+"\\output.txt");
    std::ifstream inputFile(absolutePath+"\\test.txt");
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
    CFGBuilding.CFGBuilder(absolutePath+"\\ParserGenerator\\rules.txt", matcher.tokensName);

    CFG cfg = *new CFG(CFGBuilding.getProductionRules(), CFGBuilding.getOrderedNonTerminal());

    FirstAndFollow firstAndFollow = *new FirstAndFollow(CFGBuilding.getTerminalMap(),
                                                        cfg.newNonTerminalMap, cfg.getProductions());
    firstAndFollow.generateFirstAndFollow();
   // firstAndFollow.printFirstAndFollow(cout);
    printIntoFile(firstAndFollow,cfg,absolutePath);
    set<string> terminals = CFGBuilding.getTerminalMap();
    terminals.insert({"$", EPSILON});
    string start =  cfg.newNonTerminalMap[0];
    vector<Production*> rules  = cfg.getProductions() ;
    Parser parser = Parser( rules,
                            firstAndFollow.first,
                            firstAndFollow.firstProductionMap,
                            firstAndFollow.follow,
                            start);
    parser.constructTable();
    parser.setTerminals(terminals);
    parser.setNonTerminals(cfg.newNonTerminalMap);
    parser.tableToCsv();

    vector<string> tokensNames = matcher.tokensName;
    tokensNames.push_back("$");
    for(string token: tokensNames){
        if(token == "assign"){
            token = "=";
        }
        parser.parse(token);
    }


}