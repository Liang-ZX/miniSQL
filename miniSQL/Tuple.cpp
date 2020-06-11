#include "Tuple.h"
// enum 
void Tuple::AddItem(const Item &item)
{
    ItemList.push_back(item);
    return;
}

void Tuple::AddItem(const std::string &str)
{
    Item tp;
    tp.type = str.length();
    tp.str_data.assign(str);
    ItemList.push_back(tp);
    return;
}

void Tuple::AddItem(const int &num)
{
    Item tp;
    tp.type = 0;
    tp.i_data = num;
    ItemList.push_back(tp);
    return;
}

void Tuple::AddItem(const double &num)
{
    Item tp;
    tp.type = -1;
    tp.f_data = num;
    ItemList.push_back(tp);
    return;
}

// Tuple::Tuple(const std::string &str)
// {
//     int pos0 = 0;
//     int pos1 = 0; 
//     for(pos0; pos1 != -1;pos0 = pos1 + 1,pos1 = str.find_first_of(',',pos0))
//     {
//         int sublen = pos1 - pos0;
//         // if(sublen == 0) 
//         int tps = str.find_first_of("'",pos0,sublen);
//         if(tps == -1) //not string 
//         {
//             int tpf = str.find_first_of(".",pos0,sublen);
//             if(tpf == -1) //not float
//             {
//                 item temp;
//                 temp.i_data = atoi(str.substr(pos0,sublen).c_str());
//             }
//         }
//     }
// }
std::string Tuple::TupletoString() const
{
    int len = ItemList.size();
    std::string record = "|";
    for(int i = 0;i < len;i++)
    {
        if(ItemList[i].type == 0) record.append(std::to_string(ItemList[i].i_data));
        else if(ItemList[i].type == -1) record.append(std::to_string(ItemList[i].f_data));
        else record.append(ItemList[i].str_data);
        record.push_back(ITEM_SEPARATOR);
    }
    record.push_back(RECORD_SEPARATOR);
    return record;
}

const std::vector<Item> &Tuple::GetItemList() const
{
    return ItemList;
}