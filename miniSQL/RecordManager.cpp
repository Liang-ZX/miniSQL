#include "RecordManager.h"

int RecordManager::CreateTableAllFile(const std::string &TableName)
{
    cout << "Debug(Record):begin create " << TableName << "\n";
    // if(buffer_manager.CheckTableExist(TableName))
    if(catalog_manager.existTable(TableName))
    {
        cout << "Error(Record):Table with same name existed!\n";
        return 0;
    }
    buffer_manager.writeFile("",TableName,0,0);
    buffer_manager.writeFile("",TableName,1,0);
    buffer_manager.writeFile("exist",TableName,2,0); //创建catalog会有问题
    cout << "Debug(Record):End create " << TableName << "\n";
    return 1;
}

int RecordManager::DropTableAllFile(const std::string &TableName)
{
    // if(buffer_manager.CheckTableExist(TableName) == false)
    if(catalog_manager.existTable(TableName) == false)
    {
        cout << "Error(Record):The table does not exist!\n";
        return 0;
    }
    cout << "Debug(Record):begin drop "<< TableName << "\n";
    std::string db_name = buffer_manager.GetDB_name();
    //TODO释放所有相关block不写回
    buffer_manager.writeBlockAll();
    std::string TableFile = "Data_files\\" + db_name + "\\table\\" + TableName + ".txt";
    std::string IndexFile = "Data_files\\" + db_name + "\\index\\" + TableName + ".txt";
    std::string CatalogFile = "Data_files\\" + db_name + "\\catalog\\" + TableName + ".txt";
    remove(TableFile.c_str());
    remove(IndexFile.c_str());
    remove(CatalogFile.c_str());
    cout << "Debug(Record):end drop " << TableName <<"\n";
    return 1;
}

int RecordManager::InsertRecord(const std::string &TableName,const Tuple &tuple)
{
    clock_t begin_time = clock(),end_time = clock();
    // if (buffer_manager.CheckTableExist(TableName) == false)
    if (catalog_manager.existTable(TableName) == false)
    {
        end_time = clock();
        cout << end_time - begin_time << "ms\n";
        cout << "Error(Record):The table does not exit.\n";
        return 0;
    }
    //check attributes type
    Table table = catalog_manager.getTable(TableName);
    if (CheckAttribute(table,tuple.GetItemList()) == false)
    {
        cout << "Error(Record):The attribute types are incorrect!\n";
        return 0;
    }
    //TODO处理index的冲突
    std::string record;
    record.assign(tuple.TupletoString());
    cout << "Debug(Record):Begin insert record.\n";
    int record_length = record.length();
    // int FileBlockNum = table.blockNum;
    for(int block_num = 0;block_num < table.blockNum;block_num++)
    {
        std::string block_data = buffer_manager.readFile(TableName,0,block_num,0);
        int begin_address = block_data.find_last_of(RECORD_SEPARATOR);
        if(begin_address == -1) begin_address = 0;
        else begin_address++;
        int usable_space = BLOCK_SIZE - begin_address;
        if(usable_space >= record_length)
        {
            cout << "Debug(Record):Substring of block:" << block_data << endl;
            cout << "Debug(Record):Get Insert block_num" << block_num << " begin_address " << begin_address <<"!\n" << record << endl;
            block_data.replace(begin_address,record_length,record);
            buffer_manager.writeFile(block_data,TableName,0,block_num);
            // buffer_manager.unsetBlockPin(TableName,0,block_num);
            break;
        }
        //如果当前块全部放满，则新增块
        if(block_num == table.blockNum - 1) table.blockNum++;
        // buffer_manager.unsetBlockPin(TableName,0,block_num);
    }
    end_time = clock();
    cout << end_time - begin_time << "ms\n";
    cout << "Debug(Record):End insert record.\n";
    return 1;
}

int RecordManager::DeleteRecord(const std::string &TableName)
{
    clock_t begin_time = clock(),end_time = clock();
    // if(buffer_manager.CheckTableExist(TableName) == false)
    if(catalog_manager.existTable(TableName) == false)
    {
        end_time = clock();
        cout << end_time - begin_time << "ms\n";
        cout << "Debug(Record):No such table!\n";
        return -1;
    }
    cout << "Debug(Record):Start delete all of " << TableName << endl;
    int count = 0;
    Table table = catalog_manager.getTable(TableName);
    for(int block_num = 0;block_num < table.blockNum;block_num++)
    {
        std::string teststr = buffer_manager.readFile(TableName,0,block_num);
        // if(teststr.length() == 0) break;    //若某块为空则结束（不严谨
        int block_length = teststr.length();
        for(int i = 0;i < block_length;i++) 
            if(teststr[i] == RECORD_SEPARATOR) count++;
        buffer_manager.writeFile("",TableName,0,block_num);
    }
    //TODO solve index
    end_time = clock();
    cout << end_time - begin_time << "ms\n";
    cout << "Debug(Record):End delete all of " << TableName << endl;
    cout << "Debug(Record):Delete " << count << " records\n";
    return count;
}

int RecordManager::DeleteRecord(const std::string &TableName,const std::vector<Condition> &ConditionList)
{
    clock_t begin_time = clock(), end_time = clock();
    // if(buffer_manager.CheckTableExist(TableName) == false)
    if(catalog_manager.existTable(TableName) == false)
    {
        end_time = clock();
        cout << end_time - begin_time << "ms\n";
        cout << "Debug(Record):No such table!\n";
        return -1;
    }
    //确认ConditionList中Attribute存在且数据类型无误
    Table table = catalog_manager.getTable(TableName);
    if(CheckAttribute(table,ConditionList) == false) return 0;

    cout << "Debug(Record):Start delete record fit conditions in " << TableName << endl;
    int count = 0;
    //TODO若有index则调用相关函数进行定位BLOCK

    for(int block_num = 0;block_num < table.blockNum;block_num++)
    {
        cout << "Debug(Record):In delete conditionally, block:" << block_num << endl;
        std::string block_data = buffer_manager.readFile(TableName,0,block_num);
        // if(block_data.length() == 0) break;  //若某块为空则结束（不严谨
        int block_length = block_data.length();
        int record_begin = 0;
        int record_end = block_data.find(RECORD_SEPARATOR,record_begin);
        //调用indexmanager
        while(record_end != -1 && record_begin < block_length)
        {
            bool is_delete = 0;
            
            for(int condition_id = 0;condition_id < ConditionList.size();condition_id++)
            {
                int pos = block_data.find(ConditionList[condition_id].item.str_data.c_str(),record_begin);
                cout << condition_id << "  " << pos <<  ConditionList[condition_id].item.str_data <<endl;
                if(pos != -1 && pos < record_end)
                {
                    block_data.replace(record_begin,record_end - record_begin + 1,"");
                    is_delete = 1;
                    count++;
                    break;
                }
            }
            if (!is_delete) record_begin = record_end + 1; 
            record_end = block_data.find(RECORD_SEPARATOR,record_begin);
            // else record_
        }
        buffer_manager.writeFile(block_data,TableName,0,block_num);
    }
    end_time = clock();
    cout << "Debug(Record):Delete " << count << " records.\n";
    cout << end_time - begin_time << "ms\n";
    return count;
}

int RecordManager::SelectRecord(const std::string &TableName,std::string &res)
{
    int count = 0;
    clock_t begin_time = clock(),end_time = clock();
    if(buffer_manager.CheckTableExist(TableName) == false)
    {
        end_time = clock();
        cout << end_time - begin_time << "ms\n";
        cout << "Debug(Record):No such table!\n";
        return -1;
    }
    // std::string res = "";
    for(int block_num = 0;block_num < MAX_BLOCK_NUM;block_num++)
    {
        std::string block_data = buffer_manager.readFile(TableName,0,block_num);
        cout << "Debug(Record):In select all:block " << block_num << endl <<  block_data;
        int block_length = block_data.length();
        if(block_length == 0)   //不严谨
        {
            end_time = clock();
            cout << end_time - begin_time << "ms\n";
            // res = res + "\0";
            for(int i = 0;i < res.length();i++)
            {
                if(res[i] == RECORD_SEPARATOR) 
                {
                    res[i] = '\n';
                    count++;
                }
            }
            return count;
        }
        else 
        {
            res = res + block_data.substr(0,block_length);
        }
    }
    return -1;
}

int RecordManager::SelectRecord(const std::string &TableName,const std::vector<Condition> &ConditionList,std::string &res)
{
    if(buffer_manager.CheckTableExist(TableName) == false)
    {
        cout << "Debug(Record):No such table!\n";
        return -1;
    }

    //TODO确认ConditionList中Attribute存在且数据类型无误
    cout << "Debug(Record):Start delete record fit conditions in " << TableName << endl;
    int count = 0;
    //TODO若有index则调用相关函数进行定位BLOCK
    for(int block_num = 0;block_num < MAX_BLOCK_NUM;block_num++)
    {
        std::string block_data = buffer_manager.readFile(TableName,0,block_num);
        int block_length = block_data.length();
        if (block_length == 0) break;   //不严谨
        int record_begin = 0;
        int record_end = block_data.find(RECORD_SEPARATOR,record_begin);
        while(record_end != -1 && record_begin < block_length)
        {
            for(int condition_id = 0;condition_id < ConditionList.size();condition_id++)
            {
                int pos = block_data.find(ConditionList[condition_id].item.str_data.c_str(),record_begin);
                cout << condition_id << "  " << pos <<  ConditionList[condition_id].item.str_data <<endl;
                if(pos != -1 && pos < record_end)
                {
                    res = res + block_data.substr(record_begin,record_end - record_begin);
                    res = res + '\n';
                    count++;
                    break;
                }
            }
            record_begin = record_end + 1; 
            record_end = block_data.find(RECORD_SEPARATOR,record_begin);
        }
        // buffer_manager.writeFile(block_data,TableName,0,block_num);
    }
    return count;
}

bool RecordManager::CheckAttribute(const Table &table,const std::vector<Item> &ItemList) const
{
    if (table.attriNum != ItemList.size()) return false;
    for(int i = 0;i < table.attriNum;i++) if(SameType(table.attributes[i].type,ItemList[i].type) == false) return false;
    return true;
}


bool RecordManager::CheckAttribute(const Table &table,const std::vector<Condition> &ConditionList) const
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
                    cout << "Debug(Record):The condition attribute type is wrong!";
                    return false;
                }
                else break;
            }
            if(!find)
            {
                cout << "Debug(Record):The condition attribute dose not exist!";
                return false;
            }
        }
    }
    return true;
}

const Tuple &RecordManager::RecordtoTuple(const Table &table,const std::string &Record) const
{
    int item_begin = 1;
    int item_end = Record.find(ITEM_SEPARATOR);
    Tuple result;
    for(int i = 0;i < table.attriNum;i++)
    {
        if(item_end == -1)
        {
            result.ItemList.clear();
            cout << "Debug(Record):Error in RecordtoTuple:number of item incorrect\n";
            return result;
        }
        Item tp;
        tp.type = table.attributes[i].type;
        if(tp.type == -1) tp.f_data = atof(Record.substr(item_begin,item_end - item_begin).c_str());
        else if(tp.type == 0) tp.i_data = atoi(Record.substr(item_begin,item_end - item_begin).c_str());
        else if(tp.type > 0 && tp.type <= 255) tp.str_data.assign(Record.substr(item_begin,item_end - item_begin));
        else 
        {
            result.ItemList.clear();
            cout << "Debug(Record):Error in RecordtoTuple:type of item is out of range\n";
            return result;
        }
        result.ItemList.push_back(tp);
    }
    return result;
}

bool RecordManager::CheckConditionList(const Table &table,const std::string &Record,const std::vector<Condition> ConditionList)
{
    Tuple tuple = RecordtoTuple(table,Record);
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
            cout << "Debug(Record):Error in CheckConditionList:type not exist!\n";
            return 0;
        }
    }
    return 1;
}

bool RecordManager::CheckCondition(const Item &item,const Condition &condition) const
{
    if(item.type == 0) return CheckConditionData(item.i_data,condition.relation,condition.item.i_data);
    else if(item.type == -1) return CheckConditionData(item.f_data,condition.relation,condition.item.f_data);
    else if(item.type > 0 && item.type <= 255) return CheckConditionData(item.str_data,condition.relation,condition.item.str_data);
}
template<typename T>
bool RecordManager::CheckConditionData(const T &item_data,Relation relation,const T &condition_data)const
{
    if(relation == GREATER) return item_data > condition_data;
    else if(relation == GREATER_EQUAL) return item_data >= condition_data;
    else if(relation == EQUAL) return item_data == condition_data;
    else if(relation == LESS_EQUAL) return item_data <= condition_data;
    else if(relation == LESS) return item_data < condition_data;
}

bool RecordManager::SameType(short AttrType,short ItemType) const
{
    if(AttrType == ItemType || ((ItemType > 0) && (ItemType <= AttrType))) return 1;
    else return 0;
}
