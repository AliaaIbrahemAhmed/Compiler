#ifndef COMPILER_INPUT_H
#define COMPILER_INPUT_H

#include <string>
#include <vector>

using namespace std;
class Input {

public:
    string inputFilePath;
    vector<string> inputs;
    int inputSize;
    void getInput();
    void parseInput();
    Input(string inputFilePath);
};


#endif //COMPILER_INPUT_H
