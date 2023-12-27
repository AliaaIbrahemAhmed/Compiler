#include <algorithm>
#include <iostream>
#include "Production.h"
#include "CFG.h"
#include "CFGBuilding.h"
#include "utilities.h"

vector<string> CFG::newNonTerminalMap;

CFG::CFG(vector<Production *> rules, const vector<string> &nonTerminalMap) {
    this->rules = rules;
    this->nonTerminalMap = nonTerminalMap;
    CFG::newNonTerminalMap = nonTerminalMap;
    removeLeftRecursion();
    removeLeftRefactoring();
}

void changeRHS(unsigned int nonTerminalIndex, unsigned int vecPos, const vector<vector<string>> &oldRHS,
               vector<vector<string>> &updatedRHS) {

    // Fetch the vector at the specified position from updatedRHS
    vector<string> repeatedVector = updatedRHS[vecPos];

    // Erase the element at the specified nonTerminalIndex from repeatedVector
    repeatedVector.erase(repeatedVector.begin() + nonTerminalIndex);
    for (auto element: oldRHS) {
        vector<string> currentPart = repeatedVector;

        // Insert the elements from 'element' at nonTerminalIndex in currentPart
        currentPart.insert(currentPart.begin() + nonTerminalIndex, element.begin(), element.end());

        // Add currentPart to updatedRHS
        updatedRHS.push_back(currentPart);
    }
}

bool isEqual(const string& a, const string& b) { return a == b; }

void removeIndicesFromVector(std::vector<vector<string>> &v, std::vector<int> rm) {
    for_each(rm.crbegin(), rm.crend(), [&v](auto index) { v.erase(begin(v) + index); });
}
/*replacing out the production rules with the new RHS
 * entered after removing the left recursion and left refactoring*/
void replaceProduction(Production *oldRule, Production *production) {
    // Get the left-hand side non-terminal of the old rule
    string nonTtoPutIn = oldRule->getLHS();

    // Get the right-hand side of the production rule
    vector<vector<string>> rhs = production->getRHS();
    // Store indices of RHS elements that need to be deleted
    vector<int> deletedIndices;
    unsigned int len = rhs.size();
    for (auto i = 0; i < len; ++i) {
        for (auto j = 0; j < rhs[i].size(); ++j) {
            // Check if the current element is a non-terminal matching nonTtoPutIn
            if (isNonTerminal(rhs[i][j], CFG::newNonTerminalMap) && isEqual(rhs[i][j], nonTtoPutIn)) {
                // Replace elements in RHS considering the old rule's RHS structure
                changeRHS(j, i, oldRule->getRHS(), rhs);
                deletedIndices.push_back(i);
                break;
            }
        }
    }
    // Remove elements marked for deletion from the RHS
    removeIndicesFromVector(rhs, deletedIndices);
    // Set the modified RHS to the production rule
    production->setRHS(rhs);
}
/*check whether there is left recursion in the rule or not by iterating given production
 * and find the LHS == nonTerminal RHS*/
bool checkLeftRecursion(Production *production) {
    for (auto rhs: production->getRHS()) {
        if (isNonTerminal(rhs.front(), CFG::newNonTerminalMap)
            && isEqual(rhs.front(), production->getLHS())) {
            return true;
        }
    }
    return false;
}
/*   This function is crucial for maintaining the correct order of non-terminals within 'newNonTerminalMap'.
     It ensures that when introducing or modifying non-terminals in the grammar,
     the updated non-terminal ('updatedLHS') is inserted in the correct position after the original one ('LHS').
*/
void insertIntoItsPosition(const string &updatedLHS, const string &LHS) {
    auto &newNonTerminalMap = CFG::newNonTerminalMap;
    auto it = find(newNonTerminalMap.begin(), newNonTerminalMap.end(), LHS);
    if (it != newNonTerminalMap.end()) {
        newNonTerminalMap.insert(it + 1, updatedLHS);
    }

    // Update the 'newNonTerminalMap' with the modified sequence reflecting the updated order of non-terminals
    CFG::newNonTerminalMap = newNonTerminalMap;
}

// Example:
// Consider a production rule A -> Ab | c.
// The function modifies it to A -> cA' | ε, where A' is a new non-terminal which will be added after A in order.
Production *eliminateImmediateLeftRecursion(Production *procductionRule) {
    // Generate a new non-terminal by appending a marker to the existing left-hand side (LHS)
    auto newLHS = (procductionRule->getLHS() + newNonTMark);

    // Ensure the updated non-terminal position is maintained
    insertIntoItsPosition(newLHS, procductionRule->getLHS());
    vector<vector<string>> startWithLHS;
    vector<vector<string>> rhs = procductionRule->getRHS();

    for (auto it = rhs.begin(); it != rhs.end();) {
        // Check if the element starts with the original LHS
        if (isNonTerminal(it->front(), CFG::newNonTerminalMap) && isEqual((string) it->front(), procductionRule->getLHS())) {
            // Modify the RHS by replacing the original LHS with the newLHS
            it->erase(it->begin());
            it->push_back(newLHS);
            startWithLHS.push_back(*it);
            rhs.erase(it); // Remove the modified element from RHS
        } else {
            it->push_back(newLHS); // Add the newLHS to the RHS element
            ++it;    // Move to the next element (only when not erasing from the vector)
        }
    }

    // If the RHS becomes empty, the left recursion cannot be eliminated
    if (rhs.empty()) {
        cout << "Left recursion cannot be eliminated from this production.\n" << procductionRule->castToString();
        exit(1);
    }

    // Set the modified RHS to the original production
    procductionRule->setRHS(rhs);

    // Add an epsilon production for the newLHS
    startWithLHS.push_back({EPSILON});

    // Return a new production rule with the updated LHS and startWithLHS
    return new Production(newLHS, startWithLHS);
}

void CFG::removeLeftRecursion() {
    newNonTerminalMap = this->nonTerminalMap; // Copying the non-terminal map

    // Vector to store productions that undergo immediate recursion elimination
    vector<Production *> immediateRecursion;
    for (auto i = 0; i < rules.size(); ++i) {
        // For each production, replace the current production's RHS
        // with RHS from previous productions to remove left recursion
        for (auto j = 0; j < i; ++j) {
            replaceProduction(rules[j], rules[i]);
        }

        // Check if the current production has left recursion
        if (checkLeftRecursion(rules[i])) {
            // If left recursion exists, eliminate it
            // Create a new production after eliminating immediate left recursion
            Production *updatedProduction = eliminateImmediateLeftRecursion(rules[i]);

            // Store the updated production for subsequent processing
            immediateRecursion.push_back(updatedProduction);
        }
    }

    // Add the updated productions (after left recursion elimination) to the set of rules
    this->rules.insert(this->rules.end(), immediateRecursion.begin(), immediateRecursion.end());
}


void CFG::removeLeftRefactoring() {
    pair<unsigned int, unsigned int> pair = {1, 0};
    for (unsigned int i = 0; i < rules.size(); ++i) {
        vector<vector<string>> rhs = rules[i]->getRHS();
        vector<int> commonElemContainer = getCommonIndices(rhs);
        if (!commonElemContainer.empty()) {
            if (pair.second != i) {
                pair.first = 1;
            }
            int numberOfCommon = countCommon(commonElemContainer, rhs);
            vector<string> commonElems;

            for (auto x = 0; x < numberOfCommon; x++) {
                commonElems.push_back(rhs[commonElemContainer[0]][x]);
            }
            vector<vector<string>> newRhs;
            for (int index: commonElemContainer) {
                vector<string> newRHS = rhs[index];
                newRHS.erase(newRHS.begin(), newRHS.begin() + numberOfCommon);
                if (newRHS.empty()) {
                    newRHS.emplace_back(EPSILON);
                }
                newRhs.push_back(newRHS);
            }
            removeIndicesFromVector(rhs, commonElemContainer);
            auto newLHS = (rules[i]->getLHS() + string(pair.first++, factoringMark));
            insertIntoItsPosition(newLHS, rules[i]->getLHS());
            pair.second = i;
            commonElems.push_back(newLHS);
            rhs.push_back(commonElems);
            rules[i]->setRHS(rhs);
            auto *newProc = new Production(newLHS, newRhs);
            rules.push_back(newProc);
            i--;
        }

    }
}
vector<int> CFG::getCommonIndices(vector<vector<string>> rhs) {
    vector<int> commonElements;
    for (size_t j = 0; j < rhs.size(); ++j) {
        string temp = rhs[j][0];
        // Store the current index in 'commonElements'
        commonElements.push_back(j);

        // Check for elements with the same starting string
        for (size_t k = j + 1; k < rhs.size(); ++k) {
            if (temp == rhs[k][0]) {
                commonElements.push_back(k);
            }
        }

        // If there are multiple common elements, break the loop
        if (commonElements.size() > 1) {
            break;
        } else {
            commonElements.clear(); // Clear the vector for the next iteration
        }
    }
    return commonElements;
}

int CFG::countCommon(std::vector<int> commonElement, vector<vector<string>> rhs) {
    unsigned int count = 1;
    bool flag = true;
    while (flag) {
        for (auto index = 1; index < commonElement.size(); index++) {
            if (count < rhs[commonElement[index]].size() &&
                count < rhs[commonElement[0]].size()) {
                if (rhs[commonElement[0]][count] !=
                    rhs[commonElement[index]][count]) {
                    flag = false;
                }
            } else {
                flag = false;
            }
        }
        if (flag) count++;
    }
    return count;
}

vector<Production *> CFG::getProductions() {
    return this->rules;
}

void CFG::printProduction(ostream& output) {
    output << "Contents of 'Rules of production' set after removing left Recursion and left Refactor:" << endl;
    output << "--------------------------------------" << endl;
    for (const auto &proc: CFG::rules) {
        output << proc->castToString() << endl;
    }
}