#ifndef __CatalogManager_H__
#define __CatalogManager_H__
#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <vector>
#include "buffermanager.h"
using namespace std;

extern BufferManager buffer_manager;

class Attribute {	//the definition of an attribute
public:
	string name;
	int type;	//-1 means float, 0 means int, 1~255 means string and its length
	int length;	//the unit is block
	bool isPrimaryKey;	//whether it's a primary key
	bool isUnique;	//whether it's unique
	bool hasindex;	//whether it has an index on it
	Attribute():type(0), length(0) {
		isPrimaryKey = false;
		isUnique = false;
		hasindex = false;
	}
	Attribute(string s) :name(s), type(0), length(0) {
		isPrimaryKey = false;
		isUnique = false;
		hasindex = false;
	}
	Attribute(string n, int t, int l, bool iPK, bool iU, bool hi)
		:name(n), type(t), length(l), isPrimaryKey(iPK), isUnique(iU), hasindex(hi) {}	//initialize
};

class Table {	//the definition of a table
public:
	string name;
	int blockNum;	//number of blocks that the data occupies	
	int attriNum;	//number of attributes in the table
	int totalLength;	//total length of one record, which is equal to sum(attributes[i].length)
	vector<Attribute> attributes;	//attributes of the table
	Table() : blockNum(0), attriNum(0), totalLength(0) {}	//initialize
};

class Index {	//the definition of an index
public:
	string indexName;
	string tableName;	//the table on which the index is created
	int column;			//on which column the index is created, start from 0
	int blockNum;		//number of blocks that the data occupies
	int type;
	Index() : column(0), blockNum(0), type(0) {}	//initialize
	Index(string s):indexName(s), column(0), blockNum(0), type(0) {}
};

//Realized functions:
//Read the Catalog information into memory before the program starts
//Write the Catalog information into the hard disk before the program ends
class CatalogManager {
private:
	vector<Table> tables;	//table lists
	vector<Index> indexes;	//index lists
	int tableNum = 0;	//number of tables
	int indexNum = 0;   //number of indexes

	//All information about tables will be stored in file table.catalog.txt
	void InitialTable();
	//All information about indexes will be stored in file index.catalog.txt
	void InitialIndex();
	//store table information into the hard disk
	void StoreTable();
	//store index information into the hard disk
	void StoreIndex();

public:
	//initialize
	CatalogManager() {
		InitialTable();
		InitialIndex();
	}
	//store
	~CatalogManager() {
		StoreTable();
		StoreIndex();
	}

	void createTable(Table& table);	//save the table in the catalog
	void createIndex(Index index);	//save the index in the catalog
	void dropTable(Table table);	//drop the table from the catalog
	void dropTable(string tableName);	//drop the table by name
	void dropIndex(Index index);	//drop the index from the catalog
	void dropIndex(string indexName);	//drop the index by name
	bool existTable(string tableName);	//whether the table already exists, true for yes
	bool existIndex(string indexName);	//whether the index name already exists, true for yes
	bool existIndex(string tableName, int column);	//whether an index already exists for this location, true for yes
	int getColumn(const Table& table, string attriName);	//find out on which column the index is created
	Table& getTable(string tableName);	//get the table by name
	Index& getIndex(string tableName, int column);	//get the index by location
	Index& getIndex(string indexName);	//get the index by name
	void getIndex(string tableName, vector<Index>& answer);	//get indexes by tablename
	void ShowTableCatalog();	//for test only
	void ShowIndexCatalog();	//for test only
};
#endif
