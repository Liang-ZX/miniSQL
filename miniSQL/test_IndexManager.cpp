//#include "BPT.h"
//#include "indexmanager.h"
//#include "buffermanager.h"
//#include <iostream>
//#include <fstream>
//
//BufferManager buffer_manager("test");
//CatalogManager catalog_manager;
//
//int main() {
//	int temp;
//	BPT<int> bpt1(6);
//	ifstream data("test_BPT_int_100.txt");
//	//数据的插入
//	cout << "--------insert----------" << endl;
//	while (data >> temp) {
//		bpt1.Insert_Key(temp, temp % 40);
//	}
//	bpt1.Print();
//	//数据的搜索
//	cout << "--------search----------" << endl;
//	int block_num;
//	set<int> block_num1;
//	bpt1.Search_Key(53, block_num);
//	cout << "block_num = " << block_num << endl;
//	bpt1.Search_Key(20, 30, block_num1);
//	cout << "block_num1: ";
//	for (set<int>::iterator it = block_num1.begin(); it != block_num1.end(); it++) {
//		cout << *it << " ";
//	}
//	cout << endl;
//	//数据的删除
//	cout << "--------delete----------" << endl;
//	for (int i = 1; i <= 50; i++) {
//		bpt1.Delete_Key(i);
//	}
//	bpt1.Print();
//	bpt1.Delete_All();
//	bpt1.Print();
//	//企图删除不存在的key
//	cout << "--------delete fail----------" << endl;
//	bpt1.Delete_Key(20);
//	data.close();
//	system("pause");
//}