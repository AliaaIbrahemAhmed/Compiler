#include "State.h"
#include "Node.h"
#include <unordered_map>
#include<stack>
#include<set>
#include <iostream>
#include <unordered_set>
#include <queue>

using namespace std;

using TRANSITIONS = unordered_map<string, Node>;
using TRANSITION_TABLE = unordered_map<Node, TRANSITIONS>;
#define EPSILON "\L"

class NFATODFA {

private:
	TRANSITION_TABLE transitionTable;


public:
	pair<TRANSITION_TABLE, Node> nfaToDfa(Node startNode, vector<string>& inputs);
	explicit NFATODFA(const TRANSITION_TABLE& transtionTable);
	void printTransitionTable(const TRANSITION_TABLE& transtionTable);
	Node eClosure(Node& node);

};

