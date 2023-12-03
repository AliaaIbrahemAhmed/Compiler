#include "Input.h"
#include <iostream>
#include <fstream>
#include <string>
#include <utility>

using namespace std;


Input::Input(string inputFilePath) {
    this->inputFilePath = std::move(inputFilePath);
    this->inputSize = 0;
    this->lexicalRules = *new LexicalRules();
    this->getInput();

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
        if (!this->lexicalRules.parseString(line)) {
            std::cerr << "Invalid Latex Rule" << std::endl;
            return;
        }
        inputSize++;
    }
    this->lexicalRules.showREs();
    this->lexicalRules.showRDs();
    this->lexicalRules.showKWs();
    this->lexicalRules.showPunctuations();


    inputFile.close();
}

