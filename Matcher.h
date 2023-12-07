#ifndef MATCHER_H
#define MATCHER_H

#include <iostream>
#include <fstream>
#include"State.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <string>
#include "Node.h"
#include "NFATODFA.h"



using namespace std;

class Matcher
{
public:
    Matcher();
    virtual ~Matcher();

    set<string> get_sym_table();

    void match(vector<string> token,DfaResult DFA);

    void set_output_file_name(string output_file_name);

protected:

private:

    set<string> symbol_table;
    string output_file_name;
    string out;

    void write_output_file(string name);

    bool panic_mode_recovery(string str, DfaResult DFA);

    bool matchToken(string str, DfaResult DFA);

};

#endif // MATCHER_H