#include "RecordManager.h"
#include <iostream>
#include <cstring>
#include <ctime>

BufferManager buffer_manager("testrecord");

using namespace std;
int main()
{
    RecordManager rmanager;
    // item temp;
    // temp.i_data = 10;
    // temp.type = 0;
    // Tuple tuple1,tuple2;
    // tuple1.AddItem(temp);
    Tuple tuple0,tuple1,tuple2;
    tuple0.AddItem("FirstName");
    tuple0.AddItem("LastName");
    tuple0.AddItem("Robin");
    tuple0.AddItem("Black");        //FirstName|LastName|Robin|Black
    tuple1.AddItem("FirstName");
    tuple1.AddItem("AAAAName");
    tuple1.AddItem("Alice");
    tuple1.AddItem("Brown");        //FirstName|
    tuple2.AddItem("BBBBBName");
    tuple2.AddItem("CCCCName");
    tuple2.AddItem("Touch");
    tuple2.AddItem("Zhang");
    std::vector<Condition> ConditionList;
    Condition Condition1;
    Condition1.item.str_data.assign("LastName");
    ConditionList.push_back(Condition1);
    Condition1.item.str_data.assign("CCCCName");
    ConditionList.push_back(Condition1);
    Tuple tmp;
    // tmp.AddItem()
    rmanager.CreateTableAllFile("testcreate");
    int x = 0;
    while(1)
    {
        cin >> x;
        if (x == 1) rmanager.DeleteRecord("testcreate");
        else if(x == 2) rmanager.DeleteRecord("testcreate",ConditionList);
        else if (x == 3) 
        {
            string temp = "";
            int record_num = rmanager.SelectRecord("testcreate",temp);
            cout << temp;
            cout << record_num << endl;
        }
        else if (x == 0) 
        {
            rmanager.InsertRecord("testcreate",tuple0);
            rmanager.InsertRecord("testcreate",tuple1);
            rmanager.InsertRecord("testcreate",tuple2);
        }
        else if(x == 4)
        {
            string temp = "";
            int record_num = rmanager.SelectRecord("testcreate",ConditionList,temp);
            cout << temp;
            cout <<record_num << endl;
        }
        else if (x == 5) break;
    }
    return 0;
    // getchar();
    // rmanager.DropTableAllFile("testcreate");
}