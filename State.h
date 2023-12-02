#ifndef COMPILER_STATE_H
#define COMPILER_STATE_H

#include <string>

using namespace std;
class State {
public:
    State(bool isEndState, int priority, string name);

    bool isEndState;
    int priority;
    string name;

    bool operator==(const State& other) const;
    bool operator<(const State& other) const;
};


#endif //COMPILER_STATE_H
