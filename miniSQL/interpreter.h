#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

class Interpreter
{
public:
	Interpreter(){}
	~Interpreter(){}
	int interprete(string &s);
	string getWord(string&s, int &pos);
};




#endif
