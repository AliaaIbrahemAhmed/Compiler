//
// Created by Bassant on 22/12/2023.
//

#include <iostream>
#include <algorithm>
#include "CFGBuilding.h"
#include "Terminal.h"
#include "utilities.h"

void
CFGBuilding::CFGBuilder(const string &path, const vector<string> &tokens) {
    openRulesFile.open(path, ios::in);
    if (!openRulesFile.is_open()) {
        printErrorMessage("Can't find the required file");
    } else {
        for (const auto &token: tokens) {
            this->terminalMap[token] = new Terminal(token);
        }
        buildCFG();
    }
}


void CFGBuilding::buildCFG() {
    string line;
    while (getline(this->openRulesFile, line)) {
        removeSpaces(line);
        if (line.empty()) continue;
        decodeRule(line);
    }
    this->openRulesFile.close();
}

void CFGBuilding::decodeRule(string &line) {
    string lhs;
    unsigned int index = 0;

    if (line.empty() || line[index] == '|') {
        if (this->productionRules.empty()) {
            printErrorMessage("Rule format is not correct for rule: " + line);
            return;
        }
        lhs = getLastProductionLHS();
    } else {
        lhs = checkLHS(line, index);
        if (lhs.empty()) {
            printErrorMessage("LHS format is not correct for rule: " + line);
            return;
        }
    }
    index++;
    RHSState rhs = checkRHS(line.substr(index));
    if (rhs.empty()) {
        printErrorMessage("RHS format is not correct for rule: " + line);
        return;
    }

    updateProduction(lhs, rhs);
}

Terminal *CFGBuilding::checkTerminal(string buffer) {
    extract(buffer);
    auto it = this->terminalMap.find(buffer);
    if (it == this->terminalMap.end()) {
        cout<<"Terminal " << buffer << " is not found in lexical analyzer tokens.\n";
        cout << "Add it to the parser terminals.\n";
        auto *t = new Terminal(buffer);
        this->terminalMap[buffer] = t;
    }
    return this->terminalMap[buffer];
}

RHSState CFGBuilding::checkRHS(string line) {
    extract(line);
    cout << "line: " << line << endl;
    vector<vector<string>> rhs;
    string buffer;
    bool terminalFlag = false;
    rhs.push_back(vector<string>());
    unsigned int currVec = 0;

    for (char c: line) {
        if (c == 0x27 || c == 0x91 || c == 0x92) {
            if (terminalFlag) {
                terminalFlag = false;
                if (buffer == "epsilon" || buffer == "EPSILON" || buffer == "\\L") {
                    rhs[currVec].push_back(EPSILON);
                } else {
                    rhs[currVec].push_back(checkTerminal(buffer)->getId());
                }
            } else {
                if (!buffer.empty()) {
                    rhs[currVec].push_back(addNonTerminal(buffer)->getId());
                }
                terminalFlag = true;
            }
            buffer = "";
        } else if (c == '|') {
            if (!buffer.empty()) {
                rhs[currVec].push_back(addNonTerminal(buffer)->getId());
            }
            rhs.push_back(vector<string>());
            currVec++;
            buffer = "";
        } else if (c == ' ' && !terminalFlag) {
            if (!buffer.empty()) {
                rhs[currVec].push_back(addNonTerminal(buffer)->getId());
            }
            buffer = "";
        } else {
            buffer += c;
        }
    }

    if (!buffer.empty() && !terminalFlag) {
        rhs[currVec].push_back(addNonTerminal(buffer)->getId());
    }

    // There is a closing quote missing so return an empty vector to be interpreted as error in format.
    if (terminalFlag) rhs.clear();

    return rhs;
}

void CFGBuilding::updateProduction(string lhs, vector<vector<string>> rhs) {
    auto it = this->rulesMapping.find(lhs);
    if (it == rulesMapping.end()) {
        Production *proc;
        proc = new Production(addNonTerminal(lhs), rhs);
        this->productionRules.push_back(proc);
        rulesMapping[lhs] = proc;
    } else {
        Production *proc = rulesMapping[lhs];
        proc->addToRHS(rhs);
    }
}

string CFGBuilding::getLastProductionLHS() {
    return this->productionRules.back()->getLHS()->getId();
}


string CFGBuilding::checkLHS(string &line, unsigned int &index) {
    string lhs;
    while (index < line.size() && line[index] != '=') {
        lhs += line[index];
        index++;
    }
    if (!lhs.empty() && line[index] == '=') {
        return lhs;
    }
    return "";
}

NonTerminal *CFGBuilding::addNonTerminal(string prodKey) {
    extract(prodKey);
    auto it = this->nonTerminalMap.find(prodKey);
    if (it == this->nonTerminalMap.end()) {
        this->nonTerminalMap[prodKey] = new NonTerminal(prodKey);
    }
    return this->nonTerminalMap[prodKey];
}

vector<Production *> CFGBuilding::getProductionRules() {
    return this->productionRules;
}

