#ifndef COMPILER_NODE_H
#define COMPILER_NODE_H

#include <set>

class Node {
public:
    set <State> states;

    size_t getHash() const;

    // Overridden equality operator
    bool operator==(const Node& other) const;
    void addState(State state);

    explicit Node(const set <State> &states);
    Node() : states{} {}

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
