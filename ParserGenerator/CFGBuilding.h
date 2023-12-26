//
// Created by Bassant on 22/12/2023.
//

#ifndef COMPILER_CFGBUILDING_H
#define COMPILER_CFGBUILDING_H


#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <set>
#include "Production.h"

using namespace std;
using RulesMap = unordered_map<string, Production*>;
using RHSState = vector<vector<string>>;
#define EPSILON "\\L"
class CFGBuilding {
private:
    set<string> terminalMap;
    set<string> nonTerminalMap;
    vector<string> orderedNonTerminal;
    fstream openRulesFile;
    vector<Production*> productionRules;
    RulesMap rulesMapping;
    string getLastProductionLHS();
    string checkLHS(string& line, unsigned int& index);
    vector<vector<string>> checkRHS(string line);
    string checkTerminal(string buffer);
    void updateProduction(string lhs, vector<vector<string>>rhs);
    string addNonTerminal(string prodKey);

public:
    void CFGBuilder(const string& path, const vector<string>& tokens);
    void buildCFG();
    void decodeRule(string& line);
    vector<Production*> getProductionRules();
    set<string> getTerminalMap();
    set<string> getNonTerminalMap();
    vector<string> getOrderedNonTerminal();
};


#endif //COMPILER_CFGBUILDING_H
