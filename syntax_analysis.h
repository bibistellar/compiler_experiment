#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <stack>
#include <cstring>
#include "define.h"

using namespace std;

//状态分析栈节点
typedef struct state_and_word
{
    int state;
    int syntax_code;
    int lex_code;
} SC;

class ActTable
{
private:
    typedef struct production
    {
        int left_type_code;
        int right_len;
        string production;
    } PROD;
    map <int,PROD> prod;//根据表达式查找编号
    //PROD prod[MAXPRODUCTION];

    typedef struct action_table
    {
        char action;
        int state;
        int PNO;
    } ACT;
    ACT action[MAXSIZE][MAXSIZE];

    map<string, int> VT; //终结符和非终结符编号
    map<int, string> code_VT;//允许根据编号找到终结和非终结字符

public:
    int create(fstream &f_LR)
    {
        char buf[1024];
        char temp[1024];
        int i;
        int j;
        int k;
        int VT_NO = 1;
        f_LR.getline(buf, sizeof(buf)); //c++ getline不会读入回车符号'\n',但是会读入'\r'
        f_LR.getline(buf, sizeof(buf)); //读入终结符与非终结符

        //给终结符和非终结符编号
        for (i = 0; buf[i]; i++)
        {
            if (buf[i] == ',' || buf[i] == '\r')
            {
                temp[j] = '\0';
                if (j != 0)
                {
                    //将V和T存入两个map中，由于VT的名字与编号一一对应，两个map支持通过编号查找VT的字符串名称或通过名称查找对应的编号
                    string VT_name = temp;
                    code_VT.insert(pair<int, string>(VT_NO, temp));
                    VT.insert(pair<string, int>(temp, VT_NO++));
                }
                j = 0;
            }
            else
            {
                temp[j++] = buf[i];
            }
        }

        int prod_no = 0;//产生式编号
        //给产生式编号，建立act表
        while (f_LR.getline(buf, sizeof(buf)))
        {   

            //获取当前的行数x
            k = 0;
            i = 0;
            while(buf[i]!=','){
                temp[k++] = buf[i++];
            }
            temp[k] = '\0';
            int x = atoi(temp);

            j = 0;//计算逗号数量，以此来决定得到的动作应该放哪一个格子
            for (; buf[i]; i++)
            {
                if (buf[i] != ',' && buf[i] != '\r'){
                    switch(buf[i]){
                        case 's':{
                            action[x][j].action = 's';
                            while(buf[i++] != '?');//快进到状态的数字编号
                            k = 0;//此时i指向第一个数字

                            //将数字字符串截取出来放入temp中
                            while(buf[i]!=','){
                                temp[k++] = buf[i++];
                            }

                            //状态temp填入表中
                            temp[k] = '\0';
                            action[x][j].state = atoi(temp);

                            j ++; //扫到了一个逗号，加上
                            
                            break;
                        }
                        case 'r':{
                            //动作为r时需要获取产生式，因此将相应的产生式编号存入action表的PNO项
                            //使用集合prod来存储产生式
                            k = 0;
                            PROD prod_temp;
                            action[x][j].action = 'r';
                            while(buf[i++]!='?');

                            //获取产生式左部
                            while(buf[i]!='?'){
                                temp[k++] = buf[i++];
                            }
                            temp[k] = '\0';
                            prod_temp.left_type_code = VT.at(temp);
                            
                            //跳过?->
                            i = i+3;
                            k = 0;

                            //获取产生式右部字符串表达式
                            while(buf[i]!=','){
                                temp[k++] = buf[i++];
                            }
                            temp[k] = '\0';
                            prod_temp.production = temp;
                            
                            //获取表达式右部终结符和非终结符的数量
                            prod_temp.right_len = 0;
                            for(k=0;temp[k];k++){
                                if(temp[k] == '?'){
                                    prod_temp.right_len++;
                                }
                            }

                            //判断产生式是否已在集合中，并给r动作记录产生式编号
                            int number = prod_has_in(prod_temp);
                            if(number == -1){
                                prod.insert(pair<int,PROD>(prod_no,prod_temp));
                                action[x][j].state = -1 ;//r动作不需要存储下一个状态
                                action[x][j].PNO = prod_no++;
                            }
                            else{
                                action[x][j].state = -1 ;
                                action[x][j].PNO = number;
                            }

                            j++;
                            break;
                        }
                        case 'a':{   
                            action[x][j].action = 'a';
                            while(buf[i++]!=',');
                            action[x][j].state = -1 ;

                            j +=2;
                            break;
                        }
                        //goto表的建立
                        default:{
                            k = 0;
                            action[x][j].action = 'n';
                            while(buf[i]!= ',' && buf[i] != '\r'){
                                temp[k++] = buf[i++];
                            }
                            temp[k] = '\0';

                            action[x][j].state = atoi(temp);

                            if(buf && buf[i] == ','){
                                j++;
                            }
                            break;
                        }
                    }  
                }
                else{
                    j++;
                }
            }
        }
        return 0;
    }
    //根据输入的字符串查找终结符/非终结符在表中的编号
    int code_of(const char* name){
        return VT.at(name);
    }
    //将词法分析器的成分编号转化成相应终结符在表中的编号
    int code_transfer(int code_lex){
        switch(code_lex){
            case(ID):{
                return VT.at("ID");
                break;
            }
            case(VALUE_INT):{
                return VT.at("VALUE_INT");
                break;
            }
            case(VALUE_FLOAT):{
                return VT.at("VALUE_FLOAT");
                break;
            }
            case(VALUE_STRING):{
                return VT.at("VALUE_STRING");
                break;
            }
            case(KEY_INT):{
                return VT.at("KEY_INT");
                break;
            }
            case(KEY_FLOAT):{
                return VT.at("KEY_FLOAT");
                break;
            }
            case(KEY_CHAR):{
                return VT.at("KEY_CHAR");
                break;
            }
            case(KEY_IF):{
                return VT.at("KEY_IF");
                break;
            }
            case(KEY_ELSE):{
                return VT.at("KEY_ELSE");
                break;
            }
            case(KEY_RETURN):{
                return VT.at("KEY_RETURN");
                break;
            }
            case(OP_PLUS):{
                return VT.at("+");
                break;
            }
            case(OP_ASSIGN):{
                return VT.at("=");
                break;
            }
            case(OP_MINUS):{
                return VT.at("-");
                break;
            }
            case(OP_DIVISION):{
                return VT.at("/");
                break;
            }
            case(OP_MULT):{
                return VT.at("*");
                break;
            }
            case(SEP_L_PARENTHESES):{
                return VT.at("(");
                break;
            }
            case(SEP_R_PARENTHESES):{
                return VT.at(")");
                break;
            }
            case(SEP_L_BRACES):{
                return VT.at("{");
                break;
            }
            case(SEP_R_BRACES):{
                return VT.at("}");
                break;
            }
            case(SEP_SEMICOLON):{
                return VT.at("#END");
                break;
            }
            case(SEP_COMMA):{
                return VT.at("SEP_COMMA");
                break;
            }
            case(FILE_END):{
                return VT.at("$");
            }
            default:{
                break;
            }
        }
        return 0;
    }
    int next_state(int x,int y){
        return action[x][y].state;
    }
    int next_action(int x,int y){
        return action[x][y].action;
    }
    int pro_right_len(int x,int y){
        return prod.at(action[x][y].PNO).right_len;
    }
    int pro_left_code(int x,int y){
        return prod.at(action[x][y].PNO).left_type_code;
    }
    //将动作表中被指定的格子中的产生式写入文件中,同时返回该产生式的字符串
    string write_produce(fstream& f_produce,int x,int y){
        string production;
        f_produce<<(code_VT.at(prod[action[x][y].PNO].left_type_code));
        production = code_VT.at(prod[action[x][y].PNO].left_type_code) + " ->";
        f_produce << " ->";
        for(int i = 0;i < prod[action[x][y].PNO].production.length();i++){
            if(prod[action[x][y].PNO].production[i] == '?'){
                f_produce << ' ';
                production = production + ' ';
            }
            else{
                f_produce << prod[action[x][y].PNO].production[i];
                production = production + prod[action[x][y].PNO].production[i];
            }
        }
        f_produce << endl;
        return production;
    }
    int prod_has_in(PROD prod_temp){
        for(map<int,PROD>::iterator i = prod.begin();i != prod.end();i++){
            if(i->second.left_type_code == prod_temp.left_type_code && i->second.production == prod_temp.production){
                return i->first;
            }        
        }
        return -1;
    }//判断产生式是否存在于集合当中,存在则返回关键字编号
};