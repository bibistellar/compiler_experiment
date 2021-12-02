#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <stack>
#include <cstring>

#include "global.h"

using namespace std;

int main(){
    int option;
    cout << "说明:1.请将源代码写入本程序目录下的input_code.txt"<<endl;
    cout << "     2.该编译程序目前支持类c语法的词法分析和语法分析,支持有关简单四则运算的语义分析和risc-v汇编代码生成"<<endl;
    cout <<("-----------------------------------------------------------------------------------------------")<<endl;
    cout << "请选择要进行的编译步骤:"<<endl;
    cout << "1.词法分析"<<endl;
    cout << "2.语法分析"<<endl;
    cout << "3.目标代码生成"<<endl;
    cout << "4.进行上述所有步骤"<<endl;
    cout << "0.退出"<<endl;
    cout <<("-----------------------------------------------------------------------------------------------")<<endl;
    
    cin>>option;

    switch(option){
        case(0):{
            return 0;
            break;
        }
        case(1):{
            lexical_analyzer();
            cout<<("lexical analysis has completed!")<<endl;
            break;
        }
        case(2):{
            syntax_analysis();
            cout<<("syntax analysis has completed!")<<endl;
            break;
        }
        case(3):{
            asm_code_creator();
            cout<<"asm code has created!"<<endl;
            break;
        }
        case(4):{
            
            lexical_analyzer();
        
            cout<<("lexical analysis has completed!")<<endl;
        
            syntax_analysis();
        
            cout<<("syntax analysis has completed!")<<endl;
        
            asm_code_creator();
        
            cout<<"asm code has created!"<<endl;
            break;
        }
        default:{
            break;
        }
    }
    return 0;
}