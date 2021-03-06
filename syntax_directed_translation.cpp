#include "syntax_directed_translation.h"
#include "symbol_table.h"
#include "define.h"

#include <iostream>
#include <string>
#include <map>
#include <fstream>

using namespace std;

//三地址码序列号
int three_address_order = 0;


//临时变量名字
string TempValueName(){
    static int i = 0;
    string name = "#t";
    i = i+1;
    name = name + to_string(i);
    return name;
}

//输出三地址码
void print_three_address_code(string first,string second,string third,string fourth){
    f_three_address_code << "("<<first<<","<<second<<","<<third<<","<<fourth<<")"<<endl;
}

//三地址码生成并存入缓冲区
void gencode(string first,string second,string third,string fourth){
    f_three_address_code << "("<<first<<","<<second<<","<<third<<","<<fourth<<")"<<endl;
}

void SDT(string production,stack<SYNTAXNODE> & syntax_stack,int right_length){
    //是否执行了翻译动作
    int translate = 0;
    //下一个数据存放的起始地址
    //static uint64_t offset = 0;
    
    //分析出函数开头
    if(production == "FUNC_START -> TYPE ID ( ) {"){
        SYNTAXNODE TYPE;
        SYNTAXNODE ID;
        //申请新节点
        SYNTAXNODE FUNCT_START;

        syntax_stack.pop();
        syntax_stack.pop();
        syntax_stack.pop();
        ID = syntax_stack.top();
        syntax_stack.pop();
        TYPE = syntax_stack.top();
        syntax_stack.pop();

        FUNCT_START.name = ID.name;
        FUNCT_START.type = TYPE.type;
        

        Symbol_Table* symbol_table_new = new Symbol_Table;
        Symbol_Table* p;
        symbol_table_new->parent = symbol_table;
        symbol_table_new->child = NULL;
        symbol_table_new->name = FUNCT_START.name;
        symbol_table_new->offset = 0;

        p = symbol_table->child;
        if(p){
                while(p->brother){
                p = p->brother;
            }
            p->brother = symbol_table_new;
            symbol_table = symbol_table_new;
        }
        else{
            symbol_table->child = symbol_table_new;
            symbol_table = symbol_table_new;
        }

        //输出标号
        gencode("Function","Label",symbol_table->name," ");

        syntax_stack.push(FUNCT_START);

        translate = 1;
    }
    //函数结尾
    if(production == "DEF_FUNC -> FUNC_START MS }"){
        SYNTAXNODE DEF_FUNC;
        syntax_stack.pop();
        syntax_stack.pop();
        syntax_stack.pop();
        symbol_table = symbol_table->parent;
        syntax_stack.push(DEF_FUNC);
        translate = 1;
    }

    if(production == "TYPE -> KEY_INT"){
        SYNTAXNODE KEY_INT = syntax_stack.top();
        syntax_stack.pop();
        //申请TYPE的语法分析节点并初始化
        SYNTAXNODE TYPE;
        TYPE.init("TYPE");

        //执行翻译
        TYPE.type = "int";
        TYPE.nextattr.insert(pair<string,string>("width","4"));

        syntax_stack.push(TYPE);

        //free_SYNTAXNODE(KEY_INT);
        translate = 1;
    }

    if(production == "DEF_VALUE -> TYPE ID"){
        SYNTAXNODE DEF_VALUE;
        DEF_VALUE.init("DEF_VALUE");
        SYNTAXNODE ID = syntax_stack.top();
        syntax_stack.pop();
        SYNTAXNODE TYPE = syntax_stack.top();
        syntax_stack.pop();

        //查找并获取所需属性
        string type = TYPE.type;
        string name = ID.name;
        int width = stoi(TYPE.nextattr.at("width"));

        //计算新属性
        symbol_table->enter(name,type,symbol_table->offset);
        symbol_table->offset += width;

        //翻译结束新规约节点入栈
        syntax_stack.push(DEF_VALUE);

        //请空内存
        //free_SYNTAXNODE(ID);
        //free_SYNTAXNODE(TYPE);
        translate = 1;
    }

    if(production == "S -> ID = Exp"){
        //获取产生式左部节点
        SYNTAXNODE S;
        S.init("S");
        SYNTAXNODE Exp = syntax_stack.top();
        syntax_stack.pop();
        SYNTAXNODE Assign = syntax_stack.top();
        syntax_stack.pop();
        SYNTAXNODE ID = syntax_stack.top();
        syntax_stack.pop();

        //寻找所需属性

        //执行翻译动作
        uint64_t id_addr;
        if((id_addr = symbol_table->lookup(ID.name))<0){
            cout<<"error:"<<ID.name<<" is a undefined symbol!\n";
            exit(1);
        }
        //Exp的地址是-1,无临时变量地址存储Exp
        if(Exp.addr == -1){
            gencode("Assign",to_string(Exp.val)," ","["+to_string(id_addr)+"]");
        } 
        else{
            gencode("Assign","["+to_string(Exp.addr)+"]"," ","["+to_string(id_addr)+"]");
        }

        //新节点入栈;
        syntax_stack.push(S);
        translate = 1;
    }

    if(production == "Exp -> Exp OP_LOW A"){
        //获取产生式左部节点
        SYNTAXNODE Exp_new;
        SYNTAXNODE A = syntax_stack.top();
        syntax_stack.pop();
        SYNTAXNODE OP_LOW = syntax_stack.top();
        syntax_stack.pop();
        SYNTAXNODE Exp = syntax_stack.top();
        syntax_stack.pop();

        //寻找所需属性并执行翻译动作
        string op = OP_LOW.name;
        int width;

        Exp_new.init(TempValueName());
        Exp_new.addr = symbol_table->offset;
        Exp_new.type = A.type;

        if(Exp.type == "int"){
            width = 4;
        }

        symbol_table->enter(Exp_new.name,Exp_new.type,symbol_table->offset);
        symbol_table->offset += width;

        string Exp_addr;
        string A_addr;
        if(Exp.addr == -1){
            Exp_addr = to_string(Exp.val);
        }
        else{
            Exp_addr = "[" + to_string(Exp.addr)+"]";
        }
        if(A.addr == -1){
            A_addr = to_string(A.val);
        }
        else{
            A_addr = "[" + to_string(A.addr)+"]";
        }
        gencode(op,Exp_addr,A_addr,"["+to_string(Exp_new.addr)+"]");

        //新节点入栈;
        syntax_stack.push(Exp_new);
        translate = 1;
    }

    if(production == "Exp -> A"){
        //获取产生式左部节点
        SYNTAXNODE Exp;
        Exp.init("Exp");
        SYNTAXNODE A = syntax_stack.top();
        syntax_stack.pop();


        //寻找所需属性

        //执行翻译动作
        Exp.val = A.val;
        Exp.addr = A.addr;
        Exp.type = A.type;

        //新节点入栈;
        syntax_stack.push(Exp);
        translate = 1;
    }

    if(production == "A -> B"){
        //获取产生式左部节点
        SYNTAXNODE A;
        A.init("A");
        SYNTAXNODE B = syntax_stack.top();
        syntax_stack.pop();

        //寻找所需属性

        //执行翻译动作
        A.val = B.val;
        A.addr = B.addr;
        A.type = B.type;

        //新节点入栈;
        syntax_stack.push(A);
        translate = 1;
    }

    if(production == "A -> A OP_HIGH B"){
        SYNTAXNODE A1;
        A1.init(TempValueName());
        SYNTAXNODE B = syntax_stack.top();
        syntax_stack.pop();
        SYNTAXNODE OP_HIGH = syntax_stack.top();
        syntax_stack.pop();
        SYNTAXNODE A2 = syntax_stack.top();
        syntax_stack.pop();

        string op = OP_HIGH.name;
        int width;
        if(B.type == "int"){
            width = 4;
        }
        A1.addr = symbol_table->offset;
        A1.type = A2.type;
        symbol_table->enter(A1.name,A1.type,symbol_table->offset);
        symbol_table->offset+=width;


        string B_addr;
        string A2_addr;
        if(A2.addr == -1){
            A2_addr = to_string(A2.val);
        }
        else{
            A2_addr = "[" + to_string(A2.addr)+"]";
        }
        if(B.addr == -1){
            B_addr = to_string(B.val);
        }
        else{
            B_addr = "[" + to_string(B.addr)+"]";
        }
        
        gencode(op,A2_addr,B_addr,"["+to_string(A1.addr)+"]");

        syntax_stack.push(A1);

        translate = 1;
    }

    if(production == "B -> VAL_LOC"){
        //获取产生式左部节点
        SYNTAXNODE B;
        B.init("B");
        SYNTAXNODE VAL_LOC = syntax_stack.top();
        syntax_stack.pop();

        //寻找所需属性

        //执行翻译动作
        B.val = VAL_LOC.val;
        B.type = VAL_LOC.type;
        B.addr = -1;

        //新节点入栈;
        syntax_stack.push(B);
        translate = 1;
    }

    if(production == "B -> ( Exp )"){
        SYNTAXNODE B;
        B.init("B");
        syntax_stack.pop();
        SYNTAXNODE Exp = syntax_stack.top();
        syntax_stack.pop();
        syntax_stack.pop();
        B.val = Exp.val;
        B.addr = Exp.addr;
        B.type = Exp.type;

        syntax_stack.push(B);
        translate = 1;
    }

    if(production == "B -> ID"){
        SYNTAXNODE B;
        B.init("B");
        SYNTAXNODE ID = syntax_stack.top();
        syntax_stack.pop();

        if((B.addr = symbol_table->lookup(ID.name))<0){
            cout<<"error:"<<ID.name<<" is a undefined symbol!\n";
            exit(1);
        }

        attr ID_attr = symbol_table->get_symbol(ID.name);
        B.val = ID.val;
        B.type = ID_attr.data_type;

        syntax_stack.push(B);
        translate = 1;
    }   

    if(production == "VAL_LOC -> VALUE_INT"){
        //获取产生式左部节点
        SYNTAXNODE VAL_LOC;
        VAL_LOC.init("VAL_LOC");
        SYNTAXNODE VALUE_INT = syntax_stack.top();
        syntax_stack.pop();

        //寻找所需属性

        //执行翻译动作
        VAL_LOC.val = stoi(VALUE_INT.name);
        VAL_LOC.type = "int";


        //新节点入栈;
        syntax_stack.push(VAL_LOC);
        translate = 1;
    }

    if(production == "OP_LOW -> +"){
        //获取产生式左部节点
        SYNTAXNODE OP_LOW;
        OP_LOW.init("+");
        SYNTAXNODE OP_ADD = syntax_stack.top();
        syntax_stack.pop();

        //寻找所需属性

        //执行翻译动作
        

        //新节点入栈;
        syntax_stack.push(OP_LOW);
        translate = 1;
    }

    if(production == "OP_LOW -> -"){
        //获取产生式左部节点
        SYNTAXNODE OP_LOW;
        OP_LOW.init("-");
        //SYNTAXNODE OP_ADD = syntax_stack.top();
        syntax_stack.pop();

        //寻找所需属性

        //执行翻译动作
        

        //新节点入栈;
        syntax_stack.push(OP_LOW);
        translate = 1;
    }

    if(production == "OP_HIGH -> *"){
        //获取产生式左部节点
        SYNTAXNODE OP_HIGH;
        OP_HIGH.init("*");
        //SYNTAXNODE OP_ADD = syntax_stack.top();
        syntax_stack.pop();

        //寻找所需属性

        //执行翻译动作
        

        //新节点入栈;
        syntax_stack.push(OP_HIGH);
        translate = 1;
    }

    if(production == "OP_HIGH -> /"){
        //获取产生式左部节点
        SYNTAXNODE OP_HIGH;
        OP_HIGH.init("/");
        //SYNTAXNODE OP_ADD = syntax_stack.top();
        syntax_stack.pop();

        //寻找所需属性

        //执行翻译动作
        

        //新节点入栈;
        syntax_stack.push(OP_HIGH);
        translate = 1;
    }


    if(!translate){
        for(int i=0;i<right_length;i++){
            //free_SYNTAXNODE(syntax_stack.top());
            syntax_stack.pop();
        }
        SYNTAXNODE new_node;
        new_node.init("new");
        syntax_stack.push(new_node);
    }
}