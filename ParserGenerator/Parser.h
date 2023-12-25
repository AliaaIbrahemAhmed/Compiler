//
// Created by maria on 12/23/2023.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "Production.h"
#include<unordered_map>
#include <set>
#include<iostream>
#include "NFATODFA.h"

using namespace std;

using PARSING_TABLE =  unordered_map<string,  unordered_map<string, Production>>;


class Parser{
private:
    vector<Production> rules;
    unordered_map<string , vector<string>> first;
    unordered_map<string , vector<Production>> firstProductionMap;
    unordered_map<string , vector<string>> follow;
    vector<string> stack;
    string startNonTerminal;
    string startingNonTerminal;
    PARSING_TABLE parsingTable;
    set<string> terminals;
    set<string> nonTerminals;
    vector<string> parsedTerminals;

public:
    Parser(vector<Production> &rules, unordered_map<string, vector<string>> &first,
           unordered_map<string, vector<Production>> &firstProductionMap,
           unordered_map<string, vector<string>> &follow, string& startingNonTerminal);

    void constructTable();
    string getRhsExpr(vector<string> rhs);
    void addEntryToTable(string &lhs, Production &production, string &symbol);
    void handleFollow(string &lhs, Production &production);
    void parse(string &token);
    void setTerminals(const set<string> &terminals);
    void setNonTerminals(const set<string> &nonTerminals);
    bool isTerminal(string term);
    bool isNonTerminal(string term);

    void tableToCsv();

    void writeToFile(ofstream &file);
};


#endif //COMPILER_PARSER_H

