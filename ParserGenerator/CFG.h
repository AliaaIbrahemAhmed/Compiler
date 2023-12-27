#ifndef CFG_HPP
#define CFG_HPP

#include <set>
#include "Production.h"

const string newNonTMark = "#";
const char factoringMark = '?';

class CFG {
private:
    vector<Production *> rules;

    void removeLeftRecursion();

    void removeLeftRefactoring();

    int countCommon(std::vector<int> commonElemContainer, vector<vector<string>> rhs);

    vector<int> getCommonIndices(vector<vector<string>> rhs);

public:
    CFG();

    CFG(vector<Production *> rules, const vector <string>& nonTerminalMap);

    vector<Production *> getProductions();
    static vector <string> newNonTerminalMap;

    vector<string> nonTerminalMap;
    void printProduction(ostream& output);
};

#endif