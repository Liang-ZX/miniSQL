#include "RecordManager.h"

int RecordManager::CreateTableAllFile(BufferManager &buffer_manager,const std::string &TableName)
{
    cout << "Debug(Record):begin create " << TableName << "\n";
    if(buffer_manager.CheckTableExist(TableName))
    {
        cout << "Error(Record):Table with same name existed!\n";
        return 0;
    }
    // const std::string str = "";
    buffer_manager.writeFile("",TableName,0,0);
    buffer_manager.writeFile("",TableName,1,0);
    buffer_manager.writeFile("exist",TableName,2,0); //创建catalog会有问题
    cout << "Debug(Record):End create " << TableName << "\n";
    return 1;
}

int RecordManager::DropTableAllFile(BufferManager &buffer_manager,const std::string &TableName)
{
    if(buffer_manager.CheckTableExist(TableName) == false)
    {
        cout << "Error(Record):The table does not exist!\n";
        return 0;
    }
    cout << "Debug(Record):begin drop "<< TableName << "\n";
    std::string db_name = buffer_manager.GetDB_name();
    //TODO直接释放所有相关block
    std::string TableFile = "Data_files\\" + db_name + "\\table\\" + TableName + ".txt";
    std::string IndexFile = "Data_files\\" + db_name + "\\index\\" + TableName + ".txt";
    std::string CatalogFile = "Data_files\\" + db_name + "\\catalog\\" + TableName + ".txt";
    remove(TableFile.c_str());
    remove(IndexFile.c_str());
    remove(CatalogFile.c_str());
    cout << "Debug(Record):end drop " << TableName <<"\n";
    return 1;
}

int RecordManager::InsertRecord(BufferManager &buffer_manager,const std::string &TableName,const Tuple &tuple)
{
    clock_t begin_time = clock(),end_time = clock();
    if (buffer_manager.CheckTableExist(TableName) == false)
    {
        end_time = clock();
        cout << end_time - begin_time << "ms\n";
        cout << "Error(Record):The table does not exit.\n";
        return 0;
    }
    //TODO处理catalog,index的冲突
    std::string record;
    record.assign(tuple.TupletoString());
    cout << "Debug(Record):Begin insert record.\n";
    int record_length = record.length();
    for(int block_num = 0;block_num < MAX_BLOCK_NUM;block_num++)
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
        // buffer_manager.unsetBlockPin(TableName,0,block_num);
    }
    end_time = clock();
    cout << end_time - begin_time << "ms\n";
    cout << "Debug(Record):End insert record.\n";
    return 1;
}

int RecordManager::DeleteRecord(BufferManager &buffer_manager,const std::string &TableName)
{
    clock_t begin_time = clock(),end_time = clock();
    if(buffer_manager.CheckTableExist(TableName) == false)
    {
        end_time = clock();
        cout << end_time - begin_time << "ms\n";
        cout << "Debug(Record):No such table!\n";
        return -1;
    }
    cout << "Debug(Record):Start delete all of " << TableName << endl;
    int count = 0;
    for(int block_num = 0;block_num < MAX_BLOCK_NUM;block_num++)
    {
        std::string teststr = buffer_manager.readFile(TableName,0,block_num);
        if(teststr.length() == 0) break;    //若某块为空则结束（不严谨
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

int RecordManager::DeleteRecord(BufferManager &buffer_manager,const std::string &TableName,const std::vector<Condition> &ConditionList)
{
    clock_t begin_time = clock(), end_time = clock();
    if(buffer_manager.CheckTableExist(TableName) == false)
    {
        end_time = clock();
        cout << end_time - begin_time << "ms\n";
        cout << "Debug(Record):No such table!\n";
        return -1;
    }
    //TODO确认ConditionList中Attribute存在且数据类型无误
    cout << "Debug(Record):Start delete record fit conditions in " << TableName << endl;
    int count = 0;
    //TODO若有index则调用相关函数进行定位BLOCK

    for(int block_num = 0;block_num < MAX_BLOCK_NUM;block_num++)
    {
        cout << "Debug(Record):In delete conditionally, block:" << block_num << endl;
        std::string block_data = buffer_manager.readFile(TableName,0,block_num);
        if(block_data.length() == 0) break;  //若某块为空则结束（不严谨
        int block_length = block_data.length();
        int record_begin = 0;
        int record_end = block_data.find(RECORD_SEPARATOR,record_begin);
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

int RecordManager::SelectRecord(BufferManager &buffer_manager,const std::string &TableName,std::string &res)
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

int RecordManager::SelectRecord(BufferManager &buffer_manager,const std::string &TableName,const std::vector<Condition> &ConditionList,std::string &res)
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
