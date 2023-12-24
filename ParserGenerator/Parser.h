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
    PARSING_TABLE parsingTable;

public:
    Parser(vector<Production> &rules, unordered_map<string, vector<string>> &first,
           unordered_map<string, vector<Production>> &firstProductionMap,
           unordered_map<string, vector<string>> &follow);

    void constructTable();
    string getRhsExpr(vector<string> rhs);
    void tableToCsv(set<string> nonTerminals, set<string> terminals);
    void addEntryToTable(string &lhs, Production &production, string &symbol);
    void handleFollow(string &lhs, Production &production);

};


#endif //COMPILER_PARSER_H

