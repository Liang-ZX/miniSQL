#ifndef __CatalogManager_H__
#define __CatalogManager_H__
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class Attribute{	//the definition of an arrtibute
public:
	string name;
	int type;
	int length;	//the unit is block
	bool isPrimaryKey;	//whether it's a primary key
	bool isUnique;	//whether it's unique
	Attribute(){
		isPrimaryKey = false;
		isUnique = false;
	}
	Attribute(string n, int t, int l, bool iPK, bool iU)	
		:name(n), type(t), length(l), isPrimaryKey(iPK), isUnique(iU){}	//initialize
};

class Table{	//the definition of a table
public:
	string name;	
	int blockNum;	//number of blocks that the data occupies	
	int attriNum;	//number of attributes in the table
	int totalLength;	//total length of one record, which is equal to sum(attributes[i].length)
	vector<Attribute> attributes;	//attributes of the table
	Table(): blockNum(0), attriNum(0), totalLength(0){}	//initialize
};

class Index{	//the definition of an index
public:
	string indexName;	
	string tableName;	//the table on which the index is created
	int column;			//on which column the index is created, start from 0
	int blockNum;		//number of blocks that the data occupies
	Index(): column(0), blockNum(0){}	//initialize
};

//Realized functions:
//Read the Catalog information into memory before the program starts
//Write the Catalog information into the hard disk before the program ends
class CatalogManager{
private:
	vector<Table> tables;	//table lists
	vector<Index> indexes;	//index lists
	int tableNum;	//number of tables
	int indexNum;   //number of indexes
	
	//All information about tables will be stored in file table.catlog
	void InitialTable(){	//read table information into memory
		const string filename = "table.catlog";
		fstream fin(filename.c_str(), ios::in);
		fin >> tableNum;
		for (int i = 0; i < tableNum; i++){	//get information for each table
			Table tempT;
			fin >> tempT.name;
			fin >> tempT.blockNum;
			fin >> tempT.attriNum;			
			for (int j = 0; j < tempT.attriNum; j++){	//get attributes of the table
				Attribute tempA;
		 		fin >> tempA.name;
		 		fin >> tempA.type;
		 		fin >> tempA.length;
		 		fin >> tempA.isPrimaryKey;
		 		fin >> tempA.isUnique;
		 		tempT.attributes.push_back(tempA);
		 		tempT.totalLength += tempA.length;	//update the total length of one record
		 	}
		 	tables.push_back(tempT);
		}
		fin.close();
	}
	
	//All information about indexes will be stored in file index.catlog
	void InitialIndex(){	//read index information into memory
		const string filename = "index.catlog";
		fstream fin(filename.c_str(), ios::in);
		fin >> indexNum;
		for (int i = 0; i < indexNum; i++){	//get information for each index
			Index tempI;
			fin >> tempI.indexName;
			fin >> tempI.tableName;
			fin >> tempI.column;
			fin >> tempI.blockNum;
		 	indexes.push_back(tempI);
		}
		fin.close();
	}
	
	void StoreTable(){	//store table information into the hard disk
		const string filename = "table.catlog";
		fstream fout(filename.c_str(), ios::out);		
		fout << tableNum << endl;
		for (int i = 0; i < tableNum; i++){	//for each table
			fout << tables[i].name << " ";
			fout << tables[i].blockNum << " ";
			fout << tables[i].attriNum << endl;			
			for (int j = 0; j < tables[i].attriNum; j++){	//for each attribute
				fout << tables[i].attributes[j].name << " ";
				fout << tables[i].attributes[j].type << " ";
				fout << tables[i].attributes[j].length << " ";
				fout << tables[i].attributes[j].isPrimaryKey << " ";
				fout << tables[i].attributes[j].isUnique << endl;
		 	}
		}
		fout.close(); 
	}
	
	void StoreIndex(){	//store index information into the hard disk
		const string filename = "index.catlog";
		fstream fout(filename.c_str(), ios::out);
		fout << indexNum << endl;
		for (int i = 0; i < indexNum; i++){	//for each index
			fout << indexes[i].indexName << " ";
			fout << indexes[i].tableName << " ";
			fout << indexes[i].column << " ";
			fout << indexes[i].blockNum << endl;
		}
		fout.close(); 
	}

public: 		
	CatalogManager(){	//initialize
		InitialTable();
		InitialIndex();
	}
	
	~CatalogManager(){	//store
		StoreTable();
		StoreIndex();
	}
	
	void createTable(Table& table){	//save the table in the catalog
		tableNum++;
		tables.push_back(table);
	}

	void createIndex(Index index){	//save the index in the catalog
		indexNum++;
		indexes.push_back(index);
	}
	
	void dropTable(Table table){	//drop the table from the catalog
		dropTable(table.name);
	}
	
	void dropTable(string tableName){	//drop the table by name	
		for (int i = 0; i < tableNum; i++)
			if (tables[i].name == tableName){
				tables.erase(tables.begin() + i);
				tableNum--;
			}		
		for (int i = 0; i < indexNum; i++)//表被删除了，建在这个表上的所有的index也要跟着删除
			if (indexes[i].tableName == tableName){
				indexes.erase(indexes.begin() + i);
				indexNum--;
			}
	}
	
	void dropIndex(Index index){	//drop the index from the catalog
		dropIndex(index.indexName);
	}

	void dropIndex(string indexName){	//drop the index by name
		for (int i = 0; i < indexNum; i++)
			if (indexes[i].indexName == indexName){
				indexes.erase(indexes.begin() + i);
				indexNum--;
			}		
	}

	void update(Table& table){	//update the table in the catalog
		for (int i = 0; i < tableNum; i++)
			if (tables[i].name == table.name){
				tables[i].attriNum = table.attriNum;
				tables[i].blockNum = table.blockNum;
				tables[i].totalLength = table.totalLength;
				tables[i].attributes = table.attributes;
			}		
	}

	void update(Index& index){	//update the index in the catalog
		for (int i = 0; i < indexNum; i++)
			if (indexes[i].indexName == index.indexName){
				indexes[i].tableName = index.tableName;
				indexes[i].column = index.column;
				indexes[i].blockNum = index.blockNum;
			}
	}

	bool existTable(string tableName){	//whether the table already exists, true for yes
		for (int i = 0; i < tableNum; i++)
			if (tables[i].name == tableName)
				return true;
		return false;
	}

	bool existIndex(string indexName){	//whether the index name already exists, true for yes
		for (int i = 0; i < indexNum; i++)
			if (indexes[i].indexName == indexName)
				return true;
		return false;
	}
		
	bool existIndex(string tableName, int column){	//whether an index already exists for this location, true for yes
	
		for (int i = 0; i < indexes.size(); i++)
			if (indexes[i].tableName == tableName && indexes[i].column == column)
				return true;
		return false;
	}

	int getColumn(Table& table, string attriName){	//find out on which column the index is created
		for (int i = 0; i < table.attriNum; i++)
			if (table.attributes[i].name == attriName)
				return i;			
		return -1;
	}

	Table getTable(string tableName){	//get the table by name
		Table temp;
		for (int i = 0; i < tableNum; i++)
			if (tables[i].name == tableName)
				return tables[i];		
		return temp;
	}
		
	Index getIndex(string tableName, int column){	//get the index by location
		Index temp;
		for (int i = 0; i < indexNum; i++)
			if (indexes[i].tableName == tableName && indexes[i].column == column)
				return indexes[i];
		return temp;
	}

	Index getIndex(string indexName){	//get the index by name
		Index temp;
		for (int i = 0; i < tableNum; i++)
			if (indexes[i].indexName == indexName)
				return indexes[i];
		return temp;
	}
};
#endif
