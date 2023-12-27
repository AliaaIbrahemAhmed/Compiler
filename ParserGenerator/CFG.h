#ifndef CFG_HPP
#define CFG_HPP

#include <set>
#include "Production.h"

const string newNonTMark = "#";
const char factoringMark = '?';

/**
 * @brief Context Free Grammar takes the rules and eliminate the left recursion, and
 * left refactoring from it.
*/

class CFG {
private:
    vector<Production *> rules;

    void eliminateLeftRecursion();

    void eliminateLeftRefactoring();

    int countCommon(std::vector<int> commonElemContainer, vector<vector<string>> rhs);

    vector<int> commonElemIndeces(vector<vector<string>> rhs);

public:
    CFG();

    CFG(vector<Production *> rules, vector <string> nonTerminalMap);

    vector<Production *> getProcs();
    static vector <string> newNonTerminalMap;

    vector<string> nonTerminalMap;
    void printChecked();
    void printProduction();
};

#endif