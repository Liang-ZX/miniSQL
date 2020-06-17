/*#include "buffermanager.h"
#include "RecordManager.h"
#include <iostream>
using namespace std;
// test1 write in the appropriate block
BufferManager buffer_manager("2333");
CatalogManager catalog_manager;
int main()
{
	BufferManager bmanager("test_db");
	string w_str= "Hello world!\nThis is a buffer manager test\nZhejiang University\n";
	bmanager.writeFile(w_str, "test", 0, 0);
	bmanager.deleteFile("test", 0);
	system("pause");
	return 0;
}*/
/*
//test2
int main()
{
	BufferManager bmanager("test_db");
	string w_str1 = "Hello world!\nZhejiang University\nThis is a new test.\n";
	string w_str2 = "This is a test!\nWe test the LRU.\n";
	string w_str3 = "LRU is a widly-used memory exchange method\n";
	bmanager.writeFile(w_str1, "test", 1, 0, true);
	bmanager.writeFile(w_str2, "LRUtest", 1, 0);
	bmanager.unsetBlockPin("test", 1, 0);
	bmanager.writeFile(w_str3, "test", 1, 1);
	string w_str4 = "new DB test!";
	string s = bmanager.readFile("test", 1, 0);
	cout << s << endl;
	//bmanager.writeFile(w_str4, "hello", 0, 0);
	system("pause");
	return 0;
}*/

//test 3
/*int main()
{
	BufferManager bmanager("test_db");
	//string ans = bmanager.readFile("test", 1, 0);
	//cout << ans << endl;
	bmanager.clearBlock("LRUtest", 1, 0);
	system("pause");
	return 0;
}*/
