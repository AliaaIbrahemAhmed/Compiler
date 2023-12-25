#ifndef CFG_H
#define CFG_H

#include <set>
#include "Production.h"

const string newNonTMark = "#";
const char factoringMark = '?';

class CFG {
private:
    vector<Production *> productions;
    int countCommon(std::vector<int> commonElemContainer, vector<vector<string>> rhs);

    vector<int> findCommonElemIndices(vector<vector<string>> rhs);

    void insertIntoRHS(unsigned int nonTerminalPos, unsigned int vectorPos, vector<vector<string>> toInsert);

    void replaceProduction(Production *toInsert, Production *target);

    bool hasLeftRecursion(Production *production);

public:
    CFG();

    CFG(vector<Production *> rules);
    void eliminateLeftRecursion();
    bool isNonTerminal(const string &symbol);
    set<string> nonTerminalMap;
    void eliminateLeftRefactoring();
    Production *eliminateImmediateLeftRecursion(Production *production);
    vector<Production*> getProcs();

};

#endif