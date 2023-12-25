#include <iostream>
#include <algorithm>
#include "CFG.h"
#include "CFGBuilding.h"

CFG::CFG(vector<Production *> productions) {
    this->productions = productions;
    eliminateLeftRecursion();
    eliminateLeftRefactoring();
}

bool CFG::isNonTerminal(const string &symbol) {
    return this->nonTerminalMap.find(symbol) != this->nonTerminalMap.end();
};

bool matchNonTerminals(string a, string b) { return a == b; }

void removeRHS(std::vector<vector<string>> &v, std::vector<int> rm) {
    std::for_each(rm.crbegin(), rm.crend(), [&v](auto index) { v.erase(begin(v) + index); });
}

void CFG::insertIntoRHS(unsigned int nonTerminalPos, unsigned int vectorPos, vector<vector<string>> toInsert) {
    vector<string> currentVector = productions[vectorPos]->getRHS()[nonTerminalPos];
    currentVector.erase(currentVector.begin());

    for (vector<string> elemVector: toInsert) {
        vector<string> newVector = currentVector;
        newVector.insert(newVector.begin(), elemVector.begin(), elemVector.end());
        toInsert.push_back(newVector);
        //productions[vectorPos]->addRHS(newVector);
    }
}

void CFG::replaceProduction(Production *toInsert, Production *target) {
    string nonTerminalToInsert = toInsert->getLHS();
    vector<vector<string>> rhs = target->getRHS();
    vector<int> indicesToRemove;
    unsigned int length = rhs.size();
    for (unsigned int i = 0; i < length; ++i) {
        for (unsigned int j = 0; j < rhs[i].size(); ++j) {
            if (isNonTerminal(rhs[i][j]) &&
                matchNonTerminals((string) rhs[i][j], nonTerminalToInsert)) {
                insertIntoRHS(j, i, toInsert->getRHS());
                indicesToRemove.push_back(i);
                break;
            }
        }
    }

    for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
        rhs.erase(rhs.begin() + *it);
    }

    // Set the modified rhs back to the target production
    target->setRHS(rhs);
}

bool CFG::hasLeftRecursion(Production *production) {
    for (vector<string> elemVector: production->getRHS()) {
        if (isNonTerminal(elemVector.front()) &&
            matchNonTerminals((string) elemVector.front(), production->getLHS())) {
            return true;
        }
    }
    return false;
}

Production *CFG::eliminateImmediateLeftRecursion(Production *production) {
    string newLHS = (production->getLHS() + newNonTMark);
    vector<vector<string>> startWithLHS;

    vector<vector<string>> rhs = production->getRHS();
    string ff = "";
    for (auto vectorIterator = rhs.begin(); vectorIterator != rhs.end();) {
        if (isNonTerminal(vectorIterator->front()) &&
            matchNonTerminals((string) vectorIterator->front(), production->getLHS())) {
            vectorIterator->erase(vectorIterator->begin());
            vectorIterator->push_back(newLHS);
            startWithLHS.push_back(*vectorIterator);
            rhs.erase(vectorIterator);
        } else {
            vectorIterator->push_back(newLHS);
            ++vectorIterator;
        }
    }

    if (rhs.empty()) {
        cout << "Left recursion cannot be eliminated from this production.\n" << production->toString();
        exit(1);
    }

    production->setRHS(rhs);
    startWithLHS.push_back({EPSILON});
    return new Production(newLHS, startWithLHS);
}

void CFG::eliminateLeftRecursion() {
    vector<Production *> resultOfImmediateRecursion;
    this->nonTerminalMap = nonTerminalMap;
    for (unsigned int i = 0; i < productions.size(); ++i) {
        for (unsigned int j = 0; j < i; ++j) {
            replaceProduction(productions[j], productions[i]);
        }

        if (hasLeftRecursion(productions[i])) {
            Production *newProduction = eliminateImmediateLeftRecursion(productions[i]);
            resultOfImmediateRecursion.push_back(newProduction);
        }
    }

    productions.insert(productions.end(), resultOfImmediateRecursion.begin(), resultOfImmediateRecursion.end());
}

void CFG::eliminateLeftRefactoring() {
    pair<unsigned int, unsigned int> productionPair = {1, 0};

    for (unsigned int i = 0; i < productions.size(); ++i) {
        vector<vector<string>> rhs = productions[i]->getRHS();
        vector<int> commonElemIndicesContainer = findCommonElemIndices(rhs);

        if (commonElemIndicesContainer.size() != 0) {
            if (productionPair.second != i) {
                productionPair.first = 1;
            }

            int countOfCommonElements = countCommon(commonElemIndicesContainer, rhs);
            vector<string> commonElems;

            for (unsigned int ii = 0; ii < countOfCommonElements; ii++) {
                commonElems.push_back(rhs[commonElemIndicesContainer[0]][ii]);
            }

            vector<vector<string>> newRhs;

            for (unsigned int ii = 0; ii < commonElemIndicesContainer.size(); ii++) {
                vector<string> myVector = rhs[commonElemIndicesContainer[ii]];
                myVector.erase(myVector.begin(), myVector.begin() + countOfCommonElements);

                if (myVector.empty()) {
                    myVector.push_back(EPSILON);
                }

                newRhs.push_back(myVector);
            }
            //////////////////////
            removeRHS(newRhs, commonElemIndicesContainer);
            //productions[i].removeRHS(commonElemIndicesContainer);
            string newLHS = (productions[i]->getLHS() + string(productionPair.first++, factoringMark));
            productionPair.second = i;
            commonElems.push_back(newLHS);
            newRhs.push_back(commonElems);
            productions[i]->setRHS(newRhs);
            Production *newProduction = new Production(newLHS, newRhs);
            productions.push_back(newProduction);
            i--;
        }
    }
}

vector<int> CFG::findCommonElemIndices(vector<vector<string>> rhs) {
    string commonElem;
    vector<int> commonElemIndicesContainer;

    for (unsigned int j = 0; j < rhs.size(); j++) {
        commonElem = rhs[j][0];
        commonElemIndicesContainer.push_back(j);

        for (unsigned int k = j + 1; k < rhs.size(); k++) {
            if (commonElem == rhs[k][0]) {
                commonElemIndicesContainer.push_back(k);
            }
        }

        if (commonElemIndicesContainer.size() > 1) {
            break;
        } else {
            commonElemIndicesContainer.clear();
        }
    }

    return commonElemIndicesContainer;
}

int CFG::countCommon(std::vector<int> commonElemIndicesContainer, vector<vector<string>> rhs) {
    unsigned int countOfCommonElements = 1;
    bool flag = true;

    while (flag) {
        for (unsigned int ii = 1; ii < commonElemIndicesContainer.size(); ii++) {
            if (countOfCommonElements < rhs[commonElemIndicesContainer[ii]].size() &&
                countOfCommonElements < rhs[commonElemIndicesContainer[0]].size()) {
                if (rhs[commonElemIndicesContainer[0]][countOfCommonElements] !=
                    rhs[commonElemIndicesContainer[ii]][countOfCommonElements]) {
                    flag = false;
                }
            } else {
                flag = false;
            }
        }

        if (flag) {
            countOfCommonElements++;
        }
    }

    return countOfCommonElements;
}

vector<Production*> CFG::getProcs() { return this->productions; }