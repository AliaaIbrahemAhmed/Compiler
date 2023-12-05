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

Node NFA::getRoot(const vector<pair<Node, Node>> &nodes) {
    Node root = *new Node({*new State(false, 0, "0")});
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
    TRANSITIONS sTransitions, eTransitions;
    sTransitions.insert({EPSILON, se.first});
    sTransitions.insert({EPSILON, se.second});
    nfa[se.second].insert({EPSILON, se.first});
    nfa.insert({s, sTransitions});
    nfa.insert({se.second, eTransitions});
    return {s, se.second};
}

pair<Node, Node> NFA::positiveClosure(const pair<Node, Node> &se, bool isEndState, int priority) {
    Node s = *new Node({*new State(false, 0, generateNewStateName())});
    TRANSITIONS sTransitions, eTransitions;
    sTransitions.insert({EPSILON, se.first});
    nfa[se.second].insert({EPSILON, se.first});
    nfa.insert({s, sTransitions});
    return {s, se.second};
}


pair<Node, Node> NFA::concatenation(vector<pair<Node, Node>> nodes, bool isEndState, int priority,string patternName) {
    Node s = *new Node({*new State(false, 0, generateNewStateName())});
    Node e = *new Node({*new State(isEndState, priority, patternName)});
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

pair<Node, Node> NFA::unionOP(const vector<pair<Node, Node>> &nodes, bool isEndState, int priority,string patternName) {
    Node s = *new Node({*new State(false, 0, generateNewStateName())});
    Node e = *new Node({*new State(isEndState, priority, patternName)});
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
        TRANSITIONS sTransitions, eTransitions, bTransitions;
        Node b = *new Node({*new State(false, 0, generateNewStateName())});
        sTransitions.insert({to_string(string1[0]), b});
        nfa.insert({b, bTransitions});
        Node prevNode = b;
        for (int i = 1; i < string1.size() - 1; i++) {
            Node newNode = *new Node({*new State(false, 0, generateNewStateName())});
            TRANSITIONS nTransitions;
            nfa.insert({newNode, nTransitions});
            nfa[prevNode].insert({to_string(string1[i]), newNode});
            prevNode = newNode;
        }
        if (string1.size() > 1) {
            Node newNode = *new Node({*new State(isEndState, priority, string1)});
            nfa[prevNode].insert({to_string(string1[string1.size() - 1]), newNode});
            TRANSITIONS nTransitions;
            nfa.insert({newNode, nTransitions});
            prevNode = newNode;
        }
        nfa.insert({s, sTransitions});
        return {s, prevNode};
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
    return unionOP(nodes, isEndState, priority,generateNewStateName());

}

bool NFA::isRange(const string &expression) {
    vector<string> parsed = customSplit(expression, '-');
    if (parsed.size() == 2 && parsed[0].size() == 1 && parsed[1].size() == 1) return true;
    return false;
}

pair<Node, Node> NFA::parseAND(const string &s, bool isEndState, int priority,string patternName) {
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

    return concatenation(ns, isEndState, priority,patternName);
}

pair<Node, Node> NFA::parseOr(const string &s, bool isEndState, int priority,string patternName) {
    vector<string> parsed = customSplit(s, '|');
    vector<pair<Node, Node>> nodes;
    for (const auto &i: parsed) {
        nodes.push_back(parseAND(i, false, 0,generateNewStateName()));
    }

    return unionOP(nodes, isEndState, priority,patternName);
}

pair<Node, Node>
NFA::eatBackward(string first, const pair<Node, Node> &last, stack<pair<Node, Node>> lastParsedAtStack, bool isEndState,
                 int priority) {
    string withoutSpaces = first;
    withoutSpaces.erase(remove_if(withoutSpaces.begin(), withoutSpaces.end(), ::isspace), withoutSpaces.end());
    if (withoutSpaces == "|") {
        vector<pair<Node, Node>> v;
        v.push_back(lastParsedAtStack.top());
        v.push_back(last);
        if (v.size() == 1) return v[0];
        return unionOP(v, false, 0,generateNewStateName());
    }
    bool isOr = false;
    if (first[first.size() - 1] == '|') {
        isOr = true;
    }
    pair<Node, Node> firstParsed = parseOr(first, false, 0,generateNewStateName());
    vector<pair<Node, Node>> v;
    v.push_back(firstParsed);
    v.push_back(last);
    if (v.size() == 1) return v[0];
    if (isOr) {
        return unionOP(v, isEndState, priority,generateNewStateName());
    } else {
        return concatenation(v, isEndState, priority,generateNewStateName());
    }
}

pair<Node, Node> NFA::eatForward(const pair<Node, Node> &first, string last, bool isEndState, int priority) {
    string withoutSpaces = last;
    withoutSpaces.erase(remove_if(withoutSpaces.begin(), withoutSpaces.end(), ::isspace), withoutSpaces.end());
    if (withoutSpaces == "*") {
        return kleeneClosure(first, isEndState, priority);
    } else if (withoutSpaces == "+") {
        return positiveClosure(first, isEndState, priority);

    }
    bool isOr = false;
    if (last[0] == '|') {
        isOr = true;
    }
    pair<Node, Node> lastParsed = parseOr(last, false, 0,generateNewStateName());
    vector<pair<Node, Node>> v;
    v.push_back(first);
    v.push_back(lastParsed);
    if (v.size() == 1) return v[0];
    if (isOr) {
        return unionOP(v, isEndState, priority,generateNewStateName());
    } else {
        return concatenation(v, isEndState, priority,generateNewStateName());
    }
}


pair<Node, Node> NFA::parse(string rule,string patternName) {
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
                    nodes.push(kleeneClosure(parseOr(partialRule, false, 0,generateNewStateName()), false, 0));
                    i++;
                } else if (i < rule.size() - 1 && rule[i + 1] == '+') {
                    nodes.push(positiveClosure(parseOr(partialRule, false, 0,generateNewStateName()), false, 0));
                    i++;
                } else {
                    nodes.push(parseOr(partialRule, false, 0,generateNewStateName()));
                }
                pair<Node, Node> lastRes = nodes.top();
                while (!s.empty() && s.top() != "(") {
                    nodes.pop();
                    string topExp = s.top();
                    s.pop();
                    if (s.empty() && nodes.empty()) lastRes = eatBackward(s.top(), lastRes, nodes, false ,0);
                    else lastRes = eatBackward(s.top(), lastRes, nodes, false, 0);
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
        i++;
    }

    if (!nodes.empty()) {
        pair<Node, Node> lastRes = nodes.top();
        while (!s.empty()) {
            lastRes = eatForward(lastRes, s.top(), false, 0);
            s.pop();
            nodes.pop();
            nodes.push(lastRes);
        }
    } else {
        return parseOr(s.top(), true, 0, patternName);
    }
    vector<pair<Node, Node>> v = convertStackToVector(nodes);
    return concatenation(v, true, 0, patternName);
}

vector<pair<Node, Node>> NFA::parseREs() {
    vector<pair<Node, Node>> pairs;
    for (const auto &regularExpression: this->lexicalRules.regularExpressions) {
        pair<Node, Node> p = parse(regularExpression.second,regularExpression.first);
        pairs.push_back(p);
        REsNFA.insert({regularExpression.first, p});
    }
    return pairs;
}

vector<pair<Node, Node>> NFA::parseRDs() {
    vector<pair<Node, Node>> pairs;
    for (const auto &regularDefinition: this->lexicalRules.regularDefinitions) {
        pair<Node, Node> p = parse(regularDefinition.second,regularDefinition.first);
        pairs.push_back(p);
        regularDefinitionsNFA.insert({regularDefinition.first, p});
    }
    return pairs;
}
