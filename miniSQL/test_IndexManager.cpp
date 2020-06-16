// Index_Manager.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
/*

#include "BPT.h"
#include "indexmanager.h"
#include <iostream>

int main()
{
	Index_Manager IM;
	IM.Create_Index("index1", INT);
	cout << "------index manager test-------" << endl;
	for (int i = 0; i < 20; i++) {
		IM.Insert("index1", i, i % 40);
	}
	IM.Delete("index1", 0);
	IM.Delete("index1", 1);
	IM.Delete("index1", 8);
	IM.Delete("index1", 7);
	IM.Delete("index1", 19);
	IM.Delete("index1", 18);
	IM.Insert("index1", 8,80);
	IM.Debug_Print("index1", INT);
	IM.Drop_Index("index1", INT);
}

*/

