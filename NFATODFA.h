#include "State.h"
#include "Node.h"
#include <unordered_map>
#include <stack>
#include <set>
#include <iostream>
#include <unordered_set>
#include <queue>

using namespace std;

using NFA_TRANSITIONS = unordered_multimap<string, Node>;
using DFA_TRANSITIONS = unordered_map<string, Node>;
using NFA_TRANSITION_TABLE = unordered_map<Node, NFA_TRANSITIONS>;
using DFA_TRANSITION_TABLE = unordered_map<Node, DFA_TRANSITIONS>;


#define EPSILON "\\L"

class NFATODFA {

private:
	NFA_TRANSITION_TABLE transitionTable;
	Node getRepresentingNode(Node node);


public:
	pair<DFA_TRANSITION_TABLE, Node> nfaToDfa(Node startNode, vector<string>& inputs);
	explicit NFATODFA(const NFA_TRANSITION_TABLE& transtionTable);
	void printTransitionTable(const DFA_TRANSITION_TABLE& transtionTable);
	Node eClosure(Node& node);

};

