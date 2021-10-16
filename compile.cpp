#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <stack>
#include <cstring>

#include "lexical_analyzer.h"
#include "analysis_table.h"

using namespace std;

int main(){
    string token_path = "output/token.txt";
    string input_path = "input_code.txt";
    string symbol_table_path = "output/symbol_table.txt";
    string LR_path = "LR1.csv";
    string produce_path = "output/produce.txt";

    lexical_analyzer(input_path,token_path,symbol_table_path);

    cout<<("lexical analysis has completed!")<<endl;

    analysis_table(LR_path,token_path,produce_path);

    cout<<("yntax analysis has completed!")<<endl;
    return 0;
}