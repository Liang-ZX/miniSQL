#include "interpreter.h"
#include "buffermanager.h"
#include "CatalogManager.h"

BufferManager buffer_manager("testrecord");
CatalogManager catalog_manager;
RecordManager record_manager;
int main()
{
	API api;
	Interpreter interpreter(&api, &catalog_manager);
	cout << "***********************MiniSQL***************************" << endl;
	bool readFile = false;
	ifstream file;
	string query;
	while (true)
	{
		int status = 0, insert_cnt=0;
		if (readFile) {
			insert_cnt = 0;
			file.open(interpreter.execFile);
			if (!file.is_open()) {
				cout << "Fail to open file '" << interpreter.execFile << "'." << endl;
				readFile = false;
				continue;
			}
			while (getline(file, query, ';'))
			{
				status = interpreter.interprete(query, true);
				if (query.find("insert") != query.npos)
					cout << "(" << insert_cnt++ << ")\n";
				if (status == 200)
					break; //quit
			}
			file.close();
			cout << endl;
			readFile = 0;
			interpreter.execFile = "";
			if (status == 200)
				break;
		}
		else {
			cout << "minisql>>>";
			getline(cin, query, ';');
			status = interpreter.interprete(query);
			if (status == 200) {
				break;
			}
			else if (status == 2) {
				readFile = true;
			}
			else {
				if (status == 0)
					cout << "Query OK." << endl << endl;
			}
		}
	}
	return 0;
}