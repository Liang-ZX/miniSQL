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
	void Create_Index(string File, Type type);			 //��������
	void Drop_Index(string File, Type type);			 //ɾ������
	//����key k������block_num
	bool Search(string File, int k, int& block_num);
	bool Search(string File, float k, int& block_num);
	bool Search(string File, string k, int& block_num);
	bool Search(string File, int min, int max, vector<int>& block_num);		//��Χ��ѯ
	bool Search(string File, float min, float max, vector<int>& block_num); //��Χ��ѯ
	//����key k, block_num
	void Insert(string File, int k, int block_num);
	void Insert(string File, float k, int block_num);
	void Insert(string File, string k, int block_num);
	//ɾ��key k
	void Delete(string File, int k);
	void Delete(string File, float k);
	void Delete(string File, string k);

	void Debug_Print(string File, Type type);
private:
	map<string, BPT<int>*> BPT_Int;		//int����B+��
	map<string, BPT<float>*> BPT_Float;	//float����B+��
	map<string, BPT<string>*> BPT_String;	//char����B+��
};

#endif