#include "API.h"

//interpreter根据信息创建表并传入
void API::createTable(Table& table)
{
	if (catalog_manager.existTable(table.name) == true) {
		cout << "ERROR: Table '" << table.name << "' already exists!\n";
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

	Table &tb = catalog_manager.getTable(table.name);
	Index_Manager index_manager(tb.name);
	for (int i = 0; i < tb.attriNum; i++)
		if (tb.attributes[i].isPrimaryKey == true) {
			Index index(tb.attributes[i].name);
			index.tableName = tb.name;
			index.column = i;
			index.type = tb.attributes[i].type;

			tb.attributes[i].hasindex = true;
			tb.attributes[i].isUnique = true;

			index_manager.Create_Index(index.indexName, index.column, checkType(index.type), index, index.type);
			catalog_manager.createIndex(index);
		}  
	
	cout << "Table '" << table.name << "' has been created successfully!\n";
	return;
}

//传入删除的表名

void API::dropTable(const string& tableName)
{
	if (catalog_manager.existTable(tableName) == false) {
		cout << "ERROR: Table '" << tableName << "' does not exist!\n";
		return;
	}
	Index_Manager index(tableName);
	index.Drop_All();
	catalog_manager.dropTable(tableName);
	record_manager.DropTableFile(tableName);
	cout << "Table '" << tableName << "' has been dropped successfully!\n";
}

//interpreter根据信息创建tuple并传入

void API::insertRecord(const string& tableName, const Tuple& tuple)
{
	if (catalog_manager.existTable(tableName) == false) {
		cout << "ERROR: Table '" << tableName << "' does not exist!";
		return;
	}

	if (record_manager.InsertRecord(tableName, tuple) == -1) return;
	cout << "Record has been inserted successfully!";
}

void API::selectRecord(const string& tableName)
{
	if (catalog_manager.existTable(tableName) == false) {
		cout << "ERROR: Table '" << tableName << "' does not exist!";
		return;
	}
	string res;
	int num = record_manager.SelectRecord(tableName, res);
	if (num == -1) return;
	else if (num == 0) {
		cout << "Empty set.";
		return;
	}
	show(tableName, res, num);
	if (num == 1) cout << num << " row in set.";
	else cout << num << " rows in set.";
}

//interpreter根据信息建立vector<Condition>并传入

void API::selectRecord(const string& tableName, const vector<Condition>& ConditionList)
{
	if (catalog_manager.existTable(tableName) == false) {
		cout << "ERROR: Table '" << tableName << "' does not exist!";
		return;
	}
	string res;
	int num = record_manager.SelectRecord(tableName, ConditionList, res);
	if (num == -1) {
		return;
	}
	else if (num == 0) {
		cout << "Empty set.";
		return;
	}
	show(tableName, res, num);
	if (num == 1) cout << num << " row in set.";
	else cout << num << " rows in set.";
}

void API::deleteRecord(const string& tableName)
{
	if (catalog_manager.existTable(tableName) == false) {
		cout << "ERROR: Table '" << tableName << "' does not exist!\n";
		return;
	}
	int num = record_manager.DeleteRecord(tableName);
	if (num == 1) cout << num << " row has been deleted successfully!\n";
	else cout << num << " rows have been deleted successfully!\n";
}

//interpreter根据信息建立vector<Condition>并传入

void API::deleteRecord(const string& tableName, const vector<Condition>& ConditionList)
{
	if (catalog_manager.existTable(tableName) == false)
	{
		cout << "ERROR: Table '" << tableName << "' does not exist!\n";
		return;
	}
	int num = record_manager.DeleteRecord(tableName, ConditionList);
	if (num == -1) return;
	else if (num == 1) cout << num << " row has been deleted successfully!\n";
	cout << num << " rows have been deleted successfully!\n";
}

//interpreter根据信息创建索引并传入

void API::createIndex(Index& index)
{
	if (catalog_manager.existIndex(index.indexName) == true) {
		cout << "ERROR: Index '" << index.indexName << "' already exists!\n";
		return;
	}
	Table &table = catalog_manager.getTable(index.tableName);
	if (table.attributes[index.column].isUnique == false && table.attributes[index.column].isPrimaryKey == false) {
		cout << "ERROR: " << table.name << '(' << table.attributes[index.column].name << ") is not Unique!\n";
		return;
	}
	if (table.attributes[index.column].hasindex == true) {
		cout << "ERROR: Duplicate indexes on " << table.name << '(' << table.attributes[index.column].name << ")!\n";
		return;
	}
	else  table.attributes[index.column].hasindex = true;
	
	Index_Manager index_manager(index.tableName);
	index_manager.Create_Index(index.indexName, index.column, checkType(index.type), index, index.type);
	catalog_manager.createIndex(index);
	cout << "Index '" << index.indexName << "' has been created successfully!\n";
}

void API::dropIndex(const string& indexName)
{
	if (catalog_manager.existIndex(indexName) == false) {
		cout << "ERROR: Index '" << indexName << "' does not exist!\n";
		return;
	}
	Index index = catalog_manager.getIndex(indexName);
	Table &table = catalog_manager.getTable(index.tableName);

	Index_Manager index_manager(table.name);
	index_manager.Drop_Index(indexName, checkType(index.type));

	table.attributes[index.column].hasindex = false;
	catalog_manager.dropIndex(index);
	cout << "Index '" << indexName << "' has been dropped successfully!\n";
}

Type API::checkType(int type)
{
	if (type == 0) return INT;
	else if (type > 0) return STRING;
	else return FLOAT;
}

void API::show(string tableName, string& res, int num) {
	Table table = catalog_manager.getTable(tableName);
	int* p = new int[table.attriNum];
	int temp;
	for (int i = 0; i < table.attriNum; i++) {		
		p[i] = table.attributes[i].name.length();
	}
		
	int data_length = res.length();
	int pos = 1, h = 1, spa = 0;
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < table.attriNum; j++) {
			while (pos < data_length && res[pos] != ITEM_SEPARATOR && res[pos] != RECORD_SEPARATOR) {
				pos++;
			}
			if (pos - h > p[j]) p[j] = pos - h;
			pos++; h = pos;
		}
		pos += 2; h = pos;
	}
	showl(table.attriNum, p);
	for (int i = 0; i < table.attriNum; i++) {
		cout << "|"; 
		shows(1);
		cout << table.attributes[i].name;
		shows(p[i] - table.attributes[i].name.length() + 1);
	}
	cout << "|\n";
	showl(table.attriNum, p);
	h = pos = 1;
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < table.attriNum; j++) {
			while (pos < data_length && res[pos] != ITEM_SEPARATOR && res[pos] != RECORD_SEPARATOR) {
				pos++;
			}
			cout << "|";
			shows(1);
			cout << res.substr(h, pos - h);
			shows(p[j] - (pos - h) + 1);
			pos++; h = pos;
		}
		pos += 2; h = pos;
		cout << "|\n";
	}
	showl(table.attriNum, p);
}

void API::showm(int num) {
	for (int i = 0; i < num; i++) cout << "-";
}

void API::shows(int num) {
	for (int i = 0; i < num; i++) cout << " ";
}

void API::showl(int num, int* p) {
	for (int i = 0; i < num; i++) {
		cout << "+";
		showm(p[i] + 2);
	}
	cout << "+\n";
}
