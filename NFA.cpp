#include <sstream>
#include <stack>
#include <queue>
#include "NFA.h"
#include <utility>
#include <algorithm>
#include "string"

using namespace std;
using namespace std;
using TRANSITIONS = unordered_multimap<string, Node>;
using TRANSITION_TABLE = unordered_map<Node, TRANSITIONS>;

NFA::NFA(LexicalRules &lexicalRules, Node root) : root(std::move(root)) {
    this->lexicalRules = lexicalRules;
    this->count = 1;
    vector<pair<Node, Node>> nodes, n1, n2, n3, n4;

    n1 = parseRDs();
    nodes.insert(nodes.end(), n1.begin(), n1.end());
    n2 = generateKWsNFA();
    nodes.insert(nodes.end(), n2.begin(), n2.end());

    n3 = generatePunctuationsNFA();
    nodes.insert(nodes.end(), n3.begin(), n3.end());

    n4 = parseREs();
    nodes.insert(nodes.end(), n4.begin(), n4.end());

    this->root = getRoot(nodes);
}

string convertToString(char c) {
    return string(1, c);
}


Node NFA::getRoot(const vector<pair<Node, Node>> &nodes) {
    Node node = *new Node({*new State(false, 0, "0")});
    TRANSITIONS transitions;
    for (const auto &i: nodes) {
        transitions.insert({EPSILON, i.first});
    }
    nfa.insert({node, transitions});
    return node;
}

vector<string> customSplit(const std::string &expression, char symbol) {
    vector<std::string> result;
    string currentPart;
    bool escapeMode = false;
    for (char c: expression) {
        if (c == symbol && !escapeMode) {
            // Found unescaped '|', push the current part and reset
            result.push_back(currentPart);
            currentPart.clear();
        } else if (c == '\\' && !escapeMode) {
            // Start escape mode
            escapeMode = true;
        } else {
            // Regular character, add to the current part
            if (escapeMode) {
                currentPart += '\\';
            }
            currentPart += c;
            escapeMode = false; // Reset escape mode
        }
    }
    // Add the last part
    result.push_back(currentPart);
    return result;
}

pair<Node, Node> NFA::kleeneClosure(const pair<Node, Node> &se) {
    nfa[se.second].insert({EPSILON, se.first});
    nfa[se.first].insert({EPSILON, se.second});
    nfa[se.second].insert({EPSILON, se.first});
    return {se.first, se.second};
}

pair<Node, Node> NFA::positiveClosure(const pair<Node, Node> &se) {
    nfa[se.second].insert({EPSILON, se.first});
    return {se.first, se.second};
}


pair<Node, Node> NFA::concatenation(vector<pair<Node, Node>> nodes) {
    for (int i = 0; i < nodes.size() - 1; i++) {
        nfa[nodes[i].second].insert({EPSILON, nodes[i + 1].first});
    }
    return {nodes[0].first, nodes[nodes.size() - 1].second};
}

pair<Node, Node> NFA::unionOP(const vector<pair<Node, Node>> &nodes) {
    Node s = *new Node({*new State(false, 0, generateNewStateName())});
    Node e = *new Node({*new State(false, 0, generateNewStateName())});
    TRANSITIONS sTransitions, eTransitions;
    for (const auto &node: nodes) {
        sTransitions.insert({EPSILON, node.first});
        nfa[node.second].insert({EPSILON, e});
    }
    nfa.insert({s, sTransitions});
    nfa.insert({e, eTransitions});
    return {s, e};
}


vector<pair<Node, Node>> NFA::generateNFAforKWandP(const set<string> &rules, bool which) {
    unordered_map<string, pair<Node, Node>> res;
    vector<pair<Node, Node>> nodes;
    for (string p: rules) {
        Node pRoot = *new Node({*new State(false, 0, NFA::generateNewStateName())});
        Node prevNode = pRoot;
        for (int i = 0; i < p.size() - 1; i++) {
            TRANSITIONS transitions;
            Node newNode = (*new Node({*new State(false, 0, NFA::generateNewStateName())}));
            string transitionString = convertToString(p[i]);
            transitionSet.insert(transitionString);
            transitions.insert({transitionString, newNode});
            nfa.insert({prevNode, transitions});
            prevNode = newNode;
        }
        TRANSITIONS transitions, lT;
        Node lastNode = (*new Node({*new State(true, 1, p)}));
        string transitionString = convertToString(p[p.size() - 1]);
        transitionSet.insert(transitionString);
        transitions.insert({convertToString(p[p.size() - 1]), lastNode});
        nfa.insert({prevNode, transitions});
        nfa.insert({lastNode, lT});
        res.insert({p, {pRoot, lastNode}});
        if (which) this->KWsNFA.insert({p, {pRoot, lastNode}});
        else this->punctuationsNFA.insert({p, {pRoot, lastNode}});
        nodes.emplace_back(pRoot, lastNode);
    }
    return nodes;
}

vector<pair<Node, Node>> NFA::generateKWsNFA() {
    return generateNFAforKWandP(lexicalRules.keyWords, true);


}

vector<pair<Node, Node>> NFA::generatePunctuationsNFA() {
    return generateNFAforKWandP(lexicalRules.punctuations, false);
}

string NFA::generateNewStateName() {
    string res = to_string(count);
    count += 1;
    return res;
}


vector<pair<Node, Node>> NFA::convertStackToVector(stack<pair<Node, Node>> myStack) {
    vector<pair<Node, Node>> myVector;
    while (!myStack.empty()) {
        myVector.push_back(myStack.top());
        myStack.pop();
    }
    return myVector;
}

pair<Node, Node> NFA::singleNodeNfa(const string &string1) {
    if (string1 == EPSILON) {
        Node s = *new Node({*new State(false, 0, generateNewStateName())});
        Node e = *new Node({*new State(false, 0, generateNewStateName())});
        TRANSITIONS sTransitions, eTransitions;
        sTransitions.insert({EPSILON, e});
        nfa.insert({s, sTransitions});
        nfa.insert({e, eTransitions});
        return {s, e};
    } else {
        Node s = *new Node({*new State(false, 0, generateNewStateName())});
        TRANSITIONS sTransitions, bTransitions;
        Node b = *new Node({*new State(false, 0, generateNewStateName())});
        string transitionString = convertToString(string1[0]);
        transitionSet.insert(transitionString);
        sTransitions.insert({transitionString, b});
        nfa.insert({b, bTransitions});
        Node prevNode = b;
        for (int i = 1; i < string1.size() - 1; i++) {
            Node newNode = *new Node({*new State(false, 0, generateNewStateName())});
            TRANSITIONS nTransitions;
            nfa.insert({newNode, nTransitions});
            transitionString = convertToString(string1[i]);
            transitionSet.insert(transitionString);
            nfa[prevNode].insert({transitionString, newNode});
            prevNode = newNode;
        }
        if (string1.size() > 1) {
            Node newNode = *new Node({*new State(false, 0, generateNewStateName())});
            nfa[prevNode].insert({convertToString(string1[string1.size() - 1]), newNode});
            TRANSITIONS nTransitions;
            nfa.insert({newNode, nTransitions});
            prevNode = newNode;
        }
        nfa.insert({s, sTransitions});
        return {s, prevNode};
    }
}

pair<Node, Node> NFA::range(const string &expression) {
    string withoutSpaces = expression;
    withoutSpaces.erase(remove_if(withoutSpaces.begin(), withoutSpaces.end(), ::isspace), withoutSpaces.end());
    vector<string> parsed = customSplit(withoutSpaces, '-');
    char start = parsed[0][0];
    char end = parsed[1][0];
    vector<pair<Node, Node>> nodes;
    for (char currentChar = start; currentChar <= end; ++currentChar) {
        nodes.push_back(singleNodeNfa(convertToString(currentChar)));
    }
    return unionOP(nodes);

}

bool NFA::isRange(const string &expression) {
    string withoutSpaces = expression;
    withoutSpaces.erase(remove_if(withoutSpaces.begin(), withoutSpaces.end(), ::isspace), withoutSpaces.end());
    vector<string> parsed = customSplit(withoutSpaces, '-');
    if (parsed.size() == 2 && parsed[0].size() == 1 && parsed[1].size() == 1) return true;
    return false;
}

int checkIfRD(string s, LexicalRules lexicalRules) {
    if (s.size() == 0)return 0;
    if (lexicalRules.regularDefinitions.find(s) != lexicalRules.regularDefinitions.end()) return 1;
    string sliced = s;
    sliced.erase(sliced.size() - 1);
    if (lexicalRules.regularDefinitions.find(sliced) != lexicalRules.regularDefinitions.end()) return 2;
    return 0;
}

pair<Node, Node> NFA::parseAND(const string &s) {
    if (isRange(s))return range(s);
    vector<string> parsed = customSplit(s, ' ');
    vector<pair<Node, Node>> ns = *new vector<pair<Node, Node>>;
    for (auto i: parsed) {
        int rd = checkIfRD(i, lexicalRules);
        if (isRange(i)) ns.push_back(range(i));
        else if (rd != 0) {
            if (rd == 2) {
                string sliced = i;
                sliced.erase(sliced.size() - 1);
                pair<Node, Node> res;
                res = parseOr(lexicalRules.regularDefinitions[sliced]);
                if (i.at(i.size() - 1) == '+') {
                    ns.push_back(positiveClosure(res));
                } else if (i.at(i.size() - 1) == '*') {
                    ns.push_back(kleeneClosure(res));
                }
            } else {
                pair<Node, Node> res;
                res = parseOr(lexicalRules.regularDefinitions[i]);
                ns.push_back(res);
            }
        } else if (i.size() > 1 && i.at(i.size() - 1) == '+') {
            ns.push_back(positiveClosure(singleNodeNfa(i.substr(0, i.size() - 1))));
        } else if (i.size() > 1 && i.at(i.size() - 1) == '*') {
            ns.push_back(kleeneClosure(singleNodeNfa(i.substr(0, i.size() - 1))));
        } else if (!i.empty()) {
            ns.push_back(singleNodeNfa(i));
        }
    }
    if (ns.size() == 1) return ns[0];
    return concatenation(ns);
}

pair<Node, Node> NFA::parseOr(const string &s) {
    vector<string> parsed = customSplit(s, '|');
    vector<pair<Node, Node>> nodes;
    for (const auto &i: parsed) {
        nodes.push_back(parseAND(i));
    }
    if (nodes.size() == 1) return nodes[0];
    return unionOP(nodes);
}

pair<Node, Node>
NFA::eatBackward(string first, const pair<Node, Node> &last, stack<pair<Node, Node>> lastParsedAtStack) {
    string withoutSpaces = first;
    withoutSpaces.erase(remove_if(withoutSpaces.begin(), withoutSpaces.end(), ::isspace), withoutSpaces.end());
    if (withoutSpaces == "|") {
        vector<pair<Node, Node>> v;
        v.push_back(lastParsedAtStack.top());
        v.push_back(last);
        if (v.size() == 1) return v[0];
        return unionOP(v);
    }
    bool isOr = false;
    if (first[first.size() - 1] == '|') {
        isOr = true;
    }
    pair<Node, Node> firstParsed = parseOr(first);
    vector<pair<Node, Node>> v;
    v.push_back(firstParsed);
    v.push_back(last);
    if (v.size() == 1) return v[0];
    if (isOr) {
        return unionOP(v);
    } else {
        return concatenation(v);
    }
}

pair<Node, Node> NFA::eatForward(const pair<Node, Node> &first, string last) {
    string withoutSpaces = last;
    withoutSpaces.erase(remove_if(withoutSpaces.begin(), withoutSpaces.end(), ::isspace), withoutSpaces.end());
    if (withoutSpaces == "*") {
        return kleeneClosure(first);
    } else if (withoutSpaces == "+") {
        return positiveClosure(first);

    }
    bool isOr = false;
    if (last[0] == '|') {
        isOr = true;
    }
    pair<Node, Node> lastParsed = parseOr(last);
    vector<pair<Node, Node>> v;
    v.push_back(first);
    v.push_back(lastParsed);
    if (v.size() == 1) return v[0];
    if (isOr) {
        return unionOP(v);
    } else {
        return concatenation(v);
    }
}


pair<Node, Node> NFA::parse(string rule) {
    stack<string> s;
    stack<pair<Node, Node>> nodes;
    bool escapeMood = false;
    int i = 0;
    while (i < rule.size()) {
        char c = rule[i];
        if (c == '\\' && !escapeMood) {
            i++;
            escapeMood = true;
            continue;
        }
        int asciiValue = static_cast<int>(c);
        if (asciiValue == 32 && (s.empty() || s.top().empty())) {
            i++;
            continue;
        }
        if (c == '(' && !escapeMood) {
            if (!s.empty() && s.top().empty()) s.pop();
            s.emplace("(");
            s.emplace("");
        } else if (c == ')' && !escapeMood) {
            if (s.top() == "(") s.pop();
            else {
                string partialRule = s.top();
                s.pop();
                s.pop();
                if (i < rule.size() - 1 && rule[i + 1] == '*') {
                    nodes.push(kleeneClosure(parseOr(partialRule)));
                    i++;
                } else if (i < rule.size() - 1 && rule[i + 1] == '+') {
                    nodes.push(positiveClosure(parseOr(partialRule)));
                    i++;
                } else {
                    nodes.push(parseOr(partialRule));
                }
                pair<Node, Node> lastRes = nodes.top();
                while (!s.empty() && s.top() != "(") {
                    nodes.pop();
                    string topExp = s.top();
                    s.pop();
                    lastRes = eatBackward(topExp, lastRes, nodes);
                    nodes.push(lastRes);
                }
            }
        } else if (escapeMood && c == 'L') {
            string temp = s.top();
            temp.append(EPSILON);
            s.pop();
            s.push(temp);
        } else {
            if (s.empty() || s.top() == "(" && !escapeMood) {
                s.emplace("");
            }
            s.top().push_back(c);
            if (escapeMood && s.top() == "(" || s.top() == ")") {
                s.top().push_back(' ');
            }
        }
        escapeMood = false;
        i++;
    }

    if (!nodes.empty()) {
        pair<Node, Node> lastRes = nodes.top();
        while (!s.empty()) {
            lastRes = eatForward(lastRes, s.top());
            s.pop();
            nodes.pop();
            nodes.push(lastRes);
        }
    } else {
        return parseOr(s.top());
    }
    vector<pair<Node, Node>> v = convertStackToVector(nodes);
    if (v.size() == 1)return v[0];
    return concatenation(v);
}

vector<pair<Node, Node>> NFA::parseREs() {
    vector<pair<Node, Node>> pairs;
    for (const auto &regularExpression: this->lexicalRules.regularExpressions) {
        pair<Node, Node> p = parse(regularExpression.second);
        Node last = *new Node({*new State(true, 0, regularExpression.first)});
        TRANSITIONS t;
        nfa.insert({last, t});
        nfa[p.second].insert({EPSILON, last});
        pair<Node, Node> pairToInsert = *new pair<Node, Node>{p.first, last};
        pairs.push_back(pairToInsert);
        REsNFA.insert({regularExpression.first, pairToInsert});
    }
    return pairs;
}

vector<pair<Node, Node>> NFA::parseRDs() {
    vector<pair<Node, Node>> pairs;
    while (!this->lexicalRules.rdOrder.empty()) {
        string lhs = lexicalRules.rdOrder.top();
        string rhs = lexicalRules.regularDefinitions[lhs];
        pair<Node, Node> p = parse(rhs);
        Node last = *new Node({*new State(true, 1, lhs)});
        TRANSITIONS t;
        nfa.insert({last, t});
        nfa[p.second].insert({EPSILON, last});
        pair<Node, Node> pairToInsert = *new pair<Node, Node>{p.first, last};
        pairs.push_back(pairToInsert);
        regularDefinitionsNFA.insert({lhs, pairToInsert});
        this->lexicalRules.rdOrder.pop();
    }
    return pairs;
}