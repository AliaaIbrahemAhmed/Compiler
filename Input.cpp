#include "Input.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;


Input::Input(string inputFilePath) {
    this->inputFilePath = inputFilePath;
    this->getInput();
    this->inputSize = 0;

}

void Input::getInput() {

    std::ifstream inputFile(this->inputFilePath);

    if (!inputFile.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        inputs.push_back(line);
        inputSize++;
    }

    inputFile.close();
    for (int i = 0; i < inputSize; i++) {
        cout << inputs[i] << endl;
    }

}

void Input::parseInput() {

}

