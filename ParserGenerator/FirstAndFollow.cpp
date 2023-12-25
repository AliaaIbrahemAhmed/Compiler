//
// Created by aliaa on 12/24/23.
//

#include <numeric>
#include "FirstAndFollow.h"
#include <algorithm>
#include <utility>
#include <unordered_set>

#define DollarSign "$"
#define EPSILON "\\L"


FirstAndFollow::FirstAndFollow(const set<string> &terminalMap, const set<string> &nonTerminalMap,
                               const vector<string> &orderedNonTerminal, unordered_map<string, Production*> rulesMapping)
        : terminalMap(
        terminalMap), nonTerminalMap(nonTerminalMap), orderedNonTerminal(orderedNonTerminal), rulesMapping(std::move(
        rulesMapping)) {

}


void FirstAndFollow::generateFirstAndFollow() {
    unordered_map<string, Production*> rules = this->rulesMapping;
    for (auto ntIt = this->orderedNonTerminal.rbegin(); ntIt != this->orderedNonTerminal.rend(); ++ntIt) {
        pair<vector<string>, vector<Production>> firstRes = getFirstOfNonTerminal(*rules[*ntIt]);
        first[*ntIt] = firstRes.first;
        firstProductionMap[*ntIt] = firstRes.second;
    }
    bool isStartingSymbol = true;
    for (auto &ntIt: this->orderedNonTerminal) {
        pair<vector<string>, vector<Production>> followRes = getFollowOfNonTerminal(*rules[ntIt], isStartingSymbol);
        follow[ntIt] = followRes.first;
        isStartingSymbol = false;
    }
}

pair<vector<string>, vector<Production>> FirstAndFollow::getFirstOfNonTerminal(Production pd) {
    pair<vector<string>, vector<Production>> res;
    string lhs = pd.getLHS();
    vector<vector<string>> rightHandSides = pd.getRHS();
    for (vector<string> rhs: rightHandSides) {
        if (isInTerminalMap(rhs[0]) || rhs[0] == EPSILON) {
            res.first.push_back(rhs[0]);
            vector<vector<string>> newRHS;
            newRHS.push_back(rhs);
            Production p = *new Production(lhs, newRHS);
            res.second.push_back(pd);
        } else if (isInNonTerminalMap(rhs[0])) {
            vector<string> nTRHSFirsts = first[rhs[0]];
            vector<Production> nTRHSFirstsProductions = firstProductionMap[rhs[0]];
            res.first.insert(res.first.end(), nTRHSFirsts.begin(), nTRHSFirsts.end());
            vector<vector<string>> newRHS;
            newRHS.push_back(rhs);
            Production p = *new Production(lhs, newRHS);
            res.second.insert(res.second.end(), nTRHSFirsts.size(), p);
        }
    }
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

bool checkForEpsilon(vector<string> vec) {
    for (const auto &s: vec) {
        if (s == EPSILON) {
            return true;
        }
    }
    return false;
}

pair<vector<string>, vector<Production>> FirstAndFollow::checkRulesToGetFollow(const string &nt) {
    pair<vector<string>, vector<Production>> res;
    unordered_map<string, Production*> rules = this->rulesMapping;

    for (const auto &rule: rules) {
        Production pd = *rule.second;
        string lhs = pd.getLHS();
        vector<vector<string>> rightHandSides = pd.getRHS();
        for (vector<string> rhs: rightHandSides) {
            for (int i = 0; i < rhs.size(); i++) {
                if (rhs[i] == nt) {
                    if (i == rhs.size() - 1 || (i == rhs.size() - 2 && !isInTerminalMap(rhs[i + 1]) &&
                                                 checkForEpsilon(first[rhs[i + 1]]) && nt != lhs )) {
                        vector<string> nTLHSFollow = follow[lhs];
                        res.first.insert(res.first.end(), nTLHSFollow.begin(), nTLHSFollow.end());
                    }
                    if (i < rhs.size() - 1) {
                        vector<string> ntFirst;
                        if (isInNonTerminalMap(rhs[i + 1])) ntFirst = first[rhs[i + 1]];
                        else if (isInTerminalMap(rhs[i + 1])) ntFirst = {rhs[i + 1]};
                        auto itr = std::find(ntFirst.begin(), ntFirst.end(), EPSILON);
                        if (itr != ntFirst.end()) ntFirst.erase(itr);
                        res.first.insert(res.first.end(), ntFirst.begin(), ntFirst.end());
                    }
                }
            }
        }
    }
    sort( res.first.begin(), res.first.end() );
    res.first.erase( unique( res.first.begin(), res.first.end() ), res.first.end() );

    return res;
}


bool FirstAndFollow::isInTerminalMap(const string &s) {
    return (this->terminalMap.find(s) != this->terminalMap.end());
}

bool FirstAndFollow::isInNonTerminalMap(const string &s) {
    return (this->nonTerminalMap.find(s) != this->nonTerminalMap.end());
}

bool FirstAndFollow::isTerminal(const vector<string> &rhs) {
    for (const string &s: rhs) {
        if (!isInTerminalMap(s)) return false;
    }
    return true;
}

bool FirstAndFollow::isNonTerminal(const vector<string> &rhs) {
    for (const string &s: rhs) {
        if (!isInNonTerminalMap(s)) return false;
    }
    return true;
}
