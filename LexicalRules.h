//
// Created by aliaa on 12/3/23.
//

#ifndef COMPILER_LEXICALRULES_H
#define COMPILER_LEXICALRULES_H


#include <string>
#include <vector>
#include <unordered_map>
#include <set>

using namespace std;
class LexicalRules {
public:
    unordered_map<string, string> regularDefinitions;
    unordered_map<string, string> regularExpressions;
    set<string> keyWords;
    set<string> punctuations;
    set<string> reservedSymbols;

    LexicalRules();

    bool isRegularDefinition(string s);

    bool isReserved(string s);

    string clearReservedBackSlash(string s);

    void addRegularDefinitions(string lhs, string rhs);

    void addRegularExpressions(string lhs, string rhs);

    void addKeyWords(const string &keyWord);

    void addPunctuations(string punctuation);

    bool parseString(string rule);

    bool isRE(string rule);

    bool isRD(string rule);

    bool isKW(string rule);

    bool isPunctuation(string rule);

    void showRDs();

    void showREs();

    void showPunctuations();

    void showKWs();


};


#endif //COMPILER_LEXICALRULES_H
