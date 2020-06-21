// #define DEBUG_ON
#include "RecordManager.h"


int RecordManager::CreateTableFile(const string &TableName)
{
    // 调用函数之前确认table是否存在!
    #ifdef DEBUG_ON
    cout << "Debug(Record):begin create " << TableName << "\n";
    // if(catalog_manager.existTable(TableName))
    // {
    //     cout << "ERROR(Record):Table with same name existed!\n";
    //     return 0;
    // }
    #endif

    //create table file
    buffer_manager.writeFile("",TableName,0,0);
    #ifdef DEBUG_ON
    cout << "Debug(Record):End create " << TableName << "\n";
    #endif
    return 1;
}

int RecordManager::DropTableFile(const string &TableName)
{
    #ifdef DEBUG_ON
    //check table exist
    if(catalog_manager.existTable(TableName) == false)
    {
        cout << "ERROR(Record):The table does not exist!\n";
        return 0;
    }
    cout << "Debug(Record):begin drop "<< TableName << "\n";
    #endif
    //逻辑上删除文件
    buffer_manager.deleteFile(TableName,0);
    #ifdef DEBUG_ON
    //删除catalog文件中的信息
    catalog_manager.dropTable(TableName);
    //drop index
    Index_Manager index_manager(TableName);
    index_manager.Drop_All();
    cout << "Debug(Record):end drop " << TableName <<"\n";
    #endif
    return 1;
}

int RecordManager::InsertRecord(const string &TableName,const Tuple &tuple)
{
    #ifdef DEBUG_ON
    clock_t begin_time = clock(),end_time = clock();
    #endif
    #ifdef DEBUG_ON
    //check table exist
    if (catalog_manager.existTable(TableName) == false)
    {
        #ifdef DEBUG_ON
        end_time = clock();
        cout << end_time - begin_time << "ms\n";
        cout << "ERROR(Record):The table does not exit.\n";
        #endif
        return 0;
    }
    #endif
    //check attributes type
    Table &table = catalog_manager.getTable(TableName);
    if (CheckAttribute(table,tuple.GetItemList()) == false)
    {
        cout << "ERROR: Incorrect attribute values!\n";
        return -1;
    }
    //check unique attributes
    if(CheckUnique(table,tuple) == false)
    {
        cout << "ERROR: Duplicate entry!\n";
        return -1;
    }
    //start insert record
    string record;
    record.assign(tuple.TupletoString());       //get record in string
    #ifdef DEBUG_ON
    cout << "Debug(Record):Begin insert record.\n";
    #endif
    int record_length = record.length();
    if (table.blockNum == 0) table.blockNum++;        //if the table is empty, we need to create a block first
    for(int block_num = 0;block_num < table.blockNum;block_num++)
    {
        string block_data = buffer_manager.readFile(TableName,0,block_num);
        if(InsertRecord(block_data,record))           //if insert record in current block, we need insert data into index
        {
            buffer_manager.writeFile(block_data,TableName,0,block_num);
            //insert index
            vector<Index> IndexList;
            catalog_manager.getIndex(TableName,IndexList);
            if(IndexList.size() == 0) break;
            Index_Manager index_manager(TableName);
            for(int i = 0;i < IndexList.size();i++)    //insert data to all indexes
            {
                int column = IndexList[i].column;
                if(tuple.ItemList[column].type == -1) 
                    index_manager.Insert(IndexList[i].indexName,tuple.ItemList[column].f_data,block_num);
                else if(tuple.ItemList[column].type == 0) 
                    index_manager.Insert(IndexList[i].indexName,tuple.ItemList[column].i_data,block_num);
                else if(tuple.ItemList[column].type > 0 && tuple.ItemList[column].type <= MAX_CHAR_LENGTH) 
                    index_manager.Insert(IndexList[i].indexName,tuple.ItemList[column].str_data,block_num);
                else return -1;     //type ERROR
            }
            break;
        }
        if(block_num == table.blockNum - 1) table.blockNum++;       //所有块都没有空间，启用新块
    }
    #ifdef DEBUG
    end_time = clock();
    cout << end_time - begin_time << "ms\n";
    cout << "Debug(Record):End insert record.\n";
    #endif
    return 1;
}

int RecordManager::DeleteRecord(const string &TableName)
{
    #ifdef DEBUG_ON
    clock_t begin_time = clock(),end_time = clock();
    #endif
    #ifdef DEBUG_ON
    //check exist
    if(catalog_manager.existTable(TableName) == false)
    {
        #ifdef DEBUG_ON
        end_time = clock();
        cout << end_time - begin_time << "ms\n";
        cout << "Debug(Record):No such table!\n";
        #endif
        return -1;
    }
    #endif
    #ifdef DEBUG_ON
    cout << "Debug(Record):Start delete all of " << TableName << endl;
    #endif
    int count = 0;
    Table &table = catalog_manager.getTable(TableName);
    for(int block_num = 0;block_num < table.blockNum;block_num++)       //scan linearly
    {
        string teststr = buffer_manager.readFile(TableName,0,block_num);
        int block_length = teststr.length();
        for(int i = 0;i < block_length;i++) 
            if(teststr[i] == RECORD_SEPARATOR) count++;                 //count number of records
        buffer_manager.writeFile("",TableName,0,block_num);
    }
    //clear index data
    Index_Manager index_manager(TableName);
    index_manager.Clear_Index();
    #ifdef DEBUG_ON
    end_time = clock();
    cout << end_time - begin_time << "ms\n";
    cout << "Debug(Record):End delete all of " << TableName << endl;
    cout << "Debug(Record):Delete " << count << " records\n";
    #endif
    return count;
}

int RecordManager::DeleteRecord(const string &TableName,const vector<Condition> &ConditionList)
{
    #ifdef DEBUG_ON
    clock_t begin_time = clock(), end_time = clock();
    #endif
    //check table exist
    #ifdef DEBUG_ON
    if(catalog_manager.existTable(TableName) == false)
    {
        #ifdef DEBUG_ON
        end_time = clock();
        cout << end_time - begin_time << "ms\n";
        cout << "Debug(Record):No such table!\n";
        #endif
        return -1;
    }
    #endif
    //确认ConditionList中Attribute存在且数据类型无误
    Table &table = catalog_manager.getTable(TableName);
    if(CheckAttribute(table,ConditionList) == false) 
    {
        cout << "ERROR: Incorrect attribute values!\n";
        return -1;  //wrong type
    }
    #ifdef DEBUG_ON
    cout << "Debug(Record):Start delete record fit conditions in " << TableName << endl;
    #endif
    int count = 0;
    //use index to locate blocks
    Index_Manager index_manager(TableName);
    set<int> block_id;
    vector<Index> IndexList;
    catalog_manager.getIndex(TableName,IndexList);
    if(GetRecordBlock(table,index_manager,block_id,ConditionList) == -1)        
        for(int block_num = 0;block_num < table.blockNum;block_num++)           //if can't locate blocks, scan linearly
        {
            #ifdef DEBUG_ON
            cout << "Debug(Record):In delete conditionally, block:" << block_num << endl;
            #endif
            string block_data = buffer_manager.readFile(TableName,0,block_num);
            count += DeleteRecord(table,block_data,ConditionList,index_manager,IndexList);
            buffer_manager.writeFile(block_data,TableName,0,block_num);
        }
    else 
        for(set<int>::iterator i = block_id.begin();i !=  block_id.end();i++)   //if get block set
        {
            string block_data = buffer_manager.readFile(TableName,0,*i);
            count += DeleteRecord(table,block_data,ConditionList,index_manager,IndexList);
            buffer_manager.writeFile(block_data,TableName,0,*i);
        }
    #ifdef DEBUG_ON
    end_time = clock();
    cout << "Debug(Record):Delete " << count << " records.\n";
    cout << end_time - begin_time << "ms\n";
    #endif
    return count;
}

int RecordManager::SelectRecord(const string &TableName,string &res)
{
    int count = 0;
    #ifdef DEBUG_ON
    clock_t begin_time = clock(),end_time = clock();
    #endif
    #ifdef DEBUG_ON
    if(catalog_manager.existTable(TableName) == false)      //table not exist
    {
        #ifdef DEBUG_ON
        end_time = clock();
        cout << end_time - begin_time << "ms\n";
        cout << "Debug(Record):No such table!\n";
        #endif
        return -1;
    }
    #endif
    Table &table = catalog_manager.getTable(TableName);
    for(int block_num = 0;block_num < table.blockNum;block_num++)       //scan linearly to get all records
    {
        string block_data = buffer_manager.readFile(TableName,0,block_num);
        #ifdef DEBUG_ON
        cout << "Debug(Record):In select all:block " << block_num << endl <<  block_data << endl;
        #endif
        int data_length = block_data.length();
        for(int pos = 0;pos < data_length;pos++)    if(block_data[pos] == ITEM_SEPARATOR)//find record head |.....$
        {
            int end_address = block_data.find(RECORD_SEPARATOR,pos);
            count++;
            res = res + block_data.substr(pos,end_address - pos);
            res = res + "\n";
            pos = end_address;
        }
    }
    return count;
}

int RecordManager::SelectRecord(const string &TableName,const vector<Condition> &ConditionList,string &res)
{
    //check table exist
    #ifdef DEBUG_ON
    if(catalog_manager.existTable(TableName) == false)
    {
        #ifdef DEBUG_ON
        cout << "Debug(Record):No such table!\n";
        #endif
        return -1;
    }
    #endif
    Table &table = catalog_manager.getTable(TableName);
    // check conditionList is valid
    if(CheckAttribute(table,ConditionList) == false)
    {
        cout << "ERROR: Incorrect attribute values!\n";
        return -1;
    }
    #ifdef DEBUG_ON
    cout << "Debug(Record):Start select record fit conditions in " << TableName << endl;
    #endif
    int count = 0;
    Index_Manager index_manager(TableName);
    set<int> block_id;
    //use index to get block set
    if(GetRecordBlock(table,index_manager,block_id,ConditionList) == -1)
        for(int block_num = 0;block_num < table.blockNum;block_num++)           //if can't get block set, scan linearly
        {
            string block_data = buffer_manager.readFile(TableName,0,block_num);
            count += SelectRecord(table,block_data,ConditionList,res);
        }
    else
        for(set<int>::iterator i = block_id.begin();i != block_id.end();i++)    //if get block set
        {
            string block_data = buffer_manager.readFile(TableName,0,*i);
            count += SelectRecord(table,block_data,ConditionList,res);
        }
    return count;
}

bool RecordManager::CheckAttribute(const Table &table,const vector<Item> &ItemList) const
{
    if (table.attriNum != ItemList.size()) return false;
    for(int i = 0;i < table.attriNum;i++) if(SameType(table.attributes[i].type,ItemList[i].type) == false) return false;
    return true;
}

bool RecordManager::CheckAttribute(const Table &table,const vector<Condition> &ConditionList) const
{
    // if (table.attriNum != ItemList.size()) return false;
    int ConditionNum = ConditionList.size();
    // bool flag = 1;
    for(int i = 0;i < ConditionNum;i++)
    {
        bool find = 0;
        // if (ConditionList[i].AttrName == )
        for(int j = 0;j < table.attriNum;j++)
        {
            if(table.attributes[j].name == ConditionList[i].AttrName)
            {
                find = 1;
                if(SameType(table.attributes[j].type,ConditionList[i].item.type) == false)
                {
                    cout << "Debug(Record):The condition attribute type is wrong!\n";
                    return false;
                }
                else break;
            }
        }
        if (!find)
        {
            cout << "Debug(Record):The condition attribute dose not exist!\n";
            return false;
        }
    }
    return true;
}

const Tuple RecordManager::RecordtoTuple(const Table &table,const string &Record) const
{
    int item_begin = 1;
    Tuple result;
    for(int i = 0;i < table.attriNum;i++)
    {
        int item_end = Record.find(ITEM_SEPARATOR, item_begin);
        if(item_end == -1)
        {
            result.ItemList.clear();
            #ifdef DEBUG_ON
            cout << "Debug(Record):ERROR in RecordtoTuple:number of item incorrect\n";
            #endif
            return result;
        }
        Item tp;
        tp.type = table.attributes[i].type;
        if(tp.type == -1) tp.f_data = atof(Record.substr(item_begin,item_end - item_begin).c_str());
        else if(tp.type == 0) tp.i_data = atoi(Record.substr(item_begin,item_end - item_begin).c_str());
        else if(tp.type > 0 && tp.type <= MAX_CHAR_LENGTH) tp.str_data.assign(Record.substr(item_begin,item_end - item_begin));
        else 
        {
            result.ItemList.clear();
            #ifdef DEBUG_ON
            cout << "Debug(Record):ERROR in RecordtoTuple:type of item is out of range\n";
            #endif
            return result;
        }
        result.ItemList.push_back(tp);
        item_begin = item_end + 1;
    }
    return result;
}

bool RecordManager::CheckConditionList(const Table &table,const Tuple &tuple,const vector<Condition> &ConditionList) const
{
    for(int condition_id = 0;condition_id < ConditionList.size();condition_id++)
    {
        bool flagExist = 0;
        for(int attr_id = 0;attr_id < table.attriNum;attr_id++) if(table.attributes[attr_id].name == ConditionList[condition_id].AttrName)
        {
            flagExist = 1;
            if(CheckCondition(tuple.ItemList[attr_id],ConditionList[condition_id]) == false) return 0;
        }
        if(flagExist == 0) 
        {
            #ifdef DEBUG_ON
            cout << "Debug(Record):ERROR in CheckConditionList:type not exist!\n";
            #endif
            return 0;
        }
    }
    return 1;
}

bool RecordManager::CheckCondition(const Item &item,const Condition &condition) const
{
    if(item.type == 0) return CheckConditionData(item.i_data,condition.relation,condition.item.i_data);
    else if(item.type == -1) return CheckConditionData(item.f_data,condition.relation,condition.item.f_data);
    else if(item.type > 0 && item.type <= MAX_CHAR_LENGTH) return CheckConditionData(item.str_data,condition.relation,condition.item.str_data);
    return 0;
}

template<typename T>
bool RecordManager::CheckConditionData(const T &item_data,Relation relation,const T &condition_data)const
{
    if(relation == GREATER) return item_data > condition_data;
    else if(relation == GREATER_EQUAL) return item_data >= condition_data;
    else if(relation == EQUAL) return item_data == condition_data;
    else if(relation == LESS_EQUAL) return item_data <= condition_data;
    else if(relation == LESS) return item_data < condition_data;
    else if(relation == NOT_EQUAL) return item_data != condition_data;
    else return 0;
}

bool RecordManager::SameType(int AttrType,int ItemType) const
{
    if(AttrType == ItemType || ((ItemType > 0) && (ItemType <= AttrType))) return 1;
    else return 0;
}

int RecordManager::DeleteRecord(const Table &table,string &block_data,const vector<Condition> &ConditionList,Index_Manager &index_manager,const vector<Index> &IndexList) const
{
    int block_length = block_data.length();
    int count = 0;
    for(int pos = 0;pos < block_length;pos++)
    {
        if(block_data[pos] == ITEM_SEPARATOR)       ///head of record |......$
        {
            int end_address = block_data.find(RECORD_SEPARATOR,pos);
            Tuple tuple = RecordtoTuple(table,block_data.substr(pos,end_address - pos));
            if(CheckConditionList(table,tuple,ConditionList))
            {
                if(IndexList.size() > 0) //if the table has index
                {
                    for(int i = 0;i < IndexList.size();i++)
                    {
                        int column = IndexList[i].column;
                        if (tuple.ItemList[column].type == -1)                                              //float
                            index_manager.Delete(IndexList[i].indexName,tuple.ItemList[column].f_data);
                        else if (tuple.ItemList[column].type == 0)                                          //int
                            index_manager.Delete(IndexList[i].indexName,tuple.ItemList[column].i_data);
                        else if (tuple.ItemList[column].type > 0 && tuple.ItemList[i].type < 256)           //string
                            index_manager.Delete(IndexList[i].indexName,tuple.ItemList[column].str_data);
                    }
                }
                count++;
                for(int i = pos;i <= end_address;i++) block_data[i] = EMPTY_CHAR;
            }
            pos = end_address;
        }
    }
    return count;
}

int RecordManager::BlocktoTuples(const Table &table,string &block_data,vector<Tuple> TupleList)const
{
    int block_length = block_data.length();
    int count = 0;
    for(int pos = 0;pos < block_length;pos++)
    {
        if(block_data[pos] == ITEM_SEPARATOR) //head of record |......$
        {
            int end_address = block_data.find(RECORD_SEPARATOR,pos);
            Tuple tp = RecordtoTuple(table,block_data.substr(pos,end_address - pos));
            TupleList.push_back(tp);
            pos = end_address;
            count++;
        }
    }
    return count;
}
int RecordManager::SelectRecord(const Table& table, const string& block_data, const vector<Condition>& ConditionList, string& res) const
{
    int block_length = block_data.length();
    int count = 0;
    for(int pos = 0;pos < block_length;pos++)
    {
        if(block_data[pos] == ITEM_SEPARATOR)       ///head of record |......$
        {
            int end_address = block_data.find(RECORD_SEPARATOR,pos);
            Tuple tuple = RecordtoTuple(table,block_data.substr(pos,end_address - pos));
            if(CheckConditionList(table,tuple,ConditionList))
            {
                count++;
                res = res + block_data.substr(pos,end_address - pos);
                res = res + "\n";
            }
            pos = end_address;
        }
    }
    return count;
}

bool RecordManager::InsertRecord(string &block_data, const string &record)
{
    int data_length = block_data.length();
    int record_length = record.length();
    int usable_space = 0;
    int begin_address = -1,end_address = -1;
    bool FlagDone = 0;
    for(int pos = 0;pos < BLOCK_SIZE;pos++)
    {
        if(pos >= data_length)   //未使用过的空间
        {
            if(usable_space == 0) begin_address = pos;
            usable_space += BLOCK_SIZE - data_length;
        }
        else if(block_data[pos] == EMPTY_CHAR)  //删除过的空间
        {
            if (usable_space == 0) begin_address = end_address = pos;
            usable_space++;
        }
        else if(block_data[pos] != EMPTY_CHAR)  //占用的空间
        {
            usable_space = 0;
        }
        if(usable_space >= record_length)   //找到了足够空间
        {
            end_address = pos;
            block_data.replace(begin_address,record_length,record);     //length?last?
            // buffer_manager.writeFile(block_data,TableName,0,block_num);
            return 1;                   //set flag
        }
        else if(pos >= data_length)          //该块空间不足
        {
            break;
        }
    }
    return 0;
}

bool RecordManager::CheckUnique(const Table &table,const Tuple &tuple)
{
    vector<int> unique;     //column with unique propoty
    for(int i = 0;i < table.attriNum;i++)
    {
        if(table.attributes[i].isUnique) unique.push_back(i);
    }
    if(unique.size() == 0) return 1;        //no unique column
    int unique_num = unique.size();
    vector<Tuple> TupleList;
    bool NeedConvert = 0;                    //if any unique attribut does not have index, we need to convert all records
    for (int i = 0; i < unique_num; i++)
    {
        if (table.attributes[unique[i]].hasindex == false) NeedConvert = 1;
    }
    if(NeedConvert)
        for(int block_num = 0;block_num < table.blockNum;block_num++)       //convert all records into tuplelist
        {
            string block_data = buffer_manager.readFile(table.name,0,block_num);
            BlocktoTuples(table,block_data,TupleList);
        }
    int tuple_num = TupleList.size();
    Index_Manager index_manager(table.name);
    for(int i = 0;i < unique_num;i++)
    {
        if (table.attributes[unique[i]].hasindex == false)   //without index. scan linearly
        {
            for (int tuple_id = 0; tuple_id < tuple_num; tuple_id++)
            {
                bool flag = 1;
                if (table.attributes[unique[i]].type == -1)
                    flag = CheckConditionData(TupleList[tuple_id].ItemList[unique[i]].f_data, EQUAL, tuple.ItemList[unique[i]].f_data);
                else if (table.attributes[unique[i]].type == 0)
                    flag = CheckConditionData(TupleList[tuple_id].ItemList[unique[i]].i_data, EQUAL, tuple.ItemList[unique[i]].i_data);
                else if (table.attributes[unique[i]].type > 0 && table.attributes[unique[i]].type < 256)
                    flag = CheckConditionData(TupleList[tuple_id].ItemList[unique[i]].str_data, EQUAL, tuple.ItemList[unique[i]].str_data);
                if (!flag) return 0;
            }
        }
        else //with index to check if exists
        {
            Index index = catalog_manager.getIndex(table.name, unique[i]);
            int temp;
            if (index.type == 0)
                if (index_manager.Search(index.indexName, tuple.ItemList[unique[i]].i_data, temp)) return 0;
                else;
            else if (index.type == -1)
                if (index_manager.Search(index.indexName, tuple.ItemList[unique[i]].f_data, temp)) return 0;
                else;
            else if (index.type > 0 && index.type < 256)
                if (index_manager.Search(index.indexName, tuple.ItemList[unique[i]].str_data, temp)) return 0;
                else;
        }
    }
    return 1;
}

int RecordManager::GetRecordBlock(const Table &table,Index_Manager &index_manager,set<int> &block_id,const vector<Condition> &ConditionList)
{
    vector<Index> IndexList;
    catalog_manager.getIndex(table.name,IndexList);
    if(IndexList.size() == 0) return -1;
    // for(int i = 0)
    for(int i = 0;i < ConditionList.size();i++) if(ConditionList[i].relation == EQUAL)
    {
        int column = catalog_manager.getColumn(table,ConditionList[i].AttrName);
        if (table.attributes[column].hasindex == false) continue;       //if does not have index on the column
        Index index = catalog_manager.getIndex(table.name,column);
        int block_num = -1;
        if(ConditionList[i].item.type == -1)    //float
        {
            if(index_manager.Search(index.indexName,ConditionList[i].item.f_data,block_num) == false) return 0;
            else
            {
                block_id.insert(block_num);
                return 1;
            }
        }
        else if(ConditionList[i].item.type == 0)    //int
        {
            if(index_manager.Search(index.indexName,ConditionList[i].item.i_data,block_num) == false) return 0;
            else 
            {
                block_id.insert(block_num);
                return 1;
            }
        }
        else if(ConditionList[i].item.type > 0 && ConditionList[i].item.type <= MAX_CHAR_LENGTH)        //char(type)
        {
            if(index_manager.Search(index.indexName,ConditionList[i].item.str_data,block_num) == false) return 0;
            else 
            {
                block_id.insert(block_num);
                return 1;
            }
        }
    }
    for(int i = 0;i < IndexList.size();i++)
    {
        Item minItem,maxItem;
        if(GetIndexRange(table,IndexList[i],ConditionList,minItem,maxItem))
        {
            if(minItem.type == -1) index_manager.Search(IndexList[i].indexName,minItem.f_data,maxItem.f_data,block_id);
            else if(minItem.type == 0) index_manager.Search(IndexList[i].indexName,minItem.i_data,maxItem.i_data,block_id);
            return block_id.size();
        }
    }
    return -1;
}

bool RecordManager::GetIndexRange(const Table &table,const Index &index,const vector<Condition> &Conditionlist,Item &minItem,Item &maxItem)
{
    minItem.f_data = -1e8;
    minItem.i_data = -1e8;
    maxItem.f_data = 1e8;
    maxItem.i_data = 1e8;
    // minItem.str_data.assign("");
    // maxItem.str_data.assign("");
    bool flag = 0;
    for(int i = 0;i < Conditionlist.size();i++)
    {
        int column = catalog_manager.getColumn(table,Conditionlist[i].AttrName);
        if(column == index.column)
        {
            flag = 1;
            minItem.type = maxItem.type = Conditionlist[i].item.type;
            if(Conditionlist[i].relation == GREATER || Conditionlist[i].relation == GREATER_EQUAL) 
            {
                // if(Conditionlist[i].item.type == 0)
                minItem.f_data = max(Conditionlist[i].item.f_data,minItem.f_data);
                minItem.i_data = max(Conditionlist[i].item.i_data,minItem.i_data);
                // if(Conditionlist[i].item.type > 0)
                //     if(minItem.str_data.length() == 0 || minItem.str_data < Conditionlist[i].item.str_data)
                //         minItem.str_data.assign(Conditionlist[i].item.str_data);
            }
            else if(Conditionlist[i].relation == LESS_EQUAL || Conditionlist[i].relation == LESS)
            {
                maxItem.f_data = min(Conditionlist[i].item.f_data,maxItem.f_data);
                maxItem.i_data = min(Conditionlist[i].item.i_data,maxItem.i_data);
                // if(Conditionlist[i].item.type > 0)
                //     if(maxItem.str_data.length() == 0 || maxItem.str_data > Conditionlist[i].item.str_data)
                //         maxItem.str_data.assign(Conditionlist[i].item.str_data);
            }
        }
    }
    return flag;
}
