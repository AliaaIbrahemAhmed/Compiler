#include "State.h"
#include "Node.h"
#include <unordered_map>
#include<stack>
#include<set>

using namespace std;

using TRANSITIONS = unordered_map<string, Node>;
using TRANSITION_TABLE = unordered_map<Node, TRANSITIONS>;
#define EPSILON "\L"

class NFATODFA {

private:
	TRANSITION_TABLE transitionTable;


public:
	TRANSITION_TABLE nfaToDfa(Node start_node);
	explicit NFATODFA(const TRANSITION_TABLE& transtionTable);
	Node e_closure(Node& node);

};

