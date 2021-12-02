#include "define.h"
#include "syntax_analysis.h"
#include "symbol_table.h"
#include "syntax_directed_translation.cpp"

using namespace std;

int syntax_analysis()
{
    stack<SC> lr_stack;
    stack<SYNTAXNODE> syntax_stack;
    SC cur;
    char buf[1024];
    char token_name[1024];
    char token_lexcode[1024];
    int i,j,k,syntax_code,x,y,cur_state;
    int end = 0;
    int readline = 1;
    f_LR.open(LR_path,ios::in);
    f_token.open(token_path,ios::in);
    f_produce.open(produce_path,ios::out);
    f_symbol_table.open(symbol_table_path,ios::out);
    f_three_address_code.open(three_address_code_path,ios::out);
    ActTable ActTable;

    //创建分析表
    ActTable.create(f_LR);

    //初始化分析栈
    cur.state = 0;
    cur.syntax_code = ActTable.code_of("$");
    cur.lex_code = 0;

    lr_stack.push(cur);

    //建立全局符号表(所有符号表的入口)
    symbol_table = new Symbol_Table;
    symbol_table->type = "GLOBAL";
    //开始归约
    while(!end){
        k = 0;
        j = 0;
        if(readline){
            f_token.getline(buf,sizeof(buf));
        }

        //得到标识符的词法分析编号temp
        for(i = 1;buf[i] != ',';i++){
            token_lexcode[k++] = buf[i];
        }
        //得到标识符的名字name
        for(i = i+1;buf[i] !=')';i++){
            token_name[j++] = buf[i];
        }
        token_lexcode[k] = '\0';
        if(strcmp(token_lexcode,"30") == 0){
            token_name[j++] = ')';
        }
        token_name[j] = '\0';
        //temp转换为语法分析编号
        syntax_code = ActTable.code_transfer(atoi(token_lexcode));

        //状态转移
        cur_state = lr_stack.top().state;
        switch(ActTable.next_action(cur_state , syntax_code)){
            case('r'):{
                int production_right_length = ActTable.pro_right_len(cur_state , syntax_code);
                string production;

                //规约一条语句并输出到文件中
                production = ActTable.write_produce(f_produce,cur_state,syntax_code);
                //根据规约语句执行翻译动作
                SDT(production,syntax_stack,production_right_length);

                //产生式右部出栈
                for(i = 0;i < production_right_length;i++){
                    lr_stack.pop();
                }

                //产生式左部入栈
                cur.syntax_code = ActTable.pro_left_code(cur_state , syntax_code);
                cur.lex_code = -1;
                cur.state = ActTable.next_state(lr_stack.top().state,cur.syntax_code);
                lr_stack.push(cur);



                //本次处理后不读入新行
                readline = 0;
                break;
            }
            case('s'):{
                SYNTAXNODE syntax_attr;
                cur.state = ActTable.next_state(cur_state , syntax_code);
                cur.syntax_code = syntax_code;
                cur.lex_code = atoi(token_lexcode);
                lr_stack.push(cur);
                //语法属性入栈,在这里是终结符的名字,如"+","int","a","123"等
                syntax_attr.init(token_name);
                syntax_stack.push(syntax_attr);
                readline = 1;
                break;
            }
            case('a'):{
                f_produce << "Accept!" << endl;
                end = 1;
                break;
            }
            default:{
                f_produce << "Error!" << endl;
                end = 1;
                break;
            }
        }
    }
    //symbol_table->print(f_symbol_table);
    f_token.close();
    f_produce.close();
    f_LR.close();
    f_three_address_code.close();
    f_symbol_table.close();
    return 0;
}
