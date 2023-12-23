//
// Created by Bassant on 23/12/2023.
//

#include "Production.h"

Production::Production() {}

Production::Production(string lhs, vector<vector<string>>& rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}
string Production::getLHS() {return this->lhs;}
vector<vector<string>> Production::getRHS() {return this->rhs;}

void Production::setRHS(vector<vector<string>> rhs) { this->rhs = rhs; }

void Production::addToRHS(vector<vector<string>> anotherRHS){
    this->rhs.insert(this->rhs.end(), anotherRHS.begin(), anotherRHS.end());
}