#ifndef COMPILER_NODE_H
#define COMPILER_NODE_H

#include <set>
#include <vector>
#include "State.h"

using namespace std;
class Node {
public:
    set <State> states;
    size_t getHash() const;

    // Overridden equality operator
    bool operator==(const Node& other) const;
    bool operator<(const Node& other) const;

    void addState(State state);

    Node(const set <State> &states);

};
template <>
struct std::hash<Node>
{
    std::size_t operator()(const Node& node) const
    {
        return node.getHash();
    }
};

#endif //COMPILER_NODE_H
