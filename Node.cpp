#include <string>
#include  "State.h"
#include "Node.h"


using namespace std;

size_t Node::getHash() const {
    string statesNames;
    for (State state: this->states) {
        statesNames += state.name;
    }
    return std::hash<string>()(statesNames);
}

bool Node::operator==(const Node &other) const {
    return this->getHash() == other.getHash();
}/*
bool Node::operator<(const Node &other) const {
    return this->states.size() <= other.states.size();
}
*/


void Node::addState(State state) {
    this->states.insert(state);

}

Node::Node(set<State> states) : states(states) {
    this->states = states;
}

