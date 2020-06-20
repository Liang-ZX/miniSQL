#include "API.h"

//interpreter根据信息创建表并传入
void API::createTable(Table& table) {
	if (catalog_manager.existTable(table.name) == true) {
		cout << "ERROR: Table " << table.name << " already exists!\n";
		return;
	}
	for (int i = 0; i < table.attriNum; i++)
		for (int j = 0; j < table.attriNum; j++)
			if (i != j && table.attributes[i].name == table.attributes[j].name) {
				cout << "ERROR: Duplicate column name " << table.attributes[i].name << " !\n";
				return;
			}
	catalog_manager.createTable(table);
	record_manager.CreateTableFile(table.name);
	cout << "Table " << table.name << " has been created successfully!\n";
}

//传入删除的表名
void API::dropTable(const string& tableName) {
	if (catalog_manager.existTable(tableName) == false) {
		cout << "ERROR: Table " << tableName << " does not exist!\n";
		return;
	}
	Index_Manager index(tableName);
	index.Clear_Index();
	catalog_manager.dropTable(tableName);
	record_manager.DropTableFile(tableName);
	cout << "Table " << tableName << " has been dropped successfully!\n";
}

//interpreter根据信息创建tuple并传入
void API::insertRecord(const string& tableName, const Tuple& tuple) {
	if (catalog_manager.existTable(tableName) == false) {
		cout << "ERROR: Table " << tableName << " does not exist!\n";
		return;
	}
	if (record_manager.InsertRecord(tableName, tuple) == -1) return;
	cout << "Record has been inserted successfully!\n";
}

void API::selectRecord(const string& tableName) {
	if (catalog_manager.existTable(tableName) == false) {
		cout << "ERROR: Table " << tableName << " does not exist!\n";
		return;
	}
	string res;
	record_manager.SelectRecord(tableName, res);
	show(res);
}

//interpreter根据信息建立vector<Condition>并传入
void API::selectRecord(const string& tableName, const vector<Condition>& ConditionList) {
	if (catalog_manager.existTable(tableName) == false) {
		cout << "ERROR: Table " << tableName << " does not exist!\n";
		return;
	}
	string res;
	if (record_manager.SelectRecord(tableName, ConditionList, res) == -1) return;
	show(res);
}

void API::deleteRecord(const string& tableName) {
	if (catalog_manager.existTable(tableName) == false) {
		cout << "ERROR: Table " << tableName << " does not exist!\n";
		return;
	}
	int num = record_manager.DeleteRecord(tableName);
	if (num == 1) cout << num << " row has been deleted successfully!\n";
	else cout << num << " rows have been deleted successfully!\n";
}

//interpreter根据信息建立vector<Condition>并传入
void API::deleteRecord(const string& tableName, const vector<Condition>& ConditionList) {
	if (catalog_manager.existTable(tableName) == false)
	{
		cout << "ERROR: Table " << tableName << " does not exist!\n";
		return;
	}
	int num = record_manager.DeleteRecord(tableName, ConditionList);
	if (num == -1) return;
	else if (num == 1) cout << num << " row has been deleted successfully!\n";
	cout << num << " rows have been deleted successfully!\n";
}

//interpreter根据信息创建索引并传入
void API::createIndex(Index& index) {
	if (catalog_manager.existIndex(index.indexName) == true) {
		cout << "ERROR: Index " << index.indexName << " already exists!\n";
		return;
	}
	Table table = catalog_manager.getTable(index.tableName);
	if (table.attributes[index.column].isUnique == false && table.attributes[index.column].isPrimaryKey == false) {
		cout << "ERROR: " << table.name << '(' << table.attributes[index.column].name << ") is not Unique!\n";
		return;
	}
	if (table.attributes[index.column].hasindex == true) {
		cout << "ERROR: Duplicate indexes on " << table.name << '(' << table.attributes[index.column].name << ")!\n";
		return;
	}
	else  table.attributes[index.column].hasindex = true;
	catalog_manager.createIndex(index);
	Index_Manager index_manager(index.tableName);
	index_manager.Create_Index(index.indexName, index.column, checkType(index.type), index.type);
	cout << "Index " << index.indexName << " has been created successfully!\n";
}

void API::dropIndex(const string& indexName) {
	if (catalog_manager.existIndex(indexName) == false) {
		cout << "ERROR: Index " << indexName << " does not exist!\n";
		return;
	}
	Index index = catalog_manager.getIndex(indexName);
	Table table = catalog_manager.getTable(index.tableName);
	table.attributes[index.column].hasindex = false;
	catalog_manager.dropIndex(index);
	Index_Manager index_manager(table.name);
	index_manager.Drop_Index(indexName, checkType(index.type));
	cout << "Index " << indexName << " has been dropped successfully!\n";
}

Type API::checkType(int type) {
	if (type == 0) return INT;
	else if (type > 0) return STRING;
	else return FLOAT;
}

void API::show(string &res) {
	cout << res << endl;
}

