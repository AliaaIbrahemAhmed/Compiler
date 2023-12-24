//
// Created by maria on 12/23/2023.
//
#include <numeric>
#include <fstream>
#include "Parser.h"

Parser::Parser(vector<Production> &rules, unordered_map<string, vector<string>> &first,
               unordered_map<string, vector<Production>> &firstProductionMap,
               unordered_map<string, vector<string>> &follow) : rules(rules), first(first),
                                                                firstProductionMap(firstProductionMap),
                                                                follow(follow) {}

void Parser::constructTable(){
    for(Production production : this->rules){
        string lhs = production.getLHS();
        bool hasEpsilon = false;
        vector<string> firstVec = this->first[lhs];
        vector<Production> firstProductionMapping = this->firstProductionMap[lhs];
        for(int i = 0; i < firstVec.size(); i++){
            if(firstVec[i] == EPSILON){
                hasEpsilon = true;

                handleFollow(lhs, firstProductionMapping[i]);
                continue;
            }
            this->addEntryToTable(lhs, firstProductionMapping[i], firstVec[i] );
        }
        if(!hasEpsilon){
            Production syncProduction=   Production(lhs,{{"Sync"}});
            this->handleFollow(lhs,syncProduction);
        }
    }
}

void Parser::addEntryToTable(string& lhs, Production& production, string& symbol){
    //check if there exist an entry for this non-terminal
    if(this->parsingTable[lhs].find(symbol) != this->parsingTable[lhs].end()) {
        cout << "Ambiguity Detected!\n Not LL(1) Grammer";
        exit(1);
    }
    this->parsingTable[lhs][symbol] = production;
}

void Parser::handleFollow(string& lhs, Production& production){
    vector<string> followSet = this->follow[lhs];
    for(string symbol : followSet){
        this->addEntryToTable(lhs, production, symbol);
    }
}


string Parser::getRhsExpr(vector<string> rhs){
    return accumulate(
            rhs.begin() + 1, rhs.end(),
            rhs.front(),
            [](const std::string& a, const std::string& b) {
                return a + " " + b;
            }
    );
}

void Parser::tableToCsv(set<string> nonTerminals, set<string> terminals){
    std::ofstream csvFile("parsingTable.csv");
    // Write the table content
    csvFile << ",";
    for(const auto &terminal : terminals){
        csvFile << terminal << ",";
    }
    csvFile << "\n";
    for (const auto &nonTerminal : nonTerminals) {
        csvFile << nonTerminal << ",";
        for (const auto &terminal : terminals) {
            auto it = parsingTable[nonTerminal].find(terminal);
            if(it != parsingTable[nonTerminal].end()) {
                string production = it->second.getLHS() + " --> " +
                                    getRhsExpr(it->second.getRHS()[0]);
                csvFile << production << ',';
            } else{
                csvFile << ",";
            }
        }
        csvFile << "\n";
    }
    // Close the file
    csvFile.close();
}





