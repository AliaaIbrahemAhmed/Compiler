#include "Terminal.h"
using namespace std;

// Constructor implementation
Terminal::Terminal() {}

Terminal::Terminal(string valueKey) {
    this->key = valueKey;
}
// Function to retrieve the key
string Terminal::getId() const {
    return key;
}
