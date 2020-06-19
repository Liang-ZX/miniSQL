#ifndef RECORDERMANAGER_H
#define RECORDERMANAGER_H
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <set>
#include "Tuple.h"
#include "buffermanager.h"
#include "indexmanager.h"
#include "CatalogManager.h"

extern BufferManager buffer_manager;
// extern Index_Manager index_manager;
extern CatalogManager catalog_manager;

using namespace std;
class RecordManager
{
private:
    /**
     * auxiliary function
     * Check whether an ItemType fit the attribute's type
    */
    bool SameType(short AttrType,short ItemType) const;
    /**
     * auxiliary function
     * Function: Check if an item's data is fit a condition
    */
    template<typename T>
    bool CheckConditionData(const T &item_data,Relation relation,const T &condition_data) const;
    /**
     * auxiliary function
     * Function:Check if an item is fit a condition
    */
    bool CheckCondition(const Item &item,const Condition &condition) const;
    /**
     * auxiliary function
     * Function: Check if a Record is fit ConditionList
     * table: to get the data type
     * Record: the record in string type    |temp|123.12|12|
     * Conditionlist: the Condition list
    */
    bool CheckConditionList(const Table &table,const Tuple &tuple,const vector<Condition> &ConditionList) const;
    /**
     * auxiliary function
     * Function: convert a Record into a Tuple
     * Table: table for get the item's type
     * tuple: record in tuple type
    */
    const Tuple RecordtoTuple(const Table &table,const string &Record) const;
    /**
     * auxiliary function
     * Function: Check whether the Itemlist can be inserted into table(check type)
     * Return: true for can be inserted
     * Table: the table
     * ItemList: the item list from tuple to be inserted 
    */
    bool CheckAttribute(const Table &table,const vector<Item> &ItemList) const;
    /**
     * auxiliary function
     * Function: Check whether the ConditionList can be compared with table(check type and exists)
     * Return: true for can be compared
     * Table: the table
     * ConditionList: the ConditionList
    */
    bool CheckAttribute(const Table &table,const vector<Condition> &ConditionList) const;
    /**
     * auxiliary function
     * Function: Delete records fit ConditionList in block_data(string)
    */
    int DeleteRecord(const Table &table,string &block_data,const vector<Condition> &ConditionList, Index_Manager &index_manager,const vector<Index> &IndexList) const;
    /**
     * auxiliary function
     * Function: Convert data(string) in a block to vector<Tuple> stored in tuplelist
    */
    int BlocktoTuples(const Table &table,string &block_data,vector<Tuple> TupleList)const;
    /**
     * auxiliary function
     * Function: Select Record(string) in a  block_data(string) stored in res(string)
     * Return: The number of the Records selected
     * 
    */
    int SelectRecord(const Table &table,const string &block_data,const vector<Condition> &ConditionList,string &res) const;
    /**
     * auxiliary function
     * Function: insert record(string) into block_data(string)
     * return: true means inserting successfully
     * 
    */
    bool InsertRecord(string &block_data, const string &record);
    /**
     * auxiliary function
     * Function: Check whether the tuple has attribute unique error 
    */
    bool CheckUnique(const Table &table, const Tuple &tuple);

    /**
     * Function: Get block_id[] the record in
     * return -1 means scan all block,return 0,1,2... means the size of block_id
     * 
    */
    int GetRecordBlock(const Table &table,Index_Manager &index_manager,set<int> &block_id,const vector<Condition> &ConditionList);
    /**
     * auxiliary function
     * Function: Use index and ConditionList to get the data range 
    */
    bool GetIndexRange(const Table &table,const Index &index,const vector<Condition> &Conditionlist,Item &minItem,Item &maxItem);
public:
    RecordManager(/* args */){}
    ~RecordManager(){}
    /*
    * Function: create file when create a table
    * Return:
    * Exception: 
    */
    int CreateTableFile(const string &TableName);
    /*
     * Function: delete file when drop a table
     * Return:
     * Exception:
     * Note: Check table not exist then call this function
    */
    int DropTableFile(const  string &TableName);
    /*
     * Function: Insert record into a table
     * Return: 
     * TableName: the name of the table
     * record: the data need to be inserted
     * Exception: 1.Type error(*);  2.Unique attribute error; 3.Table does not exist(*)
    */
    int InsertRecord(const string &TableName,const Tuple &tuple);
    /*
    * Function: Delete all record of a table
    * Return: (int)the number of the record deleted
    * buffer_manager: BufferManager
    * TableName: the name of the table
    * return the number of the record deleted
    * Exception:1.Table dost not exist(*)
    * */
    int DeleteRecord(const string &TableName);
    /**
     * Function: Delete record fits conditions in a table
     * Return: (int)the number of the records deleted
     * buffer_manager:BufferManager
     * TableName: the name of the table
     * ConditionList: the conditions
     * Exception:1.Table does not exist;2.Wrong type of data in condition; 3.Condition attribute does not exist
    */
    int DeleteRecord(const string &TableName,const vector<Condition> &ConditionList);
    /*
    * Function: Select all records of a table
    * Return: (int) the number of the records selected
    * buffer_manager: BufferManager
    * TableName: the name of the table
    * res: the string stored the records selected
    * Exception:1.Table dost not exist(*)
    * */
    int SelectRecord(const string &TableName,string &res);
    /**
     * Function: Select record fits conditions in a table (*)
     * Return: (int) the number of the records selected
     * buffer_manager: BufferManager
     * TableName: the name of the table
     * ConditionList: the conditions
     * res: the string stored the records selected
     * Exception:1.Table does not exist;2.Wrong type of data in condition 3.Condition attribute does not exist
    */
    int SelectRecord(const string &TableName,const vector<Condition> &ConditionList,string &res);
};  

#endif