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
		NFA_TRANSITIONS transitions = this->transitionTable[key];
		//check for epsilon transition		
		auto range = transitions.equal_range(EPSILON);
		for (auto it = range.first; it != range.second; it++) {
			State toState = *it->second.states.begin();
		    //add state if it does not exist in closure.
			if (closure.find(toState) == closure.end()) {
				closure.insert(toState);
				stack.push(toState);
			}
		}
	}
	return *new Node(closure);

};

Node NFATODFA::getRepresentingNode(Node& node, int& count, unordered_map<Node, string>& nameMap, unordered_map<Node, string>& endingMap)
{
	State endingState;
	endingState.priority = -1;
	bool isEndNode = false;
	//string name = "";
	for (State state : node.states) {
		//name += state.name;
		if (!state.isEndState) continue;
		
		isEndNode = true;
		if (state.priority > endingState.priority) {
			endingState = state;
		}
	}
	//return a node containing the state with max priority.
	string name;
	if (nameMap.find(node) != nameMap.end()) {
		name = nameMap[node];
		//cout << "found" << name ;
	}
	else {
		name = "q" + to_string(count++);
		nameMap[node] = name;
	}
	Node newNode = *new Node({ *new State(isEndNode, 0, nameMap[node]) });
	if (isEndNode) {
		endingMap[newNode] = endingState.name;
	}
	
	//return new representing node
	return newNode;
}

DfaResult NFATODFA::nfaToDfa(Node startNode, vector<string>& inputs)
{
	DFA_TRANSITION_TABLE DFA;
	queue<Node> unprocessedStates;
	unordered_set<Node> processedStates;
	Node node = this->eClosure(startNode);
	unprocessedStates.push(node);
	unordered_map<Node, string> nameMap;
	int count = 1;
	nameMap[node] = "q0";
	unordered_map<Node, string> endingMap;
	while (!unprocessedStates.empty()) {
		Node curNode = unprocessedStates.front();
		//mark node as visited
		unprocessedStates.pop();
		if (processedStates.find(curNode) != processedStates.end()) continue;
		processedStates.insert(curNode);

		// find transtion for each state in a node 
		for (string input : inputs) {
			set<State> statesEClosure;
			for (State state : curNode.states) {
				// get transitions of each state in tranisiton table 
				Node key({ state });
				NFA_TRANSITIONS transitions = this->transitionTable[key];
				//skip if a transition does not exist
				auto range = transitions.equal_range(input);
				//iterate over all states 

				for (auto it = range.first; it != range.second; it++) {
					//find e closure of state 
					set<State> stateEClosure = this->eClosure(it->second).states;
					//add e closure to the new state
					statesEClosure.insert(stateEClosure.begin(), stateEClosure.end());
				}
				//add new node to a table if transistions are found
			}
			if (!statesEClosure.empty()) {
				Node toNode = Node(statesEClosure);
				DFA[curNode][input] = toNode;
				/*
				
				Node toNode =  Node(statesEClosure);
				Node reperesentingToNode = getRepresentingNode(toNode, count, nameMap, endingMap);
				Node reperesentingFromNode = getRepresentingNode(curNode, count, nameMap, endingMap);
				DFA[reperesentingFromNode][input] = reperesentingToNode;
				//cout << nameMap[curNode] << "->" << nameMap[toNode] << endl;
				*/

				// add node to unprocessed states if not visited
			
				if (processedStates.find(toNode) == processedStates.end()) {
				//	cout << "new state " << nameMap[toNode] << endl;
					unprocessedStates.push(toNode);
				}
			}
		}
	}
	DfaResult res;
	res.DFA = DFA;
	res.startNode = getRepresentingNode(node, count, nameMap, endingMap);
	res.endMap = endingMap;
	return res;
}

NFATODFA::NFATODFA(const NFA_TRANSITION_TABLE& transtionTable)
{
	this->transitionTable = transtionTable;
}

void NFATODFA::printTransitionTable(const DFA_TRANSITION_TABLE& transtionTable)
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
				std::cout << nextState.name << " " << nextState.isEndState;
			}
			std::cout << "}\n";
		}
	}

};








