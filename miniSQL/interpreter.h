#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "CatalogManager.h"
#include "API.h"
using namespace std;

class Interpreter
{
private:
	API * api=nullptr; //∂‡Ã¨÷∏’Î
	CatalogManager * catalog_manager =nullptr;
	int readinCondition(vector<Condition> &ConditionList, Table & table, string &s, int &pos);
public:
	Interpreter(){}
	Interpreter(API* in_api, CatalogManager* catalog):api(in_api), catalog_manager(catalog){}
	~Interpreter(){}
	int interprete (string &s, bool exec_file=false);
	string getWord(string&s, int &pos);
	string execFile = "";
};

#endif
