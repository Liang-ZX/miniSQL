#ifndef TUPLE_H
#define TUPLE_H
#include <string.h>
#include <string>
#include <iostream>
#include <vector>

#define RECORD_SEPARATOR '$'
#define ITEM_SEPARATOR '|'

typedef enum{
    STRING,
    INT,
    FLOAT
}TupleDataType;

typedef enum{
    GREATER,
    GREATER_EQUAL,
    EQUAL,
    LESS_EQUAL,
    LESS,
    NOT_EQUAL
}Relation;
/**
 * str_data,i_data_f_data are the three possible type
 * type = 1~255 string
 * type = 0 int 
 * type = -1 float
 */
struct Item{
    std::string str_data;
    int i_data;
    float f_data;
    short type;
};

struct Condition{
    std::string AttrName;
    // short AttrID;
    Item item;
    Relation relation;
};

class Tuple
{
private:
    std::vector<Item> ItemList;
public:
    std::string TupletoString() const;
    void AddItem(const Item &item);
    void AddItem(const std::string &str);
    void AddItem(const int &num);
    void AddItem(const double &num);
    //'abcd',123,12.4,'xa'
    Tuple(const std::string &str);
    Tuple(/* args */){}
    ~Tuple(){}
};

// Tuple::Tuple(/* args */)
// {
// }

// Tuple::~Tuple()
// {
// }

#endif