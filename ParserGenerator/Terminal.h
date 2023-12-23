#ifndef COMPILER_TERMINAL_H
#define COMPILER_TERMINAL_H

#include <string>
using namespace std;
class Terminal {
private:
    std::string key;

public:
    Terminal();
    Terminal(string valueKey);

    std::string getId() const; // Function to retrieve the key
};

#endif // COMPILER_TERMINAL_H
