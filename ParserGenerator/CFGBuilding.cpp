//
// Created by Bassant on 22/12/2023.
//

#include <iostream>
#include <algorithm>
#include "CFGBuilding.h"
#include "utilities.h"

void CFGBuilding::CFGBuilder(const string &path, const vector<string> &tokens) {
    openRulesFile.open(path, ios::in);
    if (!openRulesFile.is_open()) {
        printErrorMessage("Can't find the required file");
    } else {
        for (const auto &token: tokens) {
            //this->terminalMap[token] = new Terminal(token);
            this->terminalMap.insert(token);
        }
        buildCFG();
    }
//    cout<<"nonTerminal: ("<<this->orderedNonTerminal.size()<<")";
//    for (const auto &element : this->orderedNonTerminal) {
//        std::cout << element << " ";
//    }
//    std::cout << std::endl;
//    cout<<"Terminal: ("<<this->terminalMap.size()<<")";
//    for (const auto &element : this->terminalMap) {
//        std::cout << element << " ";
//    }
//    std::cout << std::endl;

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

    if (line[index] == '|' || line.empty()) {
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

string CFGBuilding::checkTerminal(string buffer) {
    extract(buffer);
    auto it = this->terminalMap.find(buffer);
    if (it == this->terminalMap.end()) {
        cout << "Terminal " << buffer << " is not found in lexical analyzer tokens.\n";
        cout << "Add it to the parser terminals.\n";
        this->terminalMap.insert(buffer);
    }
    return buffer;
}

RHSState CFGBuilding::checkRHS(string line) {
    extract(line);
    vector<vector<string>> rhs;
    string buffer;
    bool terminalFlag = false;
    rhs.emplace_back();
    unsigned int currVec = 0;

    for (char c: line) {
        if (c == 0x27 || c == 0x91 || c == 0x92 || c == '\'') {
            if (terminalFlag) {
                terminalFlag = false;
                if (buffer == "epsilon" || buffer == "EPSILON" || buffer == "\\L") {
                    rhs[currVec].push_back(EPSILON);
                } else {
                    rhs[currVec].push_back(checkTerminal(buffer));
                }
            } else {
                if (!buffer.empty()) {
                    rhs[currVec].push_back(addNonTerminal(buffer));
                }
                terminalFlag = true;
            }
            buffer = "";
        } else if (c == '|') {
            if (!buffer.empty()) {
                rhs[currVec].push_back(addNonTerminal(buffer));
            }
            rhs.push_back(vector<string>());
            currVec++;
            buffer = "";
        } else if (c == ' ') {
            if (!buffer.empty()) {
                rhs[currVec].push_back(addNonTerminal(buffer));
            }
            buffer = "";
        } else {
            buffer += c;
        }
    }

    if (!buffer.empty()) {
        rhs[currVec].push_back(addNonTerminal(buffer));
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
    return this->productionRules.back()->getLHS();
}

string CFGBuilding::checkLHS(string &line, unsigned int &index) {
    string lhs;
    while (index < line.size() && line[index] != '=') {
        lhs += line[index];
        index++;
    }
    if (!lhs.empty() && line[index] == '=') {
        extract(lhs);
        addNonTerminal(lhs);
        return lhs;
    }
    return "";
}

string CFGBuilding::addNonTerminal(string prodKey) {
    // extract(prodKey);
    auto it = find(orderedNonTerminal.begin(), orderedNonTerminal.end(), prodKey);
    if (it == this->orderedNonTerminal.end()) {
        orderedNonTerminal.push_back(prodKey);
    }
    return prodKey;
}

vector<Production *> CFGBuilding::getProductionRules() {
    return this->productionRules;
}

set<string> CFGBuilding::getTerminalMap() { return this->terminalMap; }

vector<string> CFGBuilding::getOrderedNonTerminal() { return this->orderedNonTerminal; }