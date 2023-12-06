//
// Created by Bassant on 04/12/2023.
//
#include "State.h"
#include "Node.h"
#include <unordered_map>
#include<stack>
#include<set>
#include <iostream>
#include <unordered_set>
#include <queue>
#include <algorithm>

#include "DFAMinimization.h"


set<string> getinputs(const DFA_TRANSITION_TABLE &dfa) {
    // Iterate over all input symbols available in the DFA transitions
    set<string> inputs;
    for (const auto &nodeEntry: dfa) {
        for (const auto &transition: nodeEntry.second) {
            inputs.insert(transition.first);
        }
    }
    return inputs;
}

std::unordered_map<std::string, Node>
getTransitionsOfCurrentNode(const Node &currentNode, const DFA_TRANSITION_TABLE &dfa) {
    std::unordered_map<std::string, Node> transitions;

    auto it = dfa.find(currentNode);
    if (it != dfa.end()) {
        // currentNode found in the outer map, retrieve its transitions
        transitions = it->second;
    }
    return transitions;
}

/*This function responsible for getting the next state of teh given state needed*/
Node move(const Node& currentNode, const string& a, const DFA_TRANSITION_TABLE &dfa) {
    Node nextStates;
    DFA_TRANSITIONS transitions = getTransitionsOfCurrentNode(currentNode, dfa);
    for (const auto& t: transitions) {
        if (t.first == a) {
            nextStates.addState(*t.second.states.begin());
        }
    }
    return nextStates;
}
/*This function responsible for getting the next state of teh given state needed*/
set<State> getStates(const unordered_set<Node> &groups) {
    set<State> groupStates;
    for (const auto& nodeTemp: groups) {
       groupStates.insert(*nodeTemp.states.begin());
    }
    return groupStates;
}

/*This function responsible for return whether the states ar going to the partitions or not*/
bool /*DFAMinimization::*/goToSameGroup(vector<unordered_set<Node>> &groups, Node a, const Node b, string s, const DFA_TRANSITION_TABLE &dfa) {
    Node nextA = move(a, s, dfa);
    Node nextB = move(b, s, dfa);
    if (nextA.states.empty() && nextB.states.empty()) {
        cout << "true as there is no next state" << "\n";
        return true;
    } else if ((!nextA.states.empty() && nextB.states.empty()) || (nextA.states.empty() && !nextB.states.empty())) {
        cout << "false as there aren't in the same partition" << "\n";
        return false;
    } else {
        cout << "true as there is intersection which means they are in the same partition" << "\n";
        State nexta = *(nextA.states.begin());
        State nextb = *(nextB.states.begin());
        cout << "State A: " << nexta.name << "\n";
        cout << "State B: " << nextb.name << "\n";
        cout << "group size: " << groups.size() << "\n";
        /*here the iteration took  group of nodes to search in them on the state we translated to
         * nodeTemp --> contains the elements of "Accepting" subgroup and same for "nonAccepting"*/
        for (auto& subgroup: groups) {
                set<State> states = getStates(subgroup);
                if (states.find(nexta) != states.end()
                && states.find(nextb) != states.end()) {
                    return true;
            }
        }
    }
    return false;
}

vector<unordered_set<Node>> refinePartition(vector<unordered_set<Node>> &groups, string x, const DFA_TRANSITION_TABLE &dfa) {
    vector<unordered_set<Node>> newGroups;
    for (const unordered_set<Node>& T: groups) {
        // cout << "Current Group: " << printStates(T);
        unordered_map<Node, bool> markedStates;
        for (Node s: T) {
            markedStates[s] = false;
        }
        for (const auto &it: T) {
            if (markedStates[it]) continue;
            unordered_set<Node> subgroup;
            subgroup.insert(it);
            markedStates[it] = true;
            /*this part is wrong as iterator jt has to have element after it 3la tool*/
            for (const auto &jt: T) {
                if (markedStates[jt]) continue;
                if (goToSameGroup(groups, it, jt, x,dfa)) {
                    subgroup.insert(jt);
                    markedStates[jt] = true;
                }
            }
            newGroups.push_back(subgroup);
        }
    }
    return newGroups;
}
void mappingTransitions(vector<unordered_set<Node>> groups, const DFA_TRANSITION_TABLE &dfa){
    unordered_map<Node, Node> representativeStates;
    unordered_set<Node> DNodes;
    for (const auto& T : groups) {
        //Renaming for the states will be the first element in the partition
        Node repState = *T.begin();
        representativeStates[repState] = repState;
        for (auto it = next(T.begin(), 1); it != T.end(); ++it) {
            representativeStates[*it] = repState;
        }
    }
    // Printing key-value pairs of representativeStates
    for (const auto& pair : representativeStates) {
        std::cout << "Key: " << pair.first.states.begin()->name << " Value: " << pair.second.states.begin()->name << std::endl;
    }

    for (auto s : representativeStates) DNodes.insert(s.second);

    DFA_TRANSITION_TABLE finalTransitions;
   for (const auto& T : groups) {
        Node repState = representativeStates[*T.begin()];
        DFA_TRANSITIONS transitions = dfa;
        for (auto tran : ) {
            finalTransitions[repState].insert(new Transation(repState, representativeStates[tran->to], tran->condition));
        }
    }
    Dtransitions = finalTransitions;/
}
pair<DFA_TRANSITION_TABLE, Node> DFAMinimization::minimization(const DFA_TRANSITION_TABLE &dfa) {
    vector<unordered_set<Node>> groups;
    unordered_map<string, unordered_set<Node>> subgroups;
    pair<DFA_TRANSITION_TABLE, Node> minimizedTransitionTable;
    for (const auto &transition: dfa) {
        Node currentNode = transition.first;
        if (currentNode.states.begin()->isEndState) {
            subgroups["Accepting"].insert(currentNode);
        } else {
            subgroups["NonAccepting"].insert(currentNode);
        }
    }
    // Print the contents of the subgroups map
    for (const auto &subgroup: subgroups) {
        std::cout << "Subgroup: " << subgroup.first << std::endl;
        std::cout << "States: ";
        for (const auto &node: subgroup.second) {
            std::cout << node.states.begin()->name << " ";
        }
        std::cout << std::endl;
    }

    for (auto entry: subgroups) {
        groups.push_back(entry.second);
    }
    bool changed = true;
    set<string> inputSymbols = getinputs(dfa);
   // Node nextState = move(startNode,*inputSymbols.begin(),dfa);
    while (changed) {
        vector<unordered_set<Node>> newGroups(groups.begin(), groups.end());
        for (string x: inputSymbols) {
            newGroups = refinePartition(newGroups, x, dfa);
        }
        if (newGroups == groups) {
            changed = false;
        }
        groups = newGroups;
    }
    cout<<groups.size()<<"\n";
    for (const auto& unorderedSet : groups) {
        std::cout << "Set: ";
        for (const auto& node : unorderedSet) {
            std::cout << node.states.begin()->name<< " "; // Adjust this based on your Node structure
        }
        std::cout << std::endl;
    }
    mappingTransitions(groups);
    return minimizedTransitionTable;
}
