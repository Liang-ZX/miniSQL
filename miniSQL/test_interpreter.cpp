#include "interpreter.h"
#include "buffermanager.h"
#include "CatalogManager.h"

BufferManager buffer_manager("helloworld");
CatalogManager catalog_manager;
int main()
{
	Interpreter in;
	string query;
	getline(cin, query, ';');
	in.interprete(query);
	return 0;
}