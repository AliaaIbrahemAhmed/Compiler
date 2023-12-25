#include <algorithm>
#include <iostream>
#include "Production.h"
#include "CFG.h"
#include "CFGBuilding.h"
#include "utilities.h"

set <string> CFG::checked;
CFG::CFG(vector<Production *> rules, set<string> nonTerminalMap) {
    this->rules = rules;
    this->nonTerminalMap = nonTerminalMap;
    CFG::checked = nonTerminalMap;
    eliminateLeftRecursion();
    eliminateLeftRefactoring();

}

// putIn Aa | b in place of S
// rhs = Ac | dSe | f, vecPos = 1, NonTPos = 1
// rhs = Ac | f | dAae | dbe
void
insertIntoRHS(unsigned int NonTPos, unsigned int vecPos, vector<vector<string>> putIn, vector<vector<string>> &rhs) {
    // Store the target vector in repeatedVec and erase the non terminal from it before inserting its replacement.
    // Also remove the target vector from the rhs before inserting the new ones.
    vector<string> repeatedVec = rhs[vecPos];
    repeatedVec.erase(repeatedVec.begin() + NonTPos);
    for (vector<string> v: putIn) {
        vector<string> currV = repeatedVec;
        currV.insert(currV.begin() + NonTPos, v.begin(), v.end());
        rhs.push_back(currV);
    }
}

bool match(string a, string b) { return a == b; }

void removeIndicesFromVector(std::vector<vector<string>> &v, std::vector<int> rm) {
    std::for_each(rm.crbegin(), rm.crend(), [&v](auto index) { v.erase(begin(v) + index); });
}

// S --> Aa | b
// A --> Ac | dSe | f
// then A --> Ac | f | dAae | dbe
void replaceProduction(Production *putIn, Production *proc) {
    string nonTtoPutIn = putIn->getLHS();
    vector<vector<string>> rhs = proc->getRHS();
    vector<int> replacedVecIndicesToBeDeleted;
    // Stored right hand side in len to stop the replace after consuming the original rhs.
    unsigned int len = rhs.size();
    for (unsigned int i = 0; i < len; ++i) {
        for (unsigned int j = 0; j < rhs[i].size(); ++j) {
            if (isNonTerminal(rhs[i][j], CFG::checked) && match(rhs[i][j], nonTtoPutIn)) {
                insertIntoRHS(j, i, putIn->getRHS(), rhs);
                replacedVecIndicesToBeDeleted.push_back(i);
                break;
            }
        }
    }
    removeIndicesFromVector(rhs, replacedVecIndicesToBeDeleted);
    proc->setRHS(rhs);
}

bool leftRecursionExist(Production *proc) {
    for (vector<string> v: proc->getRHS()) {
        if (isNonTerminal(v.front(), CFG::checked) && match(v.front(), proc->getLHS())) {
            return true;
        }
    }
    return false;
}

// A --> Ac | Aad | bd | f
// A --> bdA# | fA#
// A# --> cA# | adA# | Epsilon
Production *eliminateImmediateLeftRecursion(Production *proc) {
    auto newLHS = (proc->getLHS() + newNonTMark);
    vector<vector<string>> startWithLHS;
    vector<vector<string>> rhs = proc->getRHS();
    for (auto vecIt = rhs.begin(); vecIt != rhs.end();) {
        if (isNonTerminal(vecIt->front(), CFG::checked) && match((string) vecIt->front(), proc->getLHS())) {
            vecIt->erase(vecIt->begin());
            vecIt->push_back(newLHS);
            startWithLHS.push_back(*vecIt);
            rhs.erase(vecIt);
        } else {
            vecIt->push_back(newLHS);
            ++vecIt;    // Only increment when not erasing from vector.
        }
    }
    if (rhs.empty()) {
        cout << "Left recursion can not be eliminated from this production.\n" << proc->toString();
        exit(1);
    }
    proc->setRHS(rhs);
    startWithLHS.push_back({EPSILON});
    return new Production(newLHS, startWithLHS);
}

void CFG::eliminateLeftRecursion() {
    checked = this->nonTerminalMap;
    vector<Production *> resultOfImmediateRecursion;
    for (unsigned int i = 0; i < rules.size(); ++i) {
        for (unsigned int j = 0; j < i; ++j) {
            replaceProduction(rules[j], rules[i]);
        }
        if (leftRecursionExist(rules[i])) {
            Production *newProc = eliminateImmediateLeftRecursion(rules[i]);
            resultOfImmediateRecursion.push_back(newProc);
        }
    }
    this->rules.insert(this->rules.end(), resultOfImmediateRecursion.begin(), resultOfImmediateRecursion.end());
}

void CFG::eliminateLeftRefactoring() {
    pair<unsigned int, unsigned int> pair = {1, 0};
    for (unsigned int i = 0; i < rules.size(); ++i) {
        vector<vector<string>> rhs = rules[i]->getRHS();
        vector<int> commonElemContainer = commonElemIndeces(
                rhs); //vector for indexes for which components has the same commonElem
        if (commonElemContainer.size() != 0) {
            if (pair.second != i) {
                pair.first = 1;
            }
            int CountOFCommonElements = countCommon(commonElemContainer, rhs);
            vector<string> commonElems;
            for (unsigned int ii = 0; ii < CountOFCommonElements; ii++) {
                commonElems.push_back(rhs[commonElemContainer[0]][ii]);
            }
            vector<vector<string>> newRhs;
            for (unsigned int ii = 0; ii < commonElemContainer.size(); ii++) {
                vector<string> myvec = rhs[commonElemContainer[ii]];
                myvec.erase(myvec.begin(), myvec.begin() + CountOFCommonElements);
                if (myvec.empty()) {
                    myvec.push_back(EPSILON);
                }
                newRhs.push_back(myvec);
            }
            removeIndicesFromVector(rhs, commonElemContainer);
            auto newLHS = (rules[i]->getLHS() + string(pair.first++, factoringMark));
            pair.second = i;
            commonElems.push_back(newLHS);
            rhs.push_back(commonElems);
            rules[i]->setRHS(rhs);
            Production *newProc = new Production(newLHS, newRhs);
            rules.push_back(newProc);
            i--;
        }

    }
}

vector<int> CFG::commonElemIndeces(vector<vector<string>> rhs) {
    string commonElem;
    vector<int> commonElemContainer;
    for (unsigned int j = 0; j < rhs.size(); j++) {
        commonElem = rhs[j][0];
        commonElemContainer.push_back(j);
        for (unsigned int k = j + 1; k < rhs.size(); k++) {
            if (commonElem == rhs[k][0]) {
                commonElemContainer.push_back(k);
            }
        }
        if (commonElemContainer.size() > 1) {
            break;
        } else {
            commonElemContainer.clear();
        }
    }
    return commonElemContainer;
}

int CFG::countCommon(std::vector<int> commonElemContainer, vector<vector<string>> rhs) {
    unsigned int CountOFCommonElements = 1;
    bool flag = true;
    while (flag) {
        for (unsigned int ii = 1; ii < commonElemContainer.size(); ii++) {
            if (CountOFCommonElements < rhs[commonElemContainer[ii]].size() &&
                CountOFCommonElements < rhs[commonElemContainer[0]].size()) {
                if (rhs[commonElemContainer[0]][CountOFCommonElements] !=
                    rhs[commonElemContainer[ii]][CountOFCommonElements]) {
                    flag = false;
                }
            } else {
                flag = false;
            }
        }

        if (flag) {
            CountOFCommonElements++;
        }
    }
    return CountOFCommonElements;
}

vector<Production *> CFG::getProcs() { return this->rules; }