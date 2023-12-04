#include "NFATODFA.h"

using namespace std;

Node NFATODFA::eClosure(Node& node)
{
	set<State> states = node.states;
// Outputting the contents of the set
    for (const auto& state : states) {
        std::cout <<"states"<< &state << " ";
    }	set<State> closure;
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
		if (transitions.find(EPSILON) != transitions.end()){
			for (State toState : transitions[EPSILON].states) {
			//add state if it does not exist in closure.
				if (closure.find(toState) == closure.end()) {
					closure.insert(toState);
					stack.push(toState);
				}
			}
		}
	}
	return *new Node(closure);

};

pair<TRANSITION_TABLE, Node >  NFATODFA::nfaToDfa(Node startNode, vector<string>& inputs)
{
	TRANSITION_TABLE DFA;
	queue<Node> unprocessedStates;
	unordered_set<Node> processedStates;
	Node node = this->eClosure(startNode);
	unprocessedStates.push(node);
	while (!unprocessedStates.empty()) {
		Node curNode = unprocessedStates.front();
		//mark node as visited
		processedStates.insert(curNode);
		unprocessedStates.pop();
		// find transtion for each state in a node 
		for (string input : inputs) {
			set<State> statesEClosure;
			for (State state : curNode.states) {
				// get transitions of each state in tranisiton table 
				Node key({ state });
				TRANSITIONS transitions = this->transitionTable[key];
				//skip if a transition does not exist
				if (transitions.find(input) == transitions.end()) continue;
				//find e closure of state 
				set<State> stateEClosure = this->eClosure(transitions[input]).states;
				//add e closure to the new state
				statesEClosure.insert(stateEClosure.begin(), stateEClosure.end());
				//add new node to a table if transistions are found
			}
			if (!statesEClosure.empty()) {
				Node toNode = *new Node(statesEClosure);
				DFA[curNode][input] = toNode;
				// add node to unprocessed states if not visited
				if (processedStates.find(toNode) == processedStates.end())
					unprocessedStates.push(toNode);
			}
		}
	}
	pair<TRANSITION_TABLE, Node> res = { DFA, node };
	return res;
}

NFATODFA::NFATODFA(const TRANSITION_TABLE& transtionTable)
{
	this->transitionTable = transtionTable;
}

void NFATODFA::printTransitionTable(const TRANSITION_TABLE& transtionTable)
{
	std::cout << "\nDFA Transition Table:\n";
	for (auto it = transtionTable.begin(); it != transtionTable.end(); ++it) {
		for (auto it1 = it->second.begin(); it1 != it->second.end(); ++it1) {
			std::cout << "    ";
			for (State state : it->first.states) {
				cout << state.name << " ";
			}
			std::cout << "on Symbol: " << it1->first << " -> { ";
			for (State nextState : it1->second.states) {
				std::cout << nextState.name << " ";
			}
			std::cout << "}\n";
		}
	}

};






