//
// Created by Salah on 23/12/2023.
//

#include <iostream>
#include "utilities.h"

string extract(string &str) {
    str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
    str.erase(str.find_last_not_of(' ') + 1);         //surfixing spaces
    return str;
}

void removeSpaces(string &str) {
    string str2;
    for (char c: str) {
        if (c == ' ' && str2.back() == ' ') continue;
        str2 += c;
    }
    str = extract(str2);
}

void printErrorMessage(const string& message) {
    cout << "Error: " << message << endl;
}
