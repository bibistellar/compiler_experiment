/*
    将中间代码转换成RISC-V汇编代码
    加载指令: lw rd,offset(rs)
    存储指令: sw rs1,offset(rs2)
*/

#include <iostream>
#include <string>
#include <map>
#include <fstream>

#include "define.h"

struct middle_code{
    string operation;
    string arg1;
    string arg2;
    string result;
};

Symbol_Table* cur_table;

struct middle_code get_one_middle_code(char* buf){
    int i = 1;
    struct middle_code code;
    while(buf[i]&&buf[i]!=','){
        code.operation+=buf[i++];
    }
    i++;
    while(buf[i]&&buf[i]!=','){
        code.arg1+=buf[i++];
    }
    i++;
    while(buf[i]&&buf[i]!=','){
        code.arg2+=buf[i++];
    }
    i++;
    while(buf[i]&&buf[i]!=','){
        code.result+=buf[i++];
    }
    i++;
    return code;
}


void data_segment_create(Symbol_Table* table){
    map<int, string>::iterator iter;
    attr attr_temp;
    if(!table){
        return;
    }
    for(iter =table->addr_table.begin(); iter != table->addr_table.end(); iter++) {
        int addr = iter->first;
        string name = iter->second;
        attr_temp = table->symbol_table.at(name);
        //分配一个地址存放变量
        if(name[0]!='#'){
            f_asm << name << ": ";
        }
        if(attr_temp.data_type == "int"){
            f_asm << ".4byte "<< 0 <<endl;
        }
    }
    data_segment_create(table->child);
    data_segment_create(table->brother);
}

Symbol_Table* search_symbol_table(Symbol_Table* table,string name){
    Symbol_Table* p;
    if(!table){
        return NULL;
    }
    if(table->name == name){
        return table;
    }
    //找兄弟节点
    if(table->brother){
        p  =  search_symbol_table(table->brother,name);
    }
    if(!p){
        return p;
    }

    //兄弟节点没找到,找子节点
    if(table->child){
        p = search_symbol_table(table->child,name);
    }
    if(!p){
        return p;
    }

    return NULL;
}

int if_mem_addr(string arg){
    string addr;
    int addr_int;
    if(arg[0] == '['){
        for(int i=1;arg[i]!=']';i++){
            addr[i-1] = arg[i];
        }
        return stoi(addr);
    }
    return -1;
}

int asm_code_creator(){
    char buf[512];
    struct middle_code code;
    int offset = 0;
    cur_table = symbol_table;
    f_three_address_code.open(three_address_code_path,ios::in);
    f_asm.open(asm_path,ios::out);
    
    memset(buf,0,sizeof(buf));

    //数据段
    f_asm << ".data" << endl;
    data_segment_create(symbol_table);
    f_asm << endl;

    //代码段
    f_asm << ".text" << endl;
    while(f_three_address_code.getline(buf,sizeof(buf))){
        code = get_one_middle_code(buf);
        if(code.operation == "Function"){
            f_asm << code.arg2 << ":" << endl;
            cur_table = search_symbol_table(symbol_table,code.arg2);
        }
        else if(code.operation == "+"){
            int addr1;
            int addr2;
            int result_addr;
            addr1 = if_mem_addr(code.arg1);
            addr2 = if_mem_addr(code.arg2);
            if(addr1 >= 0){
                f_asm << "lw   " << "x5," <<addr1 << "(x0)"<<endl; 
            }
            else{
                f_asm << "addi " << "x5,x0," << code.arg1 << endl;
            }
            if(addr2 >= 0){
                f_asm << "lw   " << "x6," <<addr2 << "(x0)"<<endl; 
            }
            else{
                f_asm << "addi " << "x6,x0," << code.arg2 << endl;
            }
            //cout << "add ";
            //两个数相加
            f_asm     << "add  "  << "x7,x5,x6" << endl;
            result_addr = if_mem_addr(code.result);
            f_asm     << "sw   "   <<"x7,"<<result_addr<<"(x0)"<<endl;
        }
        else if(code.operation == "-"){
            int addr1;
            int addr2;
            int result_addr;
            addr1 = if_mem_addr(code.arg1);
            addr2 = if_mem_addr(code.arg2);
            if(addr1 >= 0){
                f_asm << "lw   " << "x5," <<addr1 << "(x0)"<<endl; 
            }
            else{
                f_asm << "addi " << "x5,x0," << code.arg1 << endl;
            }
            if(addr2 >= 0){
                f_asm << "lw   " << "x6," <<addr2 << "(x0)"<<endl; 
            }
            else{
                f_asm << "addi " << "x6,x0," << code.arg2 << endl;
            }
            //cout << "add ";
            //两个数相加
            f_asm     << "sub  "  << "x7,x5,x6" << endl;
            result_addr = if_mem_addr(code.result);
            f_asm     << "sw   "   <<"x7,"<<result_addr<<"(x0)"<<endl;

        }
        else if(code.operation == "*"){
            int addr1;
            int addr2;
            int result_addr;
            addr1 = if_mem_addr(code.arg1);
            addr2 = if_mem_addr(code.arg2);
            if(addr1 >= 0){
                f_asm << "lw   " << "x5," <<addr1 << "(x0)"<<endl; 
            }
            else{
                f_asm << "addi " << "x5,x0," << code.arg1 << endl;
            }
            if(addr2 >= 0){
                f_asm << "lw   " << "x6," <<addr2 << "(x0)"<<endl; 
            }
            else{
                f_asm << "addi " << "x6,x0," << code.arg2 << endl;
            }
            //cout << "add ";
            //两个数相加
            f_asm     << "mul  "  << "x7,x5,x6" << endl;
            result_addr = if_mem_addr(code.result);
            f_asm     << "sw   "   <<"x7,"<<result_addr<<"(x0)"<<endl;
        }
        else if(code.operation == "-"){
            int addr1;
            int addr2;
            int result_addr;
            addr1 = if_mem_addr(code.arg1);
            addr2 = if_mem_addr(code.arg2);
            if(addr1 >= 0){
                f_asm << "lw   " << "x5," <<addr1 << "(x0)"<<endl; 
            }
            else{
                f_asm << "addi " << "x5,x0," << code.arg1 << endl;
            }
            if(addr2 >= 0){
                f_asm << "lw   " << "x6," <<addr2 << "(x0)"<<endl; 
            }
            else{
                f_asm << "addi " << "x6,x0," << code.arg2 << endl;
            }
            //cout << "add ";
            //两个数相加
            f_asm     << "div  "  << "x7,x5,x6" << endl;
            result_addr = if_mem_addr(code.result);
            f_asm     << "sw   "   <<"x7,"<<result_addr<<"(x0)"<<endl;
        }
        else if(code.operation == "assign"){
            int addr;
            int result_addr;
            addr = if_mem_addr(code.arg1);
            if(addr>=0){
                f_asm << "lw   " << "x5," <<addr<< "(x0)"<<endl; 
            }
            else{
                f_asm << "addi " << "x5,x0," << code.arg1 << endl;
            }
            f_asm     << "add  "  << "x6,x5,x0" << endl;
            result_addr = if_mem_addr(code.result);
            f_asm     << "sw   "   <<"x6,"<<result_addr<<"(x0)"<<endl;
        }
    }
    f_asm.close();
    f_three_address_code.close();
    return 0;
}