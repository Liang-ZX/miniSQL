#include "interpreter.h"
#include "buffermanager.h"
#include "CatalogManager.h"

BufferManager buffer_manager("testrecord");
CatalogManager catalog_manager;
int main()
{
	Interpreter in(nullptr, &catalog_manager);
	string query;
	getline(cin, query, ';');
	in.interprete(query);
	return 0;
}