#include "Matcher.h"
#include <iostream>
#include <fstream>
#include"State.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include "State.h"
#include "NFATODFA.h"


using namespace std;

Matcher::Matcher()
{
    //ctor
}

Matcher::~Matcher()
{
    //dtor
}

void Matcher::match(vector<string> token,pair<TRANSITION_TABLE, Node > DFA)
{
    /*for all tokens try to match them*/
    for(int i = 0 ; i < token.size() ; i++)
    {
        this->fun(token[i], DFA);
    }
    /*write in the output file*/
    write_output_file(this->output_file_name);
}

vector<string> Matcher::get_sym_table()
{
    return this->symbol_table;
}

void Matcher::set_output_file_name(string output_file_name)
{
    this->output_file_name = output_file_name;
}

void Matcher::write_output_file(string name)
{
    /*open file*/
    ofstream myfile (name.c_str());
    if (myfile.is_open())
    {
        /*write in file*/
        myfile << this->out;
        /*close file*/
        myfile.close();
    }
}

bool Matcher::error_recovery(string str, pair<TRANSITION_TABLE, Node > DFA)
{
    for(int i = 1 ; i < str.size() ; i++)
    {
        string str1 = str.substr (i);
        if(this->fun(str1,DFA))
        {
            return true;
        }
    }
    return false;
}


bool Matcher::fun(string str, pair<TRANSITION_TABLE, Node > DFA)
{
    /**try to match string str*/
    vector< set<State>> path;
    /*get the start state*/
    Node startNode=DFA.second;
    set<State> startState = startNode.states;
    TRANSITION_TABLE  transitionTable=DFA.first;
    set<State>  temp = startState;
    set<State> s = startState;
    path.push_back(temp);
    for(std::string::size_type i = 0; i < str.size(); ++i)
    {
        string x="";
        x=str[i];
        /*follow the sequance of str chars*/
        temp=transitionTable[startNode][x].states;
        /*add to path to save steps*/
        path.push_back(temp);
    }
    /*once exit str for loop = check if temp is acceptance state */
    if(temp.begin()->isEndState)
    {
        /*get the type matched from temp name*/
        string type = temp.begin()->name;
        /*write pattern in the output file*/
        this->out = this->out + type + '\n';
        if(type == "id")
        {
            /*add to sym table*/
            this->symbol_table.push_back(str);
        }
        return true;
    }
    else                /*not matched*/
    {
        /*try to go back until reach an acceptance state*/
        int iter = (int)path.size();
        while(!temp.begin()->isEndState && iter > 0)
        {
            temp = path[iter-1];
            iter--;
        }
        /*once i get there -> temp is accepted or iter = 0*/
        if(temp.begin()->isEndState)
        {
            /*i reach to an acceptance state*/
            /*get the type matched from temp name*/
            string type = temp.begin()->name;
            /*write pattern in the output file*/
            this->out = this->out + type + '\n';
            if(type == "id")
            {
                /*add to sym table*/
                this->symbol_table.push_back(str);
            }
            /*seperate str and call back fun again with the reminder*/
            string reminder = str.substr (iter+1);
            return this->fun(reminder, DFA);
        }
        else if(iter <= 0 && !temp.begin()->isEndState )           /* not matched or try delete from start */
        {

            if(this->error_recovery(str,DFA))
            {
                /*deleted from start done :D and write good in file*/
            }
            else
            {
                /*generate error to output file*/
                this->out = this->out + "ERROR in symbol  " + str + '\n';
                return false;
            }
        }
        else
        {
            /*generate error to output file*/
            this->out = this->out + "ERROR in symbol  " + str + '\n';
            return false;
        }
    }
    path.clear();
}