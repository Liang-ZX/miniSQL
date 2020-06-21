#ifndef __API_H__
#define __API_H__
#include <iostream>
#include "CatalogManager.h"
#include "RecordManager.h"
#include "indexmanager.h"
#include "Tuple.h"
using namespace std;

extern RecordManager record_manager;
extern CatalogManager catlog_manager;

class API {
public:
	API() {}
	~API() {}

	//interpreter根据信息创建表并传入
	void createTable(Table& table);

	//传入删除的表名
	void dropTable(const string& tableName);

	//interpreter根据信息创建tuple并传入
	void insertRecord(const string& tableName, const Tuple& tuple);

	void selectRecord(const string& tableName);

	//interpreter根据信息建立vector<Condition>并传入
	void selectRecord(const string& tableName, const vector<Condition>& ConditionList);

	void deleteRecord(const string& tableName);

	//interpreter根据信息建立vector<Condition>并传入
	void deleteRecord(const string& tableName, const vector<Condition>& ConditionList);

	//interpreter根据信息创建索引并传入
	void createIndex(Index& index);

	void dropIndex(const string& indexName);

private:
	Type checkType(int type);

	void show(string tableName, string& res, int num);

	void showm(int num);

	void shows(int num);

	void showl(int num, int* p);
};

#endif
