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
#include <limits>

#include "DFAMinimization.h"


set<string> getInputs(const DFA_TRANSITION_TABLE &dfa) {
    // Iterate over all input symbols available in the DFA transitions
    set<string> inputs;
    for (const auto &nodeEntry: dfa) {
        for (const auto &transition: nodeEntry.second) {
            inputs.insert(transition.first);
        }
    }
    return inputs;
}

DFA_TRANSITIONS getTransitionsOfCurrentNode(const Node &currentNode, const DFA_TRANSITION_TABLE &dfa) {
    std::unordered_map<std::string, Node> transitions;

    auto it = dfa.find(currentNode);
    if (it != dfa.end()) {
        // currentNode found in the outer map, retrieve its transitions
        transitions = it->second;
    }
    return transitions;
}

/*This function responsible for getting the next state of teh given state needed*/
Node move(const Node &currentNode, const string &a, const DFA_TRANSITION_TABLE &dfa) {
    Node nextStates;
    DFA_TRANSITIONS transitions = getTransitionsOfCurrentNode(currentNode, dfa);
    for (const auto &t: transitions) {
        if (a == "," ){
            cout<<"input for move: "<<t.first<<" "<<a<<endl;
            cout<<"ddd: "<<(t.first == a)<<endl;}
            if (t.first == a) {
           // cout<<"input for move: "<<t.first<<endl;
            nextStates.addState(*t.second.states.begin());
        }
    }
    return nextStates;
}

/*This function responsible for getting the next state of teh given state needed*/
set<State> getStates(const unordered_set<Node> &groups) {
    set<State> groupStates;
    for (const auto &nodeTemp: groups) {
        groupStates.insert(*nodeTemp.states.begin());
    }
    return groupStates;
}

/*This function responsible for return whether the states ar going to the partitions or not*/
bool goToSameGroup(vector<unordered_set<Node>> &groups, const Node &a, const Node &b, const string &s,
                   const DFA_TRANSITION_TABLE &dfa) {
    Node nextA = move(a, s, dfa);
    Node nextB = move(b, s, dfa);
    if (nextA.states.empty() && nextB.states.empty()) {
        return true;
    } else if ((!nextA.states.empty() && nextB.states.empty()) || (nextA.states.empty() && !nextB.states.empty())) {
        return false;
    } else {
        State nexta = *(nextA.states.begin());
        State nextb = *(nextB.states.begin());
        /*here the iteration took  group of nodes to search in them on the state we translated to
         * nodeTemp --> contains the elements of "Accepting" subgroup and same for "nonAccepting"*/
        for (auto &subgroup: groups) {
            set<State> states = getStates(subgroup);
            if (states.find(nexta) != states.end()
                && states.find(nextb) != states.end()) {
                return true;
            }
        }
    }
    return false;
}

vector<unordered_set<Node>>
refinePartition(vector<unordered_set<Node>> &groups, const string &x, const DFA_TRANSITION_TABLE &dfa) {
    vector<unordered_set<Node>> newGroups;
    for (const unordered_set<Node> &T: groups) {
        unordered_map<Node, bool> markedStates;
        for (const Node &s: T) {
            markedStates[s] = false;
        }
        vector<Node> nodes(T.begin(), T.end());  // Convert unordered_set to vector for indexing
        for (size_t it = 0; it < nodes.size(); ++it) {
            if (markedStates[nodes[it]]) continue;
            unordered_set<Node> subgroup;
            subgroup.insert(nodes[it]);
            markedStates[nodes[it]] = true;
            /*this part is wrong as iterator jt has to have element after it 3la tool*/
            for (size_t jt = it+1; jt < nodes.size(); ++jt) {
                if (markedStates[nodes[jt]]) continue;
                if (goToSameGroup(groups, nodes[it], nodes[jt], x, dfa)) {
                    subgroup.insert(nodes[jt]);
                    markedStates[nodes[jt]] = true;
                }
            }
            newGroups.push_back(subgroup);
        }
    }
    return newGroups;
}

Node getRepresentativeNode(const unordered_set<Node> &T) {
    Node repState;
    //if (!T.begin()->states.begin()->isEndState)
         repState = *T.begin();
   // else {
     //   int maxPriority = numeric_limits<int>::min(); // Initialize with minimum value
       // for (const Node &node: T) {
         //   for (const State &state: node.states) {
           //     if (state.priority > maxPriority) {
             //       maxPriority = state.priority;
               //      repState = node; // Update repState with the node having max priority state
                //}
           // }
        //}
    //}
    return repState;
}

// return the newStart Node and the Final transition table
DfaResult
mappingTransitions(const vector<unordered_set<Node>> &groups, const DFA_TRANSITION_TABLE &dfa,
                   unordered_map<Node, string> &endingMap, const Node &startNode) {
    unordered_map<Node, Node> representativeStates;
    unordered_map<Node, string> finalEndMap;
    for (const auto &T: groups) {
        //Renaming for the states will be the first element in the partition
        Node repState = *T.begin();
        representativeStates[repState] = repState;
        for (auto it = next(T.begin(), 1); it != T.end(); ++it) {
            representativeStates[*it] = repState;
        }
    }

    DFA_TRANSITION_TABLE finalTransitions;
    for (const auto &T: groups) {
        Node repState = representativeStates[getRepresentativeNode(T)];
        DFA_TRANSITIONS transitions = dfa.at(repState);
        for (const auto &tran: transitions) {
            DFA_TRANSITIONS temp;
            if (repState.states.begin()->isEndState) {
                finalEndMap[repState] = endingMap[repState];
                //cout << "final ending state "<<finalEndMap[repState]<<"\n"<< "final ending state "<<endingMap[repState]<<"\n";
            }
            temp[tran.first] = representativeStates[tran.second];
            // Insert temp transition into finalTransitions for repState without overwriting existing entries
            finalTransitions[repState].insert(temp.begin(), temp.end());
        }

    }
    DfaResult finalMapping;
    finalMapping.DFA = finalTransitions;
    finalMapping.startNode = representativeStates[startNode];
    finalMapping.endMap=endingMap;
    //pair<DFA_TRANSITION_TABLE, Node> res = make_pair(finalTransitions, representativeStates[startNode]);
    //endingMap = finalTransitions;
    return finalMapping;
}

// Function to mark reachable states from the initial state (DFS traversal)
void markReachableStates(const Node &initialState, std::unordered_set<Node> &reachableStates,
                         const DFA_TRANSITION_TABLE &dfaTransitions) {
    std::unordered_set<Node> visited;
    std::stack<Node> stack;
    stack.push(initialState);

    while (!stack.empty()) {
        Node current = stack.top();
        stack.pop();

        if (visited.find(current) == visited.end()) {
            visited.insert(current);
            reachableStates.insert(current);

            // Check transitions from the current state
            auto it = dfaTransitions.find(current);
            if (it != dfaTransitions.end()) {
                for (const auto &transition: it->second) {
                    Node nextState = transition.second;
                    stack.push(nextState);
                }
            }
        }
    }
}

DFA_TRANSITION_TABLE removeDeadStates(DFA_TRANSITION_TABLE &dfaTransitions, const Node &initialState) {
    unordered_set<Node> reachableStates;
    markReachableStates(initialState, reachableStates, dfaTransitions);

    // Create a new transition table to hold the updated transitions
    DFA_TRANSITION_TABLE updatedTransitions;

    // Copy transitions for reachable states to the updated table
    for (const auto &entry: dfaTransitions) {
        if (reachableStates.find(entry.first) != reachableStates.end()) {
            updatedTransitions.insert(entry);
        }
    }
    // Replace the original table with the updated transitions
    dfaTransitions = updatedTransitions;
    return dfaTransitions;
}

DfaResult DFAMinimization::minimization(const DfaResult& transitonMap) {
    DFA_TRANSITION_TABLE dfa = transitonMap.DFA;
    unordered_map<Node, string> endMap = transitonMap.endMap;
    Node startNode = transitonMap.startNode;
    vector<unordered_set<Node>> groups;
    unordered_map<string, unordered_set<Node>> subgroups;
    DFA_TRANSITION_TABLE minimizedTransitionTable;

    for (const auto &transition: dfa) {
        Node currentNode = transition.first;
        if (currentNode.states.begin()->isEndState) {
            subgroups["Accepting"].insert(currentNode);
        } else {
            subgroups["NonAccepting"].insert(currentNode);
        }
    }
    // Print the contents of the subgroups map
    /**for (const auto &subgroup: subgroups) {
        std::cout << "Subgroup: " << subgroup.first << std::endl;
        std::cout << "States: ";
        for (const auto &node: subgroup.second) {
            std::cout << node.states.begin()->name << " ";
        }
        std::cout << std::endl;
    }**/

    for (const auto &entry: subgroups) {
        groups.push_back(entry.second);
    }
    bool changed = true;
    set<string> inputSymbols = getInputs(dfa);
//for(auto x: inputSymbols) cout<<"input:" <<  x<<"\n";
    //cout<<"inputsize:" << inputSymbols.size()<<"\n";
    // Node nextState = move(startNode,*inputSymbols.begin(),dfa);
    while (changed) {
        vector<unordered_set<Node>> newGroups(groups.begin(), groups.end());
        for (const string &x: inputSymbols) {
            //cout<<"inputentered:" << x<<"\n";
            newGroups = refinePartition(newGroups, x, dfa);
        }
        if (newGroups == groups) {
            changed = false;
        }
        groups = newGroups;
    }
    std::cout << "size1: " << groups.size() << std::endl;
    DfaResult finalRes = mappingTransitions(groups, dfa, endMap, startNode);
    DFA_TRANSITION_TABLE temp = finalRes.DFA;
    std::cout << "size1: " << finalRes.DFA.size() << std::endl;
    // have to change to DFA result yet
    finalRes.DFA = removeDeadStates(temp, startNode);
    std::cout << "size12: " << finalRes.DFA.size() << std::endl;

    // Print the contents of the subgroups map
    /**for (const auto &subgroup: subgroups) {
        std::cout << "Subgroup: " << subgroup.first << std::endl;
        std::cout << "States: ";
        for (const auto &node: subgroup.second) {
            std::cout << node.states.begin()->name << " ";
        }
        std::cout << std::endl;
    }**/
    return finalRes;
}
