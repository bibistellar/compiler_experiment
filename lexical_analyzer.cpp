#include "define.h"

/*增加关键字需要修改define.h中的WORD_CODE和lexical_analyzer.h中的key_word map.*/

using namespace std;

#include "lexical_analyzer.h"

//关键字匹配
map<string,int> key_word = {
    {"int",5},
    {"float",6},
    {"char",7},
    {"if",8},
    {"else",9},
    {"for",10},
    {"while",11},
    {"bread",12},
    {"continue",13},
    {"struct",14},
    {"switch",15},
    {"case",16},
    {"return",17}
};

//扫描器
class Scanner{
    private:
        int code;
        int state;
        int ch_attr = UNKNOWN;
        string token;
    public:
        //将读入的字符进行分类
        int word_clf(char ch){
            int ch_attr;
            if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'){
                ch_attr = BLANK;
            }
            else if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z'){
                ch_attr = LETTER;
            }
            else if(ch  >= '0' && ch <= '9'){
                ch_attr = DIGIT;
            }
            else if(ch == '+' || ch == '-' || ch == '/' || ch == '*'){
                ch_attr = OP;
            }
            else if(ch == ',' || ch == ';' || ch == ':' || ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}'){
                ch_attr = SEP;
            }
            else{
                ch_attr = OTHER;
            }
            return ch_attr;
        };
        //关键字检查，确定标识符是否属于关键字
        void key_check(){
            if(key_word.count(token)){
                code = key_word.at(token);
            }
        }
        //状态机，具体的状态图参照自动机文件，每次扫描一个单词，扫描结果存放于code和token中
        void start(ifstream& f_input){
            char ch;
            int scanner_end = 0;
            state = 0;
            f_input.unsetf(ios_base::skipws);
            while(!scanner_end){
                switch(state){
                    case -1:{
                        state = -1;
                        scanner_end = 1;
                        break;
                    }
                    case 0:{
                        token.clear();
                        f_input >> ch;
                        ch_attr = word_clf(ch);  
                        switch(ch_attr){
                            case BLANK:{
                                state = 0;
                                if(f_input.eof()){
                                    state = -1;
                                }
                                break;
                            }
                            case LETTER:{
                                state = 1;
                                break;
                            }
                            case DIGIT:{
                                state = 3;
                                break;
                            }
                            case OP:{
                                state = 7;
                                break;
                            }
                            case SEP:{
                                state = 8;
                                break;
                            }
                            default:{
                                state = -1;
                            }
                        }
                        if(state == -1){
                            switch(ch){
                                case '=':{
                                    state = 9;
                                    break;
                                }
                                case '!':{
                                    state = 12;
                                    break;
                                }
                                case '>':{
                                    state = 14;
                                    break;
                                }
                                case '<':{
                                    state = 17;
                                    break;
                                }
                                case '"':{
                                    state = 20;
                                    break;
                                }
                                default:{
                                    state = -1;
                                }
                            }
                        }     
                        token = token + ch;
                        break;
                    }
                    case 1:{
                        f_input >> ch;
                        ch_attr = word_clf(ch);
                        if(ch_attr == LETTER || ch_attr == DIGIT){
                            state = 1;
                        }
                        else{
                            state = 2;
                        }
                        token += ch;
                        break;       
                    }
                    case 2:{
                        if(!f_input.eof()){
                            f_input.seekg(-1,ios::cur);
                        }
                        token.erase(token.length()-1);
                        code = ID;
                        scanner_end = 1;
                        break;
                    }
                    case 3:{
                        f_input >> ch;
                        if(ch >= '0' && ch <= '9'){
                            state = 3;
                        }
                        else if(ch == '.'){
                            state = 5;
                        }
                        else{
                            state = 4;
                        }
                        token += ch;
                        break;
                    }
                    case 4:{
                        if(!f_input.eof()){
                            f_input.seekg(-1,ios::cur);
                        }
                        token.erase(token.length()-1);
                        code = VALUE_INT;
                        scanner_end = 1;
                        break;
                    }
                    case 5:{
                        f_input >> ch;
                        ch_attr = word_clf(ch);
                        if(ch_attr == DIGIT){
                            state = 5;
                        }
                        else{
                            state = 6;
                        }
                        token += ch;
                        break;
                    }
                    case 6:{
                        if(!f_input.eof()){
                            f_input.seekg(-1,ios::cur);
                        }
                        token.erase(token.length()-1);
                        code = VALUE_FLOAT;
                        scanner_end = 1;
                        break;
                    }
                    case 7:{
                        f_input >> ch;
                        token += ch;
                        state = 23;
                        break;
                    }
                    case 8:{
                        f_input >> ch;
                        token += ch;
                        state = 24;
                        break;
                    }
                    case 9:{
                        f_input >> ch;
                        token += ch;
                        if(ch == '='){
                            state = 10;
                        }
                        else{
                            state = 11;
                        }
                        break;
                    }
                    case 10:{
                        f_input >> ch;
                        token += ch;
                        state = 25;
                        break;
                    }
                    case 11:{
                        if(!f_input.eof()){
                            f_input.seekg(-1,ios::cur);
                        }
                        token.erase(token.length()-1);
                        code = OP_ASSIGN;
                        scanner_end = 1;
                        break;
                    }
                    case 12:{
                        f_input >> ch;
                        if(ch == '!'){
                            state = 13;
                        }
                        else{
                            state = -1;
                        }
                        token += ch;
                        break;
                    }
                    case 13:{
                        f_input >> ch;
                        token += ch;
                        state = 26;
                        break;
                    }
                    case 14:{
                        f_input >> ch;
                        token += ch;
                        if(ch == '='){
                            state =15;
                        }
                        else{
                            state = 16;
                        }
                        break;
                    }
                    case 15:{
                        f_input >> ch;
                        token += ch;
                        state = 27;
                        break;
                    }
                    case 16:{
                        if(!f_input.eof()){
                            f_input.seekg(-1,ios::cur);
                        }
                        token.erase(token.length()-1);
                        code = CMP_GT;
                        scanner_end = 1;
                        break;
                    }
                    case 17:{
                        f_input >> ch;
                        token += token;
                        if(ch == '='){
                            state = 18;
                        }
                        else{
                            state = 19;
                        }
                        break;
                    }
                    case 18:{
                        f_input >> ch;
                        token += ch;
                        state = 28;
                        break;
                    }
                    case 19:{
                        if(!f_input.eof()){
                            f_input.seekg(-1,ios::cur);
                        }
                        token.erase(token.length()-1);
                        code = CMP_LT;
                        scanner_end = 1;
                        break;
                    }
                    case 20:{
                        f_input >> ch;
                        token += ch;
                        state = 21;
                        break;
                    }
                    case 21:{
                        f_input >> ch;
                        token += ch;
                        if(ch == '"'){
                            state = 22;
                        }
                        else{
                            state = 21;
                        }
                        break;
                    }
                    case 22 :{
                        f_input >> ch;
                        token += ch;
                        state = 29;
                        break;
                    }
                    case 23:{
                        if(!f_input.eof()){
                            f_input.seekg(-1,ios::cur);
                        }
                        token.erase(token.length()-1);
                        switch(token.front()){
                            case '+': code = OP_PLUS;break;
                            case '-': code = OP_MINUS;break;
                            case '*': code = OP_MULT;break;
                            case '/': code = OP_DIVISION;break;
                            default:
                                break;
                        }
                        scanner_end = 1;
                        break;
                    }
                    case 24:{
                        if(!f_input.eof()){
                            f_input.seekg(-1,ios::cur);
                        }
                        token.erase(token.length()-1);
                        switch(token.front()){
                            case ',': code = SEP_COMMA;break;
                            case ';': code = SEP_SEMICOLON;break;
                            case ':': code = SEP_COLON;break;
                            case '(': code = SEP_L_PARENTHESES;break;
                            case ')': code = SEP_R_PARENTHESES;break;
                            case '[': code = SEP_L_BRACKET;break;
                            case ']': code = SEP_R_BRACKET;break;
                            case '{': code = SEP_L_BRACES;break;
                            case '}': code = SEP_R_BRACES;break;
                            default:
                                break;
                        }
                        scanner_end = 1;
                        break;
                    }
                    case 25:{
                        if(!f_input.eof()){
                            f_input.seekg(-1,ios::cur);
                        }
                        token.erase(token.length()-1);
                        code = CMP_EQ;
                        scanner_end = 1;
                        break;
                    }
                    case 26:{
                        if(!f_input.eof()){
                            f_input.seekg(-1,ios::cur);
                        }
                        token.erase(token.length()-1);
                        code = CMP_NEQ;
                        scanner_end = 1;
                        break;
                    }
                    case 27:{
                        if(!f_input.eof()){
                            f_input.seekg(-1,ios::cur);
                        }
                        token.erase(token.length()-1);
                        code = CMP_GE;
                        scanner_end = 1;
                        break;
                    }
                    case 28:{
                        if(!f_input.eof()){
                            f_input.seekg(-1,ios::cur);
                        }
                        token.erase(token.length()-1);
                        code = CMP_LE;
                        scanner_end = 1;
                        break;
                    }
                    case 29:{
                        if(!f_input.eof()){
                            f_input.seekg(-1,ios::cur);
                        }
                        token.erase(token.length()-1);
                        code = VALUE_STRING;
                        scanner_end = 1;
                        break;
                    }
                }
            }
        }
        int get_code(){
            return code;
        }
        void write_file(ofstream& f_token){
            if(state != -1){
                f_token << "(" << code << ',' << token <<")\n";
            }
        }
        string get_token(){
            return token;
        }
};

//符号表
class Symbol_Table{
    private:
        map<string,int>table;
        int item_number = 0;
    public:
        //将得到的标识符插入符号表中
        void insert(string symbol){ 
            if(!table.count(symbol)){
                table.insert(pair<string,int>(symbol,item_number++));
            }
        }
        void write_file(ofstream& f_symbol_table){
            for(map<string,int>::iterator i = table.begin();i != table.end();i++){
                f_symbol_table << "(" << i->first << ",-1,?)\n";
            }
        }
};


int lexical_analyzer(string input_path,string token_path,string symbol_table_path);

int lexical_analyzer(string input_path,string token_path,string symbol_table_path){
    Scanner scanner;
    ifstream f_input;
    ofstream f_token;
    ofstream f_symbol_table;
    Symbol_Table symbol_table;

    f_input.open(input_path,ios::in);
    f_token.open(token_path,ios::out);
    f_symbol_table.open(symbol_table_path,ios::out);
    if(!f_input){
        printf("Fail to open input_code.txt!\n");
        return 0;
    }
    if(!f_token){
        printf("Fail to open token.txt!\n");
        return 0;
    }
    if(!f_symbol_table){
        printf("Fail to open symbolTable.txt!\n");
        return 0;
    }

    while(!f_input.eof()){
        scanner.start(f_input);
        scanner.key_check();
        if(scanner.get_code() != -1){
            scanner.write_file(f_token);
        }
        if(scanner.get_code() == ID){
            symbol_table.insert(scanner.get_token());
        }
    }
    f_token << "(" << FILE_END << ",$)\n";

    symbol_table.write_file(f_symbol_table);

    f_input.close();
    f_token.close();
    f_symbol_table.close();

    return 0;
}