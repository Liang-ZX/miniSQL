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
	//��Ӳ���ж�ȡ�Ѵ��ڵ������ļ�
	void Read_Index(string File, Type type, int n = 0);
	//���������ļ�, nΪchar(n)�е�n��columnΪ�������ڵ�����(��0����)
	void Create_Index(string File, int column, Type type, Index &index, int n = 0);
	//ɾ�������ļ�
	void Drop_Index(string File, Type type);
	//ɾ��ȫ�������ļ�
	void Drop_All();
	//���ȫ�������ļ�
	void Clear_Index();
	//��ѯ������������غ�����ǰ����Ϊ��ֵ��ѯ��������Ϊ��Χ��ѯ
	bool Search(string File, int k, int& block_num);
	bool Search(string File, float k, int& block_num);
	bool Search(string File, string k, int& block_num);
	bool Search(string File, int min, int max, set<int>& block_num);
	bool Search(string File, float min, float max, set<int>& block_num);
	//���������
	void Insert(string File, int k, int block_num);
	void Insert(string File, float k, int block_num);
	void Insert(string File, string k, int block_num);
	//ɾ������
	void Delete(string File, int k);
	void Delete(string File, float k);
	void Delete(string File, string k);
	//debug only
	void Debug_Print(string File, Type type);
private:
	string Table_name;
	map<string, BPT<int>*> BPT_Int;			//int����B+��
	map<string, BPT<float>*> BPT_Float;		//float����B+��
	map<string, BPT<string>*> BPT_String;	//string����B+��
};

#endif