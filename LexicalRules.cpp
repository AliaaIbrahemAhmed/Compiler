#include "LexicalRules.h"

#include <utility>
#include <sstream>
#include <iostream>

LexicalRules::LexicalRules() = default;

void LexicalRules::addRegularDefinitions(const string &lhs, string rhs) {
    this->regularDefinitions[lhs] = std::move(rhs);
}

void LexicalRules::addRegularExpressions(const string &lhs, string rhs) {
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
    size_t pos = rule.find(':');
    size_t pos2 = rule.find('=');
    if (pos != string::npos && pos < pos2) {
        // Regular Expression
        string lhs = rule.substr(0, pos);
        string rhs = rule.substr(pos + 1);
        this->addRegularExpressions(lhs, rhs);
        return true;
    }
    return false;
}

bool LexicalRules::isRD(string rule) {
    size_t pos = rule.find('=');
    size_t pos2 = rule.find(':');
    if (pos != string::npos && pos < pos2) {
        // Regular Definition
        string lhs = rule.substr(0, pos);
        string rhs = rule.substr(pos + 1);
        this->addRegularDefinitions(lhs, rhs);
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

bool LexicalRules::isPunctuation(string rule) {
    if (rule.find('[') == 0) {
        // Punctuations
        rule = rule.substr(1, rule.size() - 2); // Remove square brackets
        istringstream iss(rule);
        string punctuation;
        while (iss >> punctuation) {
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



