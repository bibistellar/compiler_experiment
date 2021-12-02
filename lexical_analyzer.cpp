#include "define.h"

/*增加关键字需要修改define.h中的WORD_CODE和lexical_analyzer.h中的key_word map.*/

using namespace std;

#include "lexical_analyzer.h"

int lexical_analyzer(){
    Scanner scanner;
    //ofstream f_symbol_table;
    //Symbol_Table symbol_table;

    f_input.open(input_path,ios::in);
    f_token.open(token_path,ios::out);
    //f_symbol_table.open(symbol_table_path,ios::out);
    if(!f_input){
        printf("Fail to open input_code.txt!\n");
        return 0;
    }
    if(!f_token){
        printf("Fail to open token.txt!\n");
        return 0;
    }
    //if(!f_symbol_table){
    //    printf("Fail to open symbolTable.txt!\n");
    //    return 0;
    //}

    while(!f_input.eof()){
        scanner.start(f_input);
        scanner.key_check();
        if(scanner.get_code() != -1){
            scanner.write_file(f_token);
        }
        /*if(scanner.get_code() == ID){
            symbol_table.insert(scanner.get_token());
        }*/
    }
    f_token << "(" << FILE_END << ",$)\n";

    //symbol_table.write_file(f_symbol_table);

    f_input.close();
    f_token.close();
    //f_symbol_table.close();

    return 0;
}
