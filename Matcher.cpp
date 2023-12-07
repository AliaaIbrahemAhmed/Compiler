#include "Matcher.h"
#include <iostream>
#include <fstream>
#include"State.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include "State.h"
#include "Node.h"


using namespace std;

Matcher::Matcher()
{
}

Matcher::~Matcher()
{
}

void Matcher::match(vector<string> token,DfaResult DFA)
{
    for(int i = 0 ; i < token.size() ; i++)
    {
        cout << "Current Token:" << token[i] << endl;
        this->matchToken(token[i], DFA);
    }
    write_output_file(this->output_file_name);
}

set<string> Matcher::get_sym_table()
{
    return this->symbol_table;
}

void Matcher::set_output_file_name(string output_file_name)
{
    this->output_file_name = output_file_name;
}

void Matcher::write_output_file(string name)
{
    ofstream myfile (name.c_str());
    if (myfile.is_open())
    {
        myfile << this->out;
        myfile.close();
    }
}

bool Matcher::panic_mode_recovery(string str, DfaResult DFA)
{
    for(int i = 1 ; i < str.size() ; i++)
    {
        string str1 = str.substr (i);
        if(this->matchToken(str1,DFA))
        {
            return true;
        }
    }
    return false;
}


bool Matcher::matchToken(string str, DfaResult DFA)
{
    vector< Node> path;
    Node startNode=DFA.startNode;
    set<State> startState = startNode.states;
    DFA_TRANSITION_TABLE   transitionTable=DFA.DFA;
    unordered_map<Node, string> endMap=DFA.endMap;
    Node tempNode=startNode;
    cout << "states: " << tempNode.states.size() << endl;

    path.push_back(tempNode);
    for (char ch : str) {
        string x="";
        x=ch;
        cout << "Current char:" << x << endl;
        tempNode=transitionTable[tempNode][x];
        cout << "Current Node states num: " << tempNode.states.size() << endl;

        path.push_back(tempNode);
    }
    if(tempNode.states.begin()->isEndState)
    {
        string type = endMap[tempNode];
        this->out = this->out + type + '\n';
        if(type == "id")
        {
            this->symbol_table.insert(str);
        }
        return true;
    }
    else
    {
        int iter = (int)path.size();
        while(!tempNode.states.begin()->isEndState && iter > 0)
        {
            tempNode = path[iter-1];
            iter--;
        }
        if(tempNode.states.begin()->isEndState)
        {
            string type = endMap[tempNode];
            this->out = this->out + type + '\n';
            if(type == "id")
            {
                this->symbol_table.insert(str);
            }
            string reminder = str.substr (iter);
            return this->matchToken(reminder, DFA);
        }

        else if(iter <= 0 && !tempNode.states.begin()->isEndState )
        {

            if(this->panic_mode_recovery(str,DFA))
            {}
            else
            {
                this->out = this->out + "invalid symbol  " + str + '\n';
                return false;
            }
        }
        else
        {
            this->out = this->out + "invalid symbol  " + str + '\n';
            return false;
        }
    }
    path.clear();
}