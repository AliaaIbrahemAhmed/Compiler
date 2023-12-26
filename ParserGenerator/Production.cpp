//
// Created by Bassant on 23/12/2023.
//

#include <sstream>
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
string Production::Production::toString() {
    stringstream out;
    out << this->lhs<< " ---> ";
    for (unsigned int i = 0; i < this->rhs.size(); ++i) {
        for (string e : this->rhs[i]) {
            out << e << " ";
        }
        if (i != rhs.size() - 1) out << "|| ";
    }
    out << endl;
    return out.str();
}