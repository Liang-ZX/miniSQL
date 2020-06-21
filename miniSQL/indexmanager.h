#ifndef _INDEXMANAGER_H
#define _INDEXMANAGER_H
#include "BPT.h"
#include "Tuple.h"
#include "buffermanager.h"
#include "CatalogManager.h"
#include <stdlib.h>
#include <map>

typedef TupleDataType Type;
extern BufferManager buffer_manager;
extern CatalogManager catalog_manager;

class Index_Manager {
public:
	Index_Manager(string Table_name);
	~Index_Manager();
	//从硬盘中读取已存在的索引文件
	void Read_Index(string File, Type type, int n = 0);
	//创建索引文件, n为char(n)中的n，column为属性所在的列数(从0计数)
	void Create_Index(string File, int column, Type type, Index &index, int n = 0);
	//删除索引文件
	void Drop_Index(string File, Type type);
	//删除全部索引文件
	void Drop_All();
	//清空全部索引文件
	void Clear_Index();
	//查询操作：五个重载函数，前三个为等值查询，后两个为范围查询
	bool Search(string File, int k, int& block_num);
	bool Search(string File, float k, int& block_num);
	bool Search(string File, string k, int& block_num);
	bool Search(string File, int min, int max, set<int>& block_num);
	bool Search(string File, float min, float max, set<int>& block_num);
	//插入操作：
	void Insert(string File, int k, int block_num);
	void Insert(string File, float k, int block_num);
	void Insert(string File, string k, int block_num);
	//删除操作
	void Delete(string File, int k);
	void Delete(string File, float k);
	void Delete(string File, string k);
	//debug only
	void Debug_Print(string File, Type type);
private:
	string Table_name;
	map<string, BPT<int>*> BPT_Int;			//int类型B+树
	map<string, BPT<float>*> BPT_Float;		//float类型B+树
	map<string, BPT<string>*> BPT_String;	//string类型B+树
};

#endif