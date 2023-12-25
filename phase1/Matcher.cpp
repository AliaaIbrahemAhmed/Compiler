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

void Matcher::match(vector<string> token,DfaResult minimized_DFA)
{
    for(int i = 0 ; i < token.size() ; i++)
    {
        this->matchToken(token[i], minimized_DFA);
    }
    write_to_output_file(this->output_file_name);
}

set<string> Matcher::get_symbol_table()
{
    return this->symbol_table;
}

void Matcher::set_output_file_name(string output_file_name)
{
    this->output_file_name = output_file_name;
}

void Matcher::write_to_output_file(string name)
{
    ofstream outputFile (name.c_str());
    if (outputFile.is_open())
    {
        outputFile << this->output;
        outputFile.close();
    }
}

bool Matcher::panic_mode_recovery(string str, DfaResult minimized_DFA)
{
    this->output = this->output + "invalid symbol  " + str[0] + '\n';
    for(int i = 1 ; i < str.size() ; i++)
    {
        string str1 = str.substr (i);
        if(this->matchToken(str1, minimized_DFA))
        {
            return true;
        }
    }
    return false;
}


bool Matcher::matchToken(string str, DfaResult minimized_DFA)
{
    vector< Node> path;
    Node startNode=minimized_DFA.startNode;
    set<State> startState = startNode.states;
    DFA_TRANSITION_TABLE   transitionTable=minimized_DFA.DFA;
    unordered_map<Node, string> endMap=minimized_DFA.endMap;
    Node tempNode=startNode;
    path.push_back(tempNode);
    for (char ch : str) {
        string x="";
        x=ch;
        tempNode=transitionTable[tempNode][x];
        path.push_back(tempNode);
    }
    if(tempNode.states.begin()->isEndState)
    {
        string type = endMap[tempNode];
        this->output = this->output + type + '\n';
        this->tokensName.push_back(type);
        //cout<< "tokensName -->"<<type<<endl;
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
            this->output = this->output + type + '\n';

            this->tokensName.push_back(type);
            //cout<< "tokens2 -->"<<type<<endl;

            if(type == "id")
            {
                this->symbol_table.insert(str.substr(0,iter));
            }
            string reminder = str.substr (iter);
            return this->matchToken(reminder, minimized_DFA);
        }

        else if(iter <= 0 && !tempNode.states.begin()->isEndState )
        {
            this->panic_mode_recovery(str, minimized_DFA);
        }
        else
        {
            this->output = this->output + "invalid symbol  " + str + '\n';
            return false;
        }
    }
    path.clear();
}