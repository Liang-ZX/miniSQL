#ifndef __API_H__
#define __API_H__
#include <iostream>
#include "CatalogManager.h"
#include "RecordManager.h"
#include "indexmanager.h"
#include "Tuple.h"
using namespace std;

class API {
public:
	API(){}
	~API(){}

	//interpreter������Ϣ����������
	void createTable(Table& table);

	//����ɾ���ı���
	void dropTable(const string& tableName);

	//interpreter������Ϣ����tuple������
	void insertRecord(const string& tableName, const Tuple& tuple);

	void selectRecord(const string& tableName);

	//interpreter������Ϣ����vector<Condition>������
	void selectRecord(const string& tableName, const vector<Condition>& ConditionList);
	
	void deleteRecord(const string& tableName);
	
	//interpreter������Ϣ����vector<Condition>������
	void deleteRecord(const string& tableName, const vector<Condition>& ConditionList);
	
	//interpreter������Ϣ��������������
	void createIndex(Index& index);
	
	void dropIndex(const string& indexName); 

};

#endif
