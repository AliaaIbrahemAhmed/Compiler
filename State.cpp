using namespace std;

#include <string>
#include "State.h"

State::State(bool isEndState, int priority, string name)
        : isEndState(isEndState), priority(priority), name(name) {
    this->isEndState = isEndState;
    this->name = name;
    this->priority = priority;
}


bool State::operator==(const State &other) const {
    return this->name == other.name;
}

bool State::operator<(const State &other) const {
    return this->name < other.name;
}