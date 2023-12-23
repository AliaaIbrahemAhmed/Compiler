#include "NonTerminal.h"

using namespace std;

// Constructor implementation
NonTerminal::NonTerminal() {}

NonTerminal::NonTerminal(string valueKey) {
    this->key = valueKey;
}
// Function to retrieve the key
string NonTerminal::getId() const {
    return key;
}
