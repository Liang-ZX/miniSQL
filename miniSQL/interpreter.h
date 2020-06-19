#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "CatalogManager.h"
#include "API.h"
using namespace std;

class Interpreter
{
private:
	API * api=NULL; //∂‡Ã¨÷∏’Î
	CatalogManager * catalog_manager =NULL;
public:
	Interpreter(){}
	Interpreter(API* in_api, CatalogManager* catalog):api(in_api), catalog_manager(catalog){}
	~Interpreter(){}
	int interprete (string &s);
	string getWord(string&s, int &pos);
};




#endif
