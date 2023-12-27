//
// Created by aliaa on 12/24/23.
//

#include <numeric>
#include "FirstAndFollow.h"
#include <algorithm>
#include <utility>
#include <unordered_set>
#include <iostream>

#define DollarSign "$"
#define EPSILON "\\L"


FirstAndFollow::FirstAndFollow(const set<string> &terminalMap,
                               const vector<string> &orderedNonTerminal, vector<Production *> rules)
        : terminalMap(
        terminalMap), orderedNonTerminal(orderedNonTerminal), rules(std::move(
        rules)) {
    nonTerminalMap = *new set<string>(orderedNonTerminal.begin(), orderedNonTerminal.end());
    generateRulesMapping();
}

void FirstAndFollow::printFirstAndFollow() {
    // Print First sets
    std::cout << "\nFirst Sets:\n";
    for (const auto &nonTerminal: orderedNonTerminal) {
        std::cout << nonTerminal << ": { ";
        for (const auto &symbol: first[nonTerminal]) {
            if (symbol == EPSILON) cout << "Epsilon ";
            else std::cout << symbol << " ";
        }
        std::cout << "}\n";
    }

    // Print Follow sets
    std::cout << "\nFollow Sets:\n";
    for (const auto &nonTerminal: orderedNonTerminal) {
        std::cout << nonTerminal << ": { ";
        for (const auto &symbol: follow[nonTerminal]) {
            std::cout << symbol << " ";
        }
        std::cout << "}\n";
    }
}

void removeDuplicates(std::vector<string> &vec1, std::vector<Production> &vec2) {
    std::unordered_set<string> seen;
    std::vector<int> removedIndices;

    // Traverse the vector in reverse to preserve the order of the indices
    for (int i = vec1.size(); i-- > 0;) {
        if (seen.find(vec1[i]) != seen.end()) {
            // Duplicate found, remove the element and store the index
            removedIndices.push_back(i);
            vec1.erase(vec1.begin() + i);
            vec2.erase(vec2.begin() + i);

        } else {
            // Add the element to the set if it's not a duplicate
            seen.insert(vec1[i]);
        }
    }
}

void FirstAndFollow::generateFirstAndFollow() {
    unordered_map<string, Production *> map = this->rulesMapping;
    for (auto ntIt = this->orderedNonTerminal.rbegin(); ntIt != this->orderedNonTerminal.rend(); ++ntIt) {
        pair<vector<string>, vector<Production>> firstRes = getFirstOfNonTerminal(*map[*ntIt]);

    }
    bool isStartingSymbol = true;
    for (auto &ntIt: this->orderedNonTerminal) {
        pair<vector<string>, vector<Production>> followRes = getFollowOfNonTerminal(*map[ntIt], isStartingSymbol);
        follow[ntIt] = followRes.first;
        isStartingSymbol = false;
    }
}


pair<vector<string>, vector<Production>> FirstAndFollow::getFirstOfNonTerminal(Production pd) {
    pair<vector<string>, vector<Production>> res;
    string lhs = pd.getLHS();
    if (!first[lhs].empty()) {
        return {first[lhs], firstProductionMap[lhs]};
    }
    vector<vector<string>> rightHandSides = pd.getRHS();
    for (vector<string> rhs: rightHandSides) {
        if (isInTerminalMap(rhs[0]) || rhs[0] == EPSILON) {
            res.first.push_back(rhs[0]);
            vector<vector<string>> newRHS;
            newRHS.push_back(rhs);
            Production p = *new Production(lhs, newRHS);
            res.second.push_back(p);
        } else {
            int k = 0;
            while (k < rhs.size() &&
                   (isInNonTerminalMap(rhs[k]) && k == 0 || isInNonTerminalMap(rhs[k]) && checkForEpsilon(
                           getFirstOfNonTerminal(*rulesMapping[rhs[k - 1]]).first))) {
                pair<vector<string>, vector<Production>> nTRHSFirsts = getFirstOfNonTerminal(*rulesMapping[rhs[k]]);
                res.first.insert(res.first.end(), nTRHSFirsts.first.begin(), nTRHSFirsts.first.end());
                vector<vector<string>> newRHS;
                newRHS.push_back(rhs);
                Production p = *new Production(lhs, newRHS);
                res.second.insert(res.second.end(), nTRHSFirsts.first.size(), p);
                k++;
            }
            if (k < rhs.size() && isInTerminalMap(rhs[k]) &&
                checkForEpsilon(getFirstOfNonTerminal(*rulesMapping[rhs[k - 1]]).first) ) {
                res.first.push_back(rhs[k]);
                vector<vector<string>> newRHS;
                newRHS.push_back(rhs);
                Production p = *new Production(lhs, newRHS);
                res.second.push_back(p);
            }

        }
    }
    removeDuplicates(res.first, res.second);
    first[lhs] = res.first;
    firstProductionMap[lhs] = res.second;
    return res;
}

pair<vector<string>, vector<Production>> FirstAndFollow::getFollowOfNonTerminal(Production pd, bool isStartingSymbol) {
    pair<vector<string>, vector<Production>> res;

    if (isStartingSymbol) {
        res.first.emplace_back(DollarSign);
    }

    pair<vector<string>, vector<Production>> res1 = checkRulesToGetFollow(pd.getLHS());
    res.first.insert(res.first.end(), res1.first.begin(), res1.first.end());

    return res;
}

bool FirstAndFollow::checkForEpsilon(const vector<string> &vec) {
    for (const auto &s: vec) {
        if (s == EPSILON) {
            return true;
        }
    }
    return false;
}

bool FirstAndFollow::checkForEpsilonInFirsts(const vector<string> &rhs, int index) {
    for (int i = index; i < rhs.size(); i++) {
        if (!(isInNonTerminalMap(rhs[i]) && checkForEpsilon(first[rhs[i]]))) {
            return false;
        }
    }
    return true;
}

pair<vector<string>, vector<Production>> FirstAndFollow::checkRulesToGetFollow(const string &nt) {
    pair<vector<string>, vector<Production>> res;
    unordered_map<string, Production *> map = this->rulesMapping;

    for (const auto &rule: map) {
        Production pd = *rule.second;
        string lhs = pd.getLHS();
        vector<vector<string>> rightHandSides = pd.getRHS();
        for (vector<string> rhs: rightHandSides) {
            for (int i = 0; i < rhs.size(); i++) {
                if (rhs[i] == nt) {
                    if (i == rhs.size() - 1 || (i < rhs.size() - 1 && checkForEpsilonInFirsts(rhs, i + 1))) {
                        vector<string> nTLHSFollow = follow[lhs];
                        res.first.insert(res.first.end(), nTLHSFollow.begin(), nTLHSFollow.end());
                    }
                    /*     if (i < rhs.size() - 1) {
                             vector<string> ntFirst;
                             if (isInNonTerminalMap(rhs[i + 1])) ntFirst = first[rhs[i + 1]];
                             else if (isInTerminalMap(rhs[i + 1])) ntFirst = {rhs[i + 1]};
                             auto itr = std::find(ntFirst.begin(), ntFirst.end(), EPSILON);
                             if (itr != ntFirst.end()) ntFirst.erase(itr);
                             res.first.insert(res.first.end(), ntFirst.begin(), ntFirst.end());
                         }
     */
                    int k = i + 1;
                    while (k < rhs.size() && ((isInNonTerminalMap(rhs[k]) && k == i + 1) ||
                                              isInNonTerminalMap(rhs[k]) && checkForEpsilon(first[rhs[k - 1]]))) {
                        vector<string> ntFirst = first[rhs[k]];
                        auto itr = std::find(ntFirst.begin(), ntFirst.end(), EPSILON);
                        if (itr != ntFirst.end()) ntFirst.erase(itr);
                        res.first.insert(res.first.end(), ntFirst.begin(), ntFirst.end());
                        vector<vector<string>> newRHS;
                        newRHS.push_back(rhs);
                        Production p = *new Production(lhs, newRHS);
                        res.second.insert(res.second.end(), ntFirst.size(), p);
                        k++;
                    }
                    if (k < rhs.size() && isInTerminalMap(rhs[k]) && ((k == i + 1 || checkForEpsilon(first[rhs[k - 1]])))) {
                        res.first.push_back(rhs[k]);
                        vector<vector<string>> newRHS;
                        newRHS.push_back(rhs);
                        Production p = *new Production(lhs, newRHS);
                        res.second.push_back(p);
                    }
                }
            }
        }
    }
    sort(res.first.begin(), res.first.end());
    res.first.erase(unique(res.first.begin(), res.first.end()), res.first.end());

    return res;
}


bool FirstAndFollow::isInTerminalMap(const string &s) {
    return (this->terminalMap.find(s) != this->terminalMap.end());
}

bool FirstAndFollow::isInNonTerminalMap(const string &s) {
    return (this->nonTerminalMap.find(s) != this->nonTerminalMap.end());
}

void FirstAndFollow::generateRulesMapping() {
    for (auto p: rules) {
        rulesMapping[p->getLHS()] = p;
    }
}