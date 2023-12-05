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
#define EPSILON "\\L"

NFA::NFA(LexicalRules &lexicalRules, Node root) : root(std::move(root)) {
    this->lexicalRules = lexicalRules;
    this->base = 'a';
    this->count = 0;
    vector<pair<Node, Node>> nodes, n1, n2, n3, n4;

    n1 = parseRDs(nodes);
    nodes.insert(nodes.end(), n1.begin(), n1.end());
    n2 = generateKWsNFA();
    nodes.insert(nodes.end(), n2.begin(), n2.end());

    n3 = generatePunctuationsNFA();
    nodes.insert(nodes.end(), n3.begin(), n3.end());

    n4 = parseREs(nodes);
    nodes.insert(nodes.end(), n4.begin(), n4.end());

    this->root = getRoot(nodes);
}

Node NFA::getRoot(const vector<pair<Node, Node>> &nodes) {
    Node root = *new Node({*new State(false, 0, generateNewStateName())});
    TRANSITIONS transitions;
    for (auto i: nodes) {
        transitions.insert({EPSILON, i.first});
    }
    nfa.insert({root, transitions});
    return root;
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

pair<Node, Node> NFA::kleeneClosure(const pair<Node, Node> &se, bool isEndState, int priority) {
    Node s = *new Node({*new State(false, 0, generateNewStateName())});
    Node e = *new Node({*new State(isEndState, priority, generateNewStateName())});
    TRANSITIONS sTransitions, eTransitions;
    sTransitions.insert({EPSILON, se.first});
    sTransitions.insert({EPSILON, e});
    nfa[se.second].insert({EPSILON, e});
    nfa[se.second].insert({EPSILON, se.first});
    nfa.insert({s, sTransitions});
    nfa.insert({e, eTransitions});
    return {s, e};
}

pair<Node, Node> NFA::positiveClosure(const pair<Node, Node> &se, bool isEndState, int priority) {
    Node s = *new Node({*new State(false, 0, generateNewStateName())});
    Node e = *new Node({*new State(isEndState, priority, generateNewStateName())});
    TRANSITIONS sTransitions, eTransitions;
    sTransitions.insert({EPSILON, se.first});
    nfa[se.second].insert({EPSILON, e});
    nfa[se.second].insert({EPSILON, se.first});
    nfa.insert({s, sTransitions});
    nfa.insert({e, eTransitions});
    return {s, e};
}


pair<Node, Node> NFA::concatenation(vector<pair<Node, Node>> nodes, bool isEndState, int priority) {
    Node s = *new Node({*new State(false, 0, generateNewStateName())});
    Node e = *new Node({*new State(isEndState, priority, generateNewStateName())});
    TRANSITIONS sTransitions, eTransitions;
    sTransitions.insert({EPSILON, nodes[0].first});
    for (int i = 0; i < nodes.size() - 1; i++) {
        nfa[nodes[i].second].insert({EPSILON, nodes[i + 1].first});
    }
    nfa[nodes[nodes.size() - 1].second].insert({EPSILON, e});
    nfa.insert({s, sTransitions});
    nfa.insert({e, eTransitions});
    return {s, e};
}

pair<Node, Node> NFA::unionOP(const vector<pair<Node, Node>> &nodes, bool isEndState, int priority) {
    Node s = *new Node({*new State(false, 0, generateNewStateName())});
    Node e = *new Node({*new State(isEndState, priority, generateNewStateName())});
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
            transitions.insert({to_string(p[i]), newNode});
            nfa.insert({prevNode, transitions});
            prevNode = newNode;
        }
        TRANSITIONS transitions, lT;
        Node lastNode = (*new Node({*new State(true, 1, p)}));
        transitions.insert({to_string(p[p.size() - 1]), lastNode});
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
    string res = string(1, base) + to_string(count);
    base += 1;
    base %= 26;
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

pair<Node, Node> NFA::singleNodeNfa(const string &string1, bool isEndState, int priority) {
    if (string1 == EPSILON) {
        Node s = *new Node({*new State(false, 0, generateNewStateName())});
        Node e = *new Node({*new State(isEndState, priority, string1)});
        TRANSITIONS sTransitions, eTransitions;
        sTransitions.insert({EPSILON, e});
        nfa.insert({s, sTransitions});
        nfa.insert({e, eTransitions});
        return {s, e};
    } else {
        Node s = *new Node({*new State(false, 0, generateNewStateName())});
        Node e = *new Node({*new State(isEndState, priority, string1)});
        TRANSITIONS sTransitions, eTransitions, bTransitions;
        Node b = *new Node({*new State(false, 0, generateNewStateName())});
        sTransitions.insert({to_string(string1[0]), b});
        nfa.insert({b, bTransitions});
        Node prevNode = b;
        for (int i = 0; i < string1.size() - 1; i++) {
            Node newNode = *new Node({*new State(false, 0, generateNewStateName())});
            TRANSITIONS nTransitions;
            nfa.insert({newNode, nTransitions});
            nfa[prevNode].insert({to_string(string1[i]), newNode});
            prevNode = newNode;
        }
        Node newNode = *new Node({*new State(isEndState, priority, string1)});
        nfa[prevNode].insert({to_string(string1[string1.size() - 1]), newNode});
        TRANSITIONS nTransitions;
        nfa.insert({newNode, nTransitions});
        prevNode = newNode;
        nfa[prevNode].insert({EPSILON, e});
        nfa.insert({s, sTransitions});
        nfa.insert({e, eTransitions});
        return {s, e};
    }
}


bool checkIfReserved(const LexicalRules &lexicalRules, const string &s) {
    if (s[0] != '\\') return false;
    string remaining = s;
    remaining = remaining.erase(0, 1);
    for (const auto &i: lexicalRules.reservedSymbols) {
        if (remaining == i) return true;
    }
    return false;
}

pair<Node, Node> NFA::range(const string &expression, bool isEndState, int priority) {
    vector<string> parsed = customSplit(expression, '-');
    char start = parsed[0][0];
    char end = parsed[1][0];
    vector<pair<Node, Node>> nodes;
    for (char currentChar = start; currentChar <= end; ++currentChar) {
        nodes.push_back(singleNodeNfa(to_string(currentChar), false, 0));
    }
    return unionOP(nodes, isEndState, priority);

}

bool NFA::isRange(const string &expression) {
    vector<string> parsed = customSplit(expression, '-');
    if (parsed.size() == 2 && parsed[0].size() == 1 && parsed[1].size() == 1) return true;
    return false;
}

pair<Node, Node> NFA::parseAND(const string &s, bool isEndState, int priority) {
    vector<string> parsed = customSplit(s, ' ');
    vector<pair<Node, Node>> ns = *new vector<pair<Node, Node>>;
    for (auto i: parsed) {
        if (isRange(s)) ns.push_back(range(s, 0, false));
        else if (lexicalRules.regularDefinitions.find(i) != lexicalRules.regularDefinitions.end()) {
            if (i.at(i.size() - 1) == '+') {
                ns.push_back(positiveClosure(regularDefinitionsNFA.find(i)->second, false, 0));
            } else if (i.at(i.size() - 1) == '*') {
                ns.push_back(kleeneClosure(regularDefinitionsNFA.find(i)->second, false, 0));
            } else ns.push_back(regularDefinitionsNFA.find(i)->second);
        } else if (checkIfReserved(lexicalRules, i)) {
            ns.push_back(singleNodeNfa(i, false, 0));
        } else if (i.size() > 1 && i.at(i.size() - 1) == '+') {
            ns.push_back(positiveClosure(singleNodeNfa(i.substr(0, i.size() - 1), false, 0), false, 0));
        } else if (i.size() > 1 && i.at(i.size() - 1) == '*') {
            ns.push_back(kleeneClosure(singleNodeNfa(i.substr(0, i.size() - 1), false, 0), false, 0));
        } else if (!i.empty()) {
            ns.push_back(singleNodeNfa(i, false, 0));
        }
    }
    if (ns.size() == 1) return ns[0];
    return concatenation(ns, isEndState, priority);
}

pair<Node, Node> NFA::parseOr(const string &s, bool isEndState, int priority) {
    vector<string> parsed = customSplit(s, '|');
    vector<pair<Node, Node>> nodes;
    for (const auto &i: parsed) {
        nodes.push_back(parseAND(i, false, 0));
    }
    if (nodes.size() == 1) return nodes[0];
    return unionOP(nodes, isEndState, priority);
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
        return unionOP(v, false, 0);
    }
    bool isOr = false;
    if (first[first.size() - 1] == '|') {
        isOr = true;
    }
    pair<Node, Node> firstParsed = parseOr(first, false, 0);
    vector<pair<Node, Node>> v;
    v.push_back(firstParsed);
    v.push_back(last);
    if (v.size() == 1) return v[0];
    if (isOr) {
        return unionOP(v, false, 0);
    } else {
        return concatenation(v, false, 0);
    }
}

pair<Node, Node> NFA::eatForward(const pair<Node, Node> &first, string last) {
    string withoutSpaces = last;
    withoutSpaces.erase(remove_if(withoutSpaces.begin(), withoutSpaces.end(), ::isspace), withoutSpaces.end());
    if (withoutSpaces == "*") {
        return kleeneClosure(first, false, 0);
    } else if (withoutSpaces == "+") {
        return positiveClosure(first, false, 0);

    }
    bool isOr = false;
    if (last[0] == '|') {
        isOr = true;
    }
    pair<Node, Node> lastParsed = parseOr(last, false, 0);
    vector<pair<Node, Node>> v;
    v.push_back(first);
    v.push_back(lastParsed);
    if (v.size() == 1) return v[0];
    if (isOr) {
        return unionOP(v, false, 0);
    } else {
        return concatenation(v, false, 0);
    }
}


pair<Node, Node> NFA::parse(string rule) {
    stack<string> s;
    stack<pair<Node, Node>> nodes;
    bool escapeMood = false;

    for (int i = 0; i < rule.size(); i++) {
        char c = rule[i];
        if (c == '\\' && !escapeMood) {
            escapeMood = true;
            continue;
        }
        int asciiValue = static_cast<int>(c);
        if (asciiValue == 32 && (s.empty() || s.top().empty())) {
            continue;
        }
        if (c == '(' && !escapeMood) {
            if (!s.empty() && s.top() == "") s.pop();
            s.emplace("(");
            s.emplace("");
        } else if (c == ')' && !escapeMood) {
            if (s.top() == "(") s.pop();
            else {
                string partialRule = s.top();
                s.pop();
                s.pop();
                if (i < rule.size() - 1 && rule[i + 1] == '*') {
                    nodes.push(kleeneClosure(parseOr(partialRule, false, 0), false, 0));
                    s.pop();
                } else if (i < rule.size() - 1 && rule[i + 1] == '+') {
                    nodes.push(positiveClosure(parseOr(partialRule, false, 0), false, 0));
                    s.pop();
                } else {
                    nodes.push(parseOr(partialRule, false, 0));
                }
                pair<Node, Node> lastRes = nodes.top();
                while (!s.empty() && s.top() != "(") {
                    nodes.pop();
                    lastRes = eatBackward(s.top(), lastRes, nodes);
                    s.pop();
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
            if (escapeMood) {
                s.top().push_back(' ');
            }
        }
        escapeMood = false;
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
        return parseOr(s.top(), false, 0);
    }
    vector<pair<Node, Node>> v = convertStackToVector(nodes);
    if (v.size() == 1) return v[0];
    return concatenation(v, true, 0);
}

vector<pair<Node, Node>> NFA::parseREs(vector<pair<Node, Node>> pairs) {
    for (const auto &regularExpression: this->lexicalRules.regularExpressions) {
        pairs.push_back(parse(regularExpression.second));
    }
    return pairs;
}

vector<pair<Node, Node>> NFA::parseRDs(vector<pair<Node, Node>> pairs) {
    for (const auto &regularDefinition: this->lexicalRules.regularDefinitions) {
        pair<Node, Node> p = parse(regularDefinition.second);
        pairs.push_back(p);
        regularDefinitionsNFA.insert({regularDefinition.first, p});
    }
    return pairs;
}