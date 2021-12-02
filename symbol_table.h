#ifndef DEFINE_SYMBOLTABLE
#define DEFINE_SYMBOLTABLE

#include <iostream>
#include <string>
#include <map>
#include <fstream>
using namespace std;

//符号表
//符号表结构参考P310图8.3
typedef struct attr_node{
    string symbol_type;
    string data_type;
    uint64_t addr;
    map<string,string> additional_attribute;
}attr;

class Symbol_Table
{
public:
    //符号表的名字
    string name;
    //符号表所属所属作用域类型
    string type;
    //函数专用
    string return_type;
    //该作用域内的符号,不包括临时变量
    map<string, attr> symbol_table;
    //符号按地址进行排序
    map<int,string> addr_table;
    


    //该符号表的子符号表
    Symbol_Table* child;
    //该符号表的父符号表
    Symbol_Table* parent;
    //该符号表的右兄弟节点
    Symbol_Table* brother;
    //下一个符号的偏移量
    uint64_t offset = 0;

    void enter(string symbol_name,string data_type,uint64_t offset){
        attr new_symbol_attr;
        new_symbol_attr.symbol_type = "value";
        new_symbol_attr.data_type = data_type;
        new_symbol_attr.addr = offset;
        symbol_table.insert(pair<string,attr>(symbol_name,new_symbol_attr));
        addr_table.insert(pair<int,string>(offset,symbol_name));
    }

    attr get_symbol(string symbol_name){
        return symbol_table.at(symbol_name);
    }

    void update_symbol(string symbol_name,attr new_attr){
        symbol_table[symbol_name] = new_attr;
    }

    uint64_t lookup(string symbol_name){
        if(symbol_table.count(symbol_name)){
            return symbol_table.at(symbol_name).addr;
        }
        return -1;
    }

    void print(fstream& f_symbol_table){
        map<string, attr>::iterator symbol;
        for (symbol = symbol_table.begin(); symbol != symbol_table.end(); ++symbol)
        {
            f_symbol_table << "(" << symbol->first << "," << symbol->second.symbol_type <<"," << symbol->second.data_type<<","<<symbol->second.addr<<")"<<endl;
        }
    }
};

//全局符号表
Symbol_Table* symbol_table;

#endif