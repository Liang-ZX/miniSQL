#include "RecordManager.h"
using namespace std;
BufferManager buffer_manager("testrecord");
CatalogManager catalog_manager;
void ReadConditionList(vector<Condition>& ConditionList)
{
    while (1)
    {
        Condition tp;
        int relation;
        cout << "Input relation:";
        cin >> relation;
        if (relation > 9) return;
        if (relation == 0) tp.relation = GREATER;
        else if (relation == 1) tp.relation = GREATER_EQUAL;
        else if (relation == 2) tp.relation = EQUAL;
        else if (relation == 3) tp.relation = LESS_EQUAL;
        else if (relation == 4) tp.relation = LESS;
        else if (relation == 5) tp.relation = NOT_EQUAL;
        cout << "Input AttrName";
        cin >> tp.AttrName;
        cout << "Input type:";
        cin >> tp.item.type;
        cout << "Input data:";
        if (tp.item.type == -1) cin >> tp.item.f_data;
        else if (tp.item.type == 0) cin >> tp.item.i_data;
        else cin >> tp.item.str_data;
        ConditionList.push_back(tp);
    }
}
int main()
{
    cout << "RecordManager_test.cpp\n";
    RecordManager rmanager;
    Table TestTable;
    cin >> TestTable.name;
    cin >> TestTable.attriNum;
    TestTable.blockNum = 0;
    TestTable.totalLength = 0;
    for(int i = 0;i < TestTable.attriNum;i++)
    {
        cout << "Attribute" << i << endl;
        Attribute tp;
        cin >> tp.name >> tp.type;
        // TestTable.attributes[i].
        TestTable.attributes.push_back(tp);
    }
    if (catalog_manager.existTable(TestTable.name) == false)
    {
        rmanager.CreateTableFile(TestTable.name);
        catalog_manager.createTable(TestTable);
    }

    Tuple tuple0,tuple1,tuple2;
    tuple0.AddItem("FirstName");    tuple0.AddItem("LastName");    tuple0.AddItem("Robin");    tuple0.AddItem("Black");        //FirstName|LastName|Robin|Black
    // tuple1.AddItem(0.0);    
    tuple1.AddItem("FirstName");    tuple1.AddItem("AAAAName");    tuple1.AddItem("Alice");    tuple1.AddItem("Brown");        //FirstName|
    tuple2.AddItem("BBBBBName");    tuple2.AddItem("CCCCName");    tuple2.AddItem("Touch");    tuple2.AddItem("Zhang");
    std::vector<Condition> ConditionList;
    Condition Condition1;
    // Condition1.item.str_data.assign("LastName");
    // ConditionList.push_back(Condition1);
    // Condition1.item.str_data.assign("CCCCName");
    // ConditionList.push_back(Condition1);
    Tuple tmp;
    // tmp.AddItem()
    int x = 0;
    while(1)
    {
        cin >> x;
        if (x == 1)
        {
            string TableName;
            cout << "Input TableName to delete all record\n";
            cin >> TableName;
            rmanager.DeleteRecord(TableName);
        }
        else if (x == 2)
        {
            string TableName;
            cout << "Input TableName to delete conditionally\n";
            cin >> TableName;
            ConditionList.clear();
            ReadConditionList(ConditionList);
            rmanager.DeleteRecord(TableName, ConditionList);
        }
        else if (x == 3) 
        {
            string temp = "";
            string TableName;
            cout << "Input TableName to Select all records\n";
            cin >> TableName;
            int record_num = rmanager.SelectRecord(TableName,temp);
            cout << temp;
            cout << record_num << endl;
        }
        else if (x == 0) 
        {
            tuple0.ItemList.clear();
            for (int i = 0; i < TestTable.attriNum; i++)
            {
                cout << "Attr" << i << " " << TestTable.attributes[i].type << ":";
                if (TestTable.attributes[i].type == -1)
                {
                    float tp;
                    cin >> tp;
                    tuple0.AddItem(tp);
                }
                else if (TestTable.attributes[i].type == 0)
                {
                    int tp;
                    cin >> tp;
                    tuple0.AddItem(tp);
                }
                else
                {
                    string tp;
                    cin >> tp;
                    tuple0.AddItem(tp);
                }
            }
            string TableName;
            cout << "Input TableName to insert!\n";
            cin >> TableName;
            rmanager.InsertRecord(TableName, tuple0);
            //rmanager.InsertRecord(TestTable.name,tuple1);
            //rmanager.InsertRecord(TestTable.name,tuple2);
        }
        else if(x == 4)
        {
            string TableName;
            cout << "Input TableName to Select conditionally\n";
            cin >> TableName;
            ConditionList.clear();
            ReadConditionList(ConditionList);
            string res;
            int count = rmanager.SelectRecord(TableName, ConditionList,res);
            cout << count << endl << res;
        }
        else if (x == 5)
        {
            //test wrong attribute type
            rmanager.InsertRecord("alltype", tuple1);
        }
        else if (x == 6)
        {
            cout << "Table to create index:";
            string TableName;
            cin >> TableName;
            cout << "Index Name:";
            string IndexName;
            cin >> IndexName;
            cout << "Column:";
            int column;
            cin >> column;
            Index_Manager index_manager(TableName);
            short type = TestTable.attributes[column].type;
            if (type == -1) index_manager.Create_Index(IndexName, column, FLOAT);
            else if (type == 0)index_manager.Create_Index(IndexName, column, INT);
            else index_manager.Create_Index(IndexName, column, STRING, type);
        }
        else if (x == 7) break;
    }
    return 0;
}


/*datatest:
alltype
3
first -1
second 0
third  10



4
alltype
2
second
0
55
12

0
25.3
4
touch

6
firstindex
alltype
0
*/