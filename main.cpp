#include <set>
#include <iostream>
#include <unordered_map>
#include "State.h"
#include "Node.h"

using namespace std;


int main() {
    State s7 = *new State(false, 1, "g");
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

  /*  for(State state: set1){
        std::cout << state.name << endl;
    }*/
    Node node = *new Node(set1);
    Node node2 = *new Node(set2);
    unordered_map<Node,int> map1;
    map1[node] = 1;
    map1[node2] = 2;
    cout << node2.getHash() << endl;
    cout << node.getHash();

}