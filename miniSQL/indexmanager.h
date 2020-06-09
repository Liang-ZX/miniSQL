#ifndef _INDEXMANAGER_H
#define _INDEXMANAGER_H
#include "BPT.h"
#include "Tuple.h"
#include <map>
typedef TupleDataType Type;

class Index_Manager {
public:
	Index_Manager();
	~Index_Manager();
	void Create_Index(string File, Type type);			 //创建索引
	void Drop_Index(string File, Type type);			 //删除索引
	//检索key k，返回block_num
	bool Search(string File, int k, int& block_num);
	bool Search(string File, float k, int& block_num);
	bool Search(string File, string k, int& block_num);
	bool Search(string File, int min, int max, vector<int>& block_num);		//范围查询
	bool Search(string File, float min, float max, vector<int>& block_num); //范围查询
	//插入key k, block_num
	void Insert(string File, int k, int block_num);
	void Insert(string File, float k, int block_num);
	void Insert(string File, string k, int block_num);
	//删除key k
	void Delete(string File, int k);
	void Delete(string File, float k);
	void Delete(string File, string k);

	void Debug_Print(string File, Type type);
private:
	map<string, BPT<int>*> BPT_Int;		//int类型B+树
	map<string, BPT<float>*> BPT_Float;	//float类型B+树
	map<string, BPT<string>*> BPT_String;	//char类型B+树
};

#endif