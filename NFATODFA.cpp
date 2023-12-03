#include "NFATODFA.h"
#include <iostream>
using namespace std;

Node NFATODFA::e_closure(Node& node)
{
	set<State> states = node.states;
	set<State> closure;
	stack<State>stack;
	for (State s : states) {
		closure.insert(s);
		stack.push(s);
	}
	while (!stack.empty()) {
		State fromState = stack.top();
		stack.pop();
		//create key for state
		set<State> statesSet;
		statesSet.insert(fromState);
		Node key(statesSet);
		//get transtion of state
		TRANSITIONS transitions = this->transitionTable[key];
		//check for epsilon transition
		bool x = (this->transitionTable.find(key) != transitionTable.end());
		cout << transitions[EPSILON].getHash() <<  (x) << endl;
		cout << (transitions.find(EPSILON) != transitions.end());
		
		if (transitions.find(EPSILON) != transitions.end()){
			cout << (transitions.find(EPSILON) != transitions.end());
			for (State toState : transitions["\L"].states) {
			//add state if it does not exist in closure.
				if (closure.find(toState) == closure.end()) {
					closure.insert(toState);
					stack.push(toState);
				}
			}
		}
	}
	return Node(closure);

};

TRANSITION_TABLE NFATODFA::nfaToDfa(Node start_node)
{
	return TRANSITION_TABLE();
}

NFATODFA::NFATODFA(const TRANSITION_TABLE& transtionTable)
{
	this->transitionTable = transtionTable;
}




