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
		if (readFile) {
			file.open(interpreter.execFile);
			if (!file.is_open()) {
				cout << "Fail to open file '" << interpreter.execFile << "'." << endl;
				readFile = false;
				continue;
			}
			while (getline(file, query, ';'))
			{
				int status = interpreter.interprete(query);
				if (status == 200)
					break; //quit
			}
			file.close();
			readFile = 0;
			interpreter.execFile = "";
		}
		else {
			cout << "minisql>>>";
			getline(cin, query, ';');
			int status = interpreter.interprete(query);
			if (status == 200) {
				break;
			}
			else if (status == 2) {
				readFile = true;
			}
			else {
				if( status == 0)
					cout << "query OK" << endl;
			}
		}
	}
	return 0;
}