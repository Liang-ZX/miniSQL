#include "CatalogManager.h"

void CatalogManager::InitialTable() {	//read table information into memory
	const string filename = "table.catalog";
	string str = buffer_manager.readFile(filename, 0, 0, false, true);
	if (!str[0]) return;
	istringstream strcin(str);
	string s;
	strcin >> s; tableNum = stoi(s);
	for (int i = 0; i < tableNum; i++) {	//get information for each table
		Table tempT;
		strcin >> tempT.name;
		strcin >> s; tempT.blockNum = stoi(s);
		strcin >> s; tempT.attriNum = stoi(s);
		for (int j = 0; j < tempT.attriNum; j++) {	//get attributes of the table
			Attribute tempA;
			strcin >> tempA.name;
			strcin >> s; tempA.type = stoi(s);
			strcin >> s; tempA.length = stoi(s);
			strcin >> s; tempA.isPrimaryKey = stoi(s);
			strcin >> s; tempA.isUnique = stoi(s);
			strcin >> s; tempA.hasindex = stoi(s);
			tempT.attributes.push_back(tempA);
			tempT.totalLength += tempA.length;	//update the total length of one record
		}
		tables.push_back(tempT);
	}
}

void CatalogManager::InitialIndex() {	//read index information into memory
	const string filename = "index.catalog";
	string str = buffer_manager.readFile(filename, 1, 0, false, true);
	if (!str[0]) return;
	istringstream strcin(str);
	string s;
	strcin >> s; indexNum = stoi(s);
	for (int i = 0; i < indexNum; i++) {	//get information for each index
		Index tempI;
		strcin >> tempI.indexName;
		strcin >> tempI.tableName;
		strcin >> s; tempI.column = stoi(s);
		strcin >> s; tempI.blockNum = stoi(s);
		strcin >> s; tempI.type = stoi(s);
		indexes.push_back(tempI);
	}
}

void CatalogManager::StoreTable() {	//store table information into the hard disk
	const string filename = "table.catalog";
	string res = to_string(tableNum) + '\n';
	for (int i = 0; i < tableNum; i++) {	//for each table
		res = res + tables[i].name + ' ';
		res = res + to_string(tables[i].blockNum) + ' ';
		res = res + to_string(tables[i].attriNum) + '\n';
		for (int j = 0; j < tables[i].attriNum; j++) {	//for each attribute
			res = res + tables[i].attributes[j].name + ' ';
			res = res + to_string(tables[i].attributes[j].type) + ' ';
			res = res + to_string(tables[i].attributes[j].length) + ' ';
			res = res + to_string(tables[i].attributes[j].isPrimaryKey) + ' ';
			res = res + to_string(tables[i].attributes[j].isUnique) + ' ';
			res = res + to_string(tables[i].attributes[j].hasindex) + '\n';
		}
	}
	buffer_manager.writeFile(res, filename, 0, 0);
}

void CatalogManager::StoreIndex() {	//store index information into the hard disk
	const string filename = "index.catalog";
	string res = to_string(indexNum) + '\n';
	for (int i = 0; i < indexNum; i++) {	//for each index
		res = res + indexes[i].indexName + ' ';
		res = res + indexes[i].tableName + ' ';
		res = res + to_string(indexes[i].column) + ' ';
		res = res + to_string(indexes[i].blockNum) + ' ';
		res = res + to_string(indexes[i].type) + '\n';
	}
	buffer_manager.writeFile(res, filename, 1, 0);
}

void CatalogManager::createTable(Table& table) {	//save the table in the catalog
	tableNum++;
	table.totalLength = 0;
	for (int i = 0; i < table.attriNum; i++)
		table.totalLength += table.attributes[i].length;
	tables.push_back(table);
}

void CatalogManager::createIndex(Index index) {	//save the index in the catalog
	indexNum++;
	indexes.push_back(index);
}

void CatalogManager::dropTable(Table table) {	//drop the table from the catalog
	dropTable(table.name);
}

void CatalogManager::dropTable(string tableName) {	//drop the table by name	
	for (int i = 0; i < tableNum; i++)
		if (tables[i].name == tableName) {
			tables.erase(tables.begin() + i);
			tableNum--;
		}
	for (int i = 0; i < indexNum; i++)	//drop indexes on the table
		if (indexes[i].tableName == tableName) {
			indexes.erase(indexes.begin() + i);
			indexNum--;
		}
}

void CatalogManager::dropIndex(Index index) {	//drop the index from the catalog
	dropIndex(index.indexName);
}

void CatalogManager::dropIndex(string indexName) {	//drop the index by name
	for (int i = 0; i < indexNum; i++)
		if (indexes[i].indexName == indexName) {
			indexes.erase(indexes.begin() + i);
			indexNum--;
		}
}

bool CatalogManager::existTable(string tableName) {	//whether the table already exists, true for yes
	for (int i = 0; i < tableNum; i++)
		if (tables[i].name == tableName)
			return true;
	return false;
}

bool CatalogManager::existIndex(string indexName) {	//whether the index name already exists, true for yes
	for (int i = 0; i < indexNum; i++)
		if (indexes[i].indexName == indexName)
			return true;
	return false;
}

bool CatalogManager::existIndex(string tableName, int column) {	//whether an index already exists for this location, true for yes

	for (int i = 0; i < indexes.size(); i++)
		if (indexes[i].tableName == tableName && indexes[i].column == column)
			return true;
	return false;
}

int CatalogManager::getColumn(const Table& table, string attriName) {	//find out on which column the index is created
	for (int i = 0; i < table.attriNum; i++)
		if (table.attributes[i].name == attriName)
			return i;
	return -1;
}

Table& CatalogManager::getTable(string tableName) {	//get the table by name
	Table temp;
	for (int i = 0; i < tableNum; i++)
		if (tables[i].name == tableName)
			return tables[i];
	return temp;
}

Index& CatalogManager::getIndex(string tableName, int column) {	//get the index by location
	Index temp;
	for (int i = 0; i < indexNum; i++)
		if (indexes[i].tableName == tableName && indexes[i].column == column)
			return indexes[i];
	return temp;
}

Index& CatalogManager::getIndex(string indexName) {	//get the index by name
	Index temp;
	for (int i = 0; i < indexNum; i++)
		if (indexes[i].indexName == indexName)
			return indexes[i];
	return temp;
}

void CatalogManager::getIndex(string tableName, vector<Index>& answer) {	//get indexes by tablename
	for (int i = 0; i < indexNum; i++) {
		if (indexes[i].tableName == tableName) {
			answer.push_back(indexes[i]);
		}
	}
}

void CatalogManager::ShowTableCatalog() {	//for debug only
	cout << "## Number of tables:" << tableNum << endl;
	for (int i = 0; i < tableNum; i++) {
		cout << "TABLE " << i << endl;
		cout << "Table Name: " << tables[i].name << endl;
		cout << "Number of attributes: " << tables[i].attriNum << endl;
		cout << "Number of blocks occupied in disk: " << tables[i].blockNum << endl;
		cout << "Total length of table: " << tables[i].totalLength << endl;
		for (int j = 0; j < tables[i].attriNum; j++) {
			cout << "name:" << tables[i].attributes[j].name << "\t";
			cout << "type:" << tables[i].attributes[j].type << "\t";
			cout << "length:" << tables[i].attributes[j].length << "\t";
			cout << "isPrimaryKey:" << tables[i].attributes[j].isPrimaryKey << "\t";
			cout << "isUnique:" << tables[i].attributes[j].isUnique << "\t";
			cout << "hasindex:" << tables[i].attributes[j].hasindex << endl;
		}
		cout << endl;
	}
}

void CatalogManager::ShowIndexCatalog() {	//for debug only
	cout << "## Number of indexes:" << indexNum << endl;
	for (int i = 0; i < indexNum; i++) {
		cout << "INDEX " << i << endl;
		cout << "Index Name: " << indexes[i].indexName << endl;
		cout << "Table Name: " << indexes[i].tableName << endl;
		cout << "Column Number: " << indexes[i].column << endl;
		cout << "Column Number of blocks occupied in disk: " << indexes[i].blockNum << endl;
		cout << "Type of the column: " << indexes[i].type << endl;
	}
	cout << endl;
}

void CatalogManager::initial() {	//initialize after the database switch
	InitialTable();
	InitialIndex();
}

void CatalogManager::store() {	//save all before the database switch
	StoreTable();
	StoreIndex();
}
