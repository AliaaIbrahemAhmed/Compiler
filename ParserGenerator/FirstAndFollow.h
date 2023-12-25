//
// Created by aliaa on 12/24/23.
//

#ifndef COMPILER_FIRSTANDFOLLOW_H
#define COMPILER_FIRSTANDFOLLOW_H


#include "CFGBuilding.h"

class FirstAndFollow {
private:

public:

    set<string> terminalMap;
    set<string> nonTerminalMap;
    vector<string> orderedNonTerminal;
    unordered_map<basic_string<char>, Production*> rulesMapping;

    unordered_map<string, vector<string>> first;
    unordered_map<string, vector<Production>> firstProductionMap;

    unordered_map<string, vector<string>> follow;
    unordered_map<string, vector<Production>> followProductionMap;

    FirstAndFollow(const set<string> &terminalMap, const set<string> &nonTerminalMap,
                   const vector<string> &orderedNonTerminal, unordered_map<string, Production*> rulesMapping);


    bool isInNonTerminalMap(const string& s);

    bool isInTerminalMap(const string& s);

    bool isNonTerminal(const vector<string>& rhs);

    bool isTerminal(const vector<string>& rhs);

    void generateFirstAndFollow();

    pair<vector<string>, vector<Production>> getFirstOfNonTerminal(Production pd);

    pair<vector<string>, vector<Production>> getFollowOfNonTerminal(Production pd, bool isStartingSymbol);

    pair<vector<string>, vector<Production>> checkRulesToGetFollow(const string& nt);
};


#endif //COMPILER_FIRSTANDFOLLOW_H
