#include <set>
#include <iostream>
#include <unordered_map>
#include "State.h"
#include "Node.h"
#include "DFAMinimization.h"

using namespace std;


int main() {
    /**State s7 = *new State(false, 1, "g");
    State s1 = *new State(false, 1, "a");
    State s4 = *new State(false, 1, "d");
    State s2 = *new State(false, 1, "b");
    State s6 = *new State(false, 1, "f");
    State s5 = *new State(false, 1, "e");
    State s3 = *new State(false, 1, "c");

    set<State> set1;
    set<State> set2;
    set1.insert(s7);
    set1.insert(s1);
    set1.insert(s4);
    set1.insert(s2);
    set2.insert(s6);
    set2.insert(s5);
    set2.insert(s3);

    for(State state: set1){
        std::cout <<"Set1: "<< state.name << endl;
    }
    Node node = *new Node(set1);
    Node node2 = *new Node(set2);
    unordered_map<Node,int> map1;
    map1[node] = 1;
    map1[node2] = 2;
    cout << node2.getHash() << endl;
    cout << node.getHash();**/
    // Creating nodes
    Node nodeA({State(false, 1, "A")});
    Node nodeB({State(false, 1, "B")});
    Node nodeC({State(false, 2, "C")});

    // Creating transition table
    TRANSITION_TABLE transitionTable;
    transitionTable[nodeA] = {{"a", nodeB}};
    transitionTable[nodeB] = {{"1", nodeC}};
    transitionTable[nodeC] = {{"l", nodeA}};

    // Creating a start node
    Node startNode({State(false, 1, "A")});

    // Assigning values to the pair
    std::pair<TRANSITION_TABLE, Node> transitionPair;
    transitionPair.first = transitionTable;
    transitionPair.second = startNode;

    // Iterating and printing the transition table
    TRANSITION_TABLE& table = transitionPair.first;
    Node& start = transitionPair.second;

    std::cout << "Transition Table: \n";
    string x = "l";
    for (const auto& state : table[nodeC][x].states){
        std::cout<<state.name<<endl;
    }

        for (const auto& entry : table) {
        Node currentNode = entry.first;
        unordered_map<basic_string<char>, Node> transitions = entry.second;

        std::cout << "From Node: ";
        for (const auto& state : currentNode.states) {
            std::cout << state.name << " ";
        }
        std::cout << "\n";

        for (const auto& transition : transitions) {
            std::cout << "  On input " << transition.first << " goes to state: ";
            for (const auto& state : transition.second.states) {
                std::cout << state.name << " ";
            }
            std::cout << std::endl;
        }
    }

    return 0;

}