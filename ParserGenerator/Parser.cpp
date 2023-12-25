//
// Created by maria on 12/23/2023.
//
#include <numeric>
#include <fstream>
#include "Parser.h"

Parser::Parser(vector<Production> &rules, unordered_map<string, vector<string>> &first,
               unordered_map<string, vector<Production>> &firstProductionMap,
               unordered_map<string, vector<string>> &follow,
               string& startingNonTerminal) : rules(rules), first(first),
                                              firstProductionMap(firstProductionMap),
                                              follow(follow),
                                              startNonTerminal(startingNonTerminal){
    this->stack.push_back("$");
    this->stack.push_back(startingNonTerminal);
    this->file.open("leftRecursion.txt");
}

void Parser::setTerminals(const set<string> &terminals) {
    Parser::terminals = terminals;
}

void Parser::setNonTerminals(const set<string> &nonTerminals) {
    Parser::nonTerminals = nonTerminals;
}


void Parser::constructTable(){
    for(Production production : this->rules){
        string lhs = production.getLHS();
        bool hasEpsilon = false;
        vector<string> firstVec = this->first[lhs];
        vector<Production> firstProductionMapping = this->firstProductionMap[lhs];
        for(int i = 0; i < firstVec.size(); i++){
            if(firstVec[i] == EPSILON){
                hasEpsilon = true;

                handleFollow(lhs, firstProductionMapping[i]);
                continue;
            }
            this->addEntryToTable(lhs, firstProductionMapping[i], firstVec[i] );
        }
        if(!hasEpsilon){
            Production syncProduction=   Production(lhs,{{"Sync"}});
            this->handleFollow(lhs,syncProduction);
        }
    }
}

void Parser::addEntryToTable(string& lhs, Production& production, string& symbol){
    //check if there exist an entry for this non-terminal
    if(this->parsingTable[lhs].find(symbol) != this->parsingTable[lhs].end()) {
        cout << "Ambiguity Detected!\n Not LL(1) Grammer";
        exit(1);
    }
    this->parsingTable[lhs][symbol] = production;
}

void Parser::handleFollow(string& lhs, Production& production){
    vector<string> followSet = this->follow[lhs];
    for(string symbol : followSet){
        this->addEntryToTable(lhs, production, symbol);
    }
}


string Parser::getRhsExpr(vector<string> rhs){
    return accumulate(
            rhs.begin() + 1, rhs.end(),
            rhs.front(),
            [](const std::string& a, const std::string& b) {
                return a + " " + b;
            }
    );
}

void Parser::tableToCsv(){
    std::ofstream csvFile("parsingTable.csv");
    // Write the table content
    csvFile << ",";
    for(const auto &terminal : this->terminals){
        csvFile << terminal << ",";
    }
    csvFile << "\n";
    for (const auto &nonTerminal : this->nonTerminals) {
        csvFile << nonTerminal << ",";
        for (const auto &terminal : terminals) {
            auto it = parsingTable[nonTerminal].find(terminal);
            if(it != parsingTable[nonTerminal].end()) {
                string production = it->second.getLHS() + " --> " +
                                    getRhsExpr(it->second.getRHS()[0]);
                csvFile << production << ',';
            } else{
                csvFile << ",";
            }
        }
        csvFile << "\n";
    }
    // Close the file
    csvFile.close();
}


void Parser::parse(string &token){
    while(true) {
        string term = this->stack.back();
        if(this->isTerminal(term)){
            //case is epsilon pop and continue parsing
            this->stack.pop_back();
            if(term == EPSILON)
                continue;

            // Report Error
            if(term != token){
                file << "Error: missing " + term << endl;
                continue;
            }
            if(term == token){
                if(token != "$") this->parsedTerminals.push_back(token);
                else{
                    this->writeToFile();
                    this->file.close();


                }
                break;
            }
        }
        //NonTerminal case
        //Empty Entry
        if(this->parsingTable[term].find(token) == this->parsingTable[term].end()){
            //discard token
            file << "Error: (illegal " + term + ") -discard " << token << endl;
            return;
        }
        //pop element from stack
        if(this->parsingTable[term][token].getRHS()[0][0] == "Sync"){
            file << "Error: (Sync)" << endl;
            this->stack.pop_back();
        }
        else{
            this->writeToFile();
            this->stack.pop_back();
            vector<string> rhs = this->parsingTable[term][token].getRHS()[0];
            for(int i = rhs.size()-1; i > -1; --i){
                this->stack.push_back(rhs[i]);
            }
        }
    }

}


void Parser::writeToFile(){
    for(int i = 0; i < this->parsedTerminals.size(); i++){
        this->file << this->parsedTerminals[i] << " ";
    }
    for(int i = this->stack.size()-1; i > 0; i--){
        this->file << this->stack[i] << " ";
    }
    this->file << endl;
}

bool Parser::isNonTerminal(string term) {
    return this->nonTerminals.find(term) != this->nonTerminals.end();
}

bool Parser::isTerminal(string term) {
    return this->terminals.find(term) != this->terminals.end();
}







