#include "LexicalRules.h"

#include <utility>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "string"

using namespace std;

LexicalRules::LexicalRules() {
    this->reservedSymbols = {"*", "(", ")", "+", "-", "L", "|"};
}

string clearSpaces(string s) {
    string withoutSpaces = s;
    withoutSpaces.erase(remove_if(withoutSpaces.begin(), withoutSpaces.end(), ::isspace), withoutSpaces.end());
    return withoutSpaces;
}

void LexicalRules::addRegularDefinitions(string lhs, string rhs) {
    //lhs.erase(std::remove_if(lhs.begin(), lhs.end(), ::isspace), lhs.end());
    this->regularDefinitions[lhs] = std::move(rhs);
}

void LexicalRules::addRegularExpressions(string lhs, string rhs) {
    //lhs.erase(std::remove_if(lhs.begin(), lhs.end(), ::isspace), lhs.end());
    this->regularExpressions[lhs] = std::move(rhs);
}

void LexicalRules::addKeyWords(const string &keyWord) {
    this->keyWords.insert(keyWord);
}

void LexicalRules::addPunctuations(string punctuation) {
    this->punctuations.insert(punctuation);
}


bool LexicalRules::parseString(string rule) {
    if (isRE(rule)) return true;
    if (isRD(rule)) return true;
    if (isKW(rule)) return true;
    if (isPunctuation(rule)) return true;
    return false;
}


bool LexicalRules::isRE(string rule) {
    // rule.erase(std::remove_if(rule.begin(), rule.end(), ::isspace), rule.end());
    size_t pos = rule.find(':');
    size_t pos2 = rule.find('=');
    if (pos != string::npos && pos < pos2) {
        // Regular Expression
        string lhs = rule.substr(0, pos);
        string rhs = rule.substr(pos + 1);
        this->addRegularExpressions(clearSpaces(lhs), rhs);
        return true;
    }
    return false;
}

bool LexicalRules::isRD(string rule) {
    // rule.erase(std::remove_if(rule.begin(), rule.end(), ::isspace), rule.end());
    size_t pos = rule.find('=');
    size_t pos2 = rule.find(':');
    if (pos != string::npos && pos < pos2) {
        // Regular Definition
        string lhs = rule.substr(0, pos);
        string rhs = rule.substr(pos + 1);

        this->addRegularDefinitions(clearSpaces(lhs), rhs);
        this->rdOrder.push(clearSpaces(lhs));
        return true;
    }
    return false;
}

bool LexicalRules::isKW(string rule) {
    if (rule.find('{') == 0) {
        // Keywords
        rule = rule.substr(1, rule.size() - 2); // Remove curly braces
        istringstream iss(rule);
        string keyword;
        while (iss >> keyword) {
            this->addKeyWords(keyword);
        }
        return true;
    }
    return false;
}

bool LexicalRules::isReserved(string s) {
    int i = 0;
    while (i < s.size()) {
        if (s[i] == '\\' && this->reservedSymbols.find((to_string(s[i + 1]))) != this->reservedSymbols.end()) {
            i += 2;
            continue;
        } else if (s[i] == ' ') {
            i += 1;
            continue;
        } else {
            return false;
        }
        return true;
    }
}


bool LexicalRules::isPunctuation(string rule) {
    if (rule.find('[') == 0) {
        // Punctuations
        rule = rule.substr(1, rule.size() - 2); // Remove square brackets
        istringstream iss(rule);
        string punctuation;
        while (iss >> punctuation) {
            punctuation.erase(std::remove(punctuation.begin(), punctuation.end(), '\\'), punctuation.end());
            this->addPunctuations(punctuation);
        }
        return true;
    }
    return false;
}

void LexicalRules::showRDs() {
    for (auto &regularDefinition: this->regularDefinitions) {
        cout << "Token: " << regularDefinition.first << ", Regular Definition: " << regularDefinition.second
             << std::endl;
    }
}

void LexicalRules::showREs() {
    for (auto &regularExpression: this->regularExpressions) {
        cout << "Token: " << regularExpression.first << ", Regular Expression: " << regularExpression.second
             << std::endl;
    }
}

void LexicalRules::showPunctuations() {
    for (auto &punctuation: this->punctuations) {
        cout << "Punctuation:  " << punctuation << endl;
    }
}

void LexicalRules::showKWs() {
    for (auto &kw: this->keyWords) {
        cout << "KeyWord:  " << kw << endl;
    }
}

bool LexicalRules::isRegularDefinition(string s) {
    return this->regularDefinitions.find(s) != this->regularDefinitions.end();
}




