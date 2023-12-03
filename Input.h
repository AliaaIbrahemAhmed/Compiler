#ifndef COMPILER_INPUT_H
#define COMPILER_INPUT_H

#include <string>
#include <vector>
#include "LexicalRules.h"

using namespace std;
class Input {

public:
    string inputFilePath;
    vector<string> inputs;
    LexicalRules lexicalRules;
    int inputSize;
    void getInput();
    Input(string inputFilePath);
};


#endif //COMPILER_INPUT_H
