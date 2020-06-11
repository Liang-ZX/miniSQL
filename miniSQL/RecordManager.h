#ifndef RECORDERMANAGER_H
#define RECORDERMANAGER_H
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "Tuple.h"
#include "buffermanager.h"
// #include "indexmanager.h"
#include "CatalogManager.h"

extern BufferManager buffer_manager;
// extern Index_Manager index_manager;
extern CatalogManager catalog_manager;

class RecordManager
{
private:
    /**
     * Function: Check whether the Itemlist can be inserted into table(check type)
     * Return: true for can be inserted
     * Table: the table
     * ItemList: the item list from tuple to be inserted 
    */
    bool CheckAttribute(const Table &table,const std::vector<Item> &ItemList) const;
    /**
     * Function: Check whether the ConditionList can be compared with table(check type and exists)
     * Return: true for can be compared
     * Table: the table
     * ConditionList: the ConditionList
    */
    bool CheckAttribute(const Table &table,const std::vector<Condition> &ConditionList) const;
public:
    RecordManager(/* args */){}
    ~RecordManager(){}
    /*
    * Function: create all files when create a table
    * Return:
    */
    int CreateTableAllFile(const std::string &TableName);
    /*
     * Function: delete all files when drop a table
     * Return:
    */
    int DropTableAllFile(const std:: string &TableName);
    /*
     * Function: Insert record into a table
     * Return: 
     * TableName: the name of the table
     * record: the data need to be inserted
    */
    int InsertRecord(const std::string &TableName,const Tuple &tuple);
    /*
    * Function: Delete all record of a table
    * Return: (int)the number of the record deleted
    * buffer_manager: BufferManager
    * TableName: the name of the table
    * return the number of the record deleted
    * Exception:1.Table dost not exist
    * */
    int DeleteRecord(const std::string &TableName);
    /**
     * Function: Delete record fits conditions in a table
     * Return: (int)the number of the records deleted
     * buffer_manager:BufferManager
     * TableName: the name of the table
     * ConditionList: the conditions
     * Exception:1.Table does not exist;2.Wrong type of data in condition
     * 暂时所有条目、条件均为字符串进行测试
    */
    int DeleteRecord(const std::string &TableName,const std::vector<Condition> &ConditionList);
    /*
    * Function: Select all records of a table
    * Return: (int) the number of the records selected
    * buffer_manager: BufferManager
    * TableName: the name of the table
    * res: the string stored the records selected
    * Exception:1.Table dost not exist
    * */
    int SelectRecord(const std::string &TableName,std::string &res);
    /**
     * Function: Select record fits conditions in a table
     * Return: (int) the number of the records selected
     * buffer_manager: BufferManager
     * TableName: the name of the table
     * ConditionList: the conditions
     * res: the string stored the records selected
     * Exception:1.Table does not exist;2.Wrong type of data in condition
     * 暂时所有条目、条件均为字符串进行测试
    */
    int SelectRecord(const std::string &TableName,const std::vector<Condition> &ConditionList,std::string &res);

};  

#endif