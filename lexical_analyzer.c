#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define key_word_num 12
#define MAX_NAME_LENGTH 1024

//备注:关键字属性的匹配依赖于关键字在WORD_CODE编码和key_word字符串组中的位置,增加新的关键字需要更改上述两个的代码，以及if_key子函数和key_word_num宏定义.
enum WORD_CODE{
    ID = 1,
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_STRING,
    KEY_INT,//5
    KEY_FLOAT,
    KEY_CHAR,
    KEY_IF,
    KEY_ELSE,
    KEY_FOR,
    KEY_WHILE,
    KEY_BREAK,
    KEY_CONTINUE,
    KEY_STRUCT,
    KEY_SWITCH,
    KEY_CASE,
    OP_PLUS,//+
    OP_ASSIGN,//=
    OP_MINUS,//-
    OP_DIVISION,// /
    OP_MULT,//*
    CMP_EQ,//==
    CMP_GE,//>=
    CMP_GT,//>
    CMP_LE,//<=
    CMP_LT,//<
    CMP_NEQ,//!=
    SEP_L_PARENTHESES,//(
    SEP_R_PARENTHESES,//)
    SEP_L_BRACKET,//[
    SEP_R_BRACKET,//]
    SEP_L_BRACES,//{
    SEP_R_BRACES,//}
    SEP_DQM,//"
    SEP_POINT,//.
    SEP_COLON,//:
    SEP_SEMICOLON,//;
    SEP_COMMA//,
}word_code;

enum WORD_ATTR{
    LETTER = 1,
    DIGIT,
    OP,
    CMP,
    SEP,
    OTHER,
    UNKNOWN,
    BLANK
}word_attr;

char *key_word[key_word_num] = {
    "int",
    "float",
    "char",
    "if",
    "else",
    "for",
    "while",
    "bread",
    "continue",
    "struct",
    "switch",
    "case"
};

struct Symbol_Table_Item{
    char name[MAX_NAME_LENGTH];
    int attribute;
    int offset;
    struct Symbol_Table_Item* next_item;
};

struct Symbol_Table{
    struct Symbol_Table_Item* first_item;
    int item_number;
};

struct Scanner_Ret{
    int code;
    char token[MAX_NAME_LENGTH];
};

int word_clf(char ch);//对字符进行一个简单的分类
void scanner(FILE* f_input,struct Scanner_Ret *scanner_result);//扫描单词并进行分类
int if_key(char* word);//检查标识符是否为关键字，如果是则返回关键字编码
void symbol_insert(struct Symbol_Table *symbol_table,struct Scanner_Ret result);//将得到的标识符插入符号表中，如果符号表中已有该符号，则什么也不会发生
void write_token(FILE* f_token,struct Scanner_Ret scanner_result);//将得到的单词写入文件中
void write_symbol_table(FILE* f_table,struct Symbol_Table *symbol_table);//将得到的符号表写入文件中
void free_symbol_table(struct Symbol_Table *symbol_table);//释放符号表所申请的内存空间

int main(){
    struct Scanner_Ret scanner_result;
    struct Symbol_Table symbol_table = {NULL,0};
    FILE *f_input;
    FILE *f_token;
    FILE *f_symbol_table;

    //打开文件
    f_input = fopen("input_code.txt","r");
    f_token = fopen("token.txt","w+");
    f_symbol_table = fopen("symbolTable.txt","w+");
    if(f_input == NULL){
        printf("Fail to open input_code.txt!\n");
        return 0;
    }
    if(f_token == NULL){
        printf("Fail to open token.txt!\n");
        return 0;
    }
    if(f_symbol_table == NULL){
        printf("Fail to open symbolTable.txt!\n");
        return 0;
    }

    //扫描单词，并将得到的结果写入token文件和符号表中
    while(!feof(f_input)){
        scanner(f_input,&scanner_result);
        if(scanner_result.code == 1){
            scanner_result.code = if_key(scanner_result.token);
        }

        if(scanner_result.code == ID){
            symbol_insert(&symbol_table,scanner_result);
        }
        if(scanner_result.code != -1){
            write_token(f_token,scanner_result);
        }
    }

    //将符号表写入文件中
    write_symbol_table(f_symbol_table,&symbol_table);
    //释放内存
    free_symbol_table(&symbol_table);

    fclose(f_input);
    fclose(f_token);
    fclose(f_symbol_table);
    return 0;
}

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

void scanner(FILE* f_input,struct Scanner_Ret *scanner_result){
    char ch;
    //假设变量名称不超过1024个字符
    int token_pointer = 0;
    int state = 0;
    int ch_attr = UNKNOWN;
    int scanner_end = 0;
    while(!scanner_end){
        
        if(token_pointer == 1023){
            state = -1;
        }
        //进行状态转移
        //先使用ch_attr判定下一个状态，ch_attr无法识别再具体分析ch
        switch (state){
            case -1:{
                state = -1;
                scanner_end = 1;
                scanner_result->code = -1;
                break;
            }
            case 0:{
                ch = fgetc(f_input);
                ch_attr = word_clf(ch);
                switch(ch_attr){
                    case BLANK:{
                        state = 0;
                        if(feof(f_input)){
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
                scanner_result->token[token_pointer] = ch;
                break;
            }
            case 1:{
                ch = fgetc(f_input);
                ch_attr = word_clf(ch);
                if(ch_attr == LETTER || ch_attr == DIGIT){
                    state = 1;
                }
                else{
                    state = 2;
                }
                scanner_result->token[++token_pointer] = ch;
                break;
            }
            case 2:{
                if(!feof(f_input)){
                    fseek(f_input,-1,SEEK_CUR);
                }
                scanner_result->token[token_pointer]='\0';
                scanner_result->code = ID;
                scanner_end = 1;
                break;
            }
            case 3:{
                ch = fgetc(f_input);
                if(ch >= '0' && ch <= '9'){
                    state = 3;
                }
                else if(ch == '.'){
                    state = 5;
                }
                else{
                    state = 4;
                }
                scanner_result->token[++token_pointer] = ch;
                break;
            }
            case 4:{
                if(!feof(f_input)){
                    fseek(f_input,-1,SEEK_CUR);
                }
                scanner_result->token[token_pointer]='\0';
                scanner_result->code = VALUE_INT;
                scanner_end = 1;
                break;
            }
            case 5:{
                ch = fgetc(f_input);
                ch_attr = word_clf(ch);
                if(ch_attr == DIGIT){
                    state = 5;
                }
                else{
                    state = 6;
                }
                scanner_result->token[++token_pointer] = ch;
                break;
            }
            case 6:{
                if(!feof(f_input)){
                    fseek(f_input,-1,SEEK_CUR);
                }
                scanner_result->token[token_pointer]='\0';
                scanner_result->code = VALUE_FLOAT;
                scanner_end = 1;
                break;
            }
            case 7:{
                ch = fgetc(f_input);
                scanner_result->token[++token_pointer] = ch;
                state = 23;
                break;
            }
            case 8:{
                ch = fgetc(f_input);
                scanner_result->token[++token_pointer] = ch;
                state = 24;
                break;
            }
            case 9:{
                ch = fgetc(f_input);
                scanner_result->token[++token_pointer] = ch;
                if(ch == '='){
                    state = 10;
                }
                else{
                    state = 11;
                }
                break;
            }
            case 10:{
                ch = fgetc(f_input);
                scanner_result->token[++token_pointer] = ch;
                state = 25;
                break;
            }
            case 11:{
                if(!feof(f_input)){
                    fseek(f_input,-1,SEEK_CUR);
                }
                scanner_result->token[token_pointer]='\0';
                scanner_result->code = OP_ASSIGN;
                scanner_end = 1;
                break;
            }
            case 12:{
                ch = fgetc(f_input);
                scanner_result->token[++token_pointer] = ch;
                if(ch == '!'){
                    state = 13;
                }
                else{
                    state = -1;
                }
                break;
            }
            case 13:{
                ch = fgetc(f_input);
                scanner_result->token[++token_pointer] = ch;
                state = 26;
                break;
            }
            case 14:{
                ch = fgetc(f_input);
                scanner_result->token[++token_pointer] = ch;
                if(ch == '='){
                    state = 15;
                }
                else{
                    state = 16;
                }
                break;
            }
            case 15:{
                ch = fgetc(f_input);
                scanner_result->token[++token_pointer] = ch;
                state = 27;
                break;
            }
            case 16:{
                if(!feof(f_input)){
                    fseek(f_input,-1,SEEK_CUR);
                }
                scanner_result->token[token_pointer]='\0';
                scanner_result->code = CMP_GT;
                scanner_end = 1;
                break; 
            }
            case 17:{
                ch = fgetc(f_input);
                scanner_result->token[++token_pointer] = ch;
                if(ch == '='){
                    state = 18;
                }
                else{
                    state = 19;
                }
                break;
            }
            case 18:{
                ch = fgetc(f_input);
                scanner_result->token[++token_pointer] = ch;
                state = 28;
                break;
            }
            case 19:{
                if(!feof(f_input)){
                    fseek(f_input,-1,SEEK_CUR);
                }
                scanner_result->token[token_pointer]='\0';
                scanner_result->code = CMP_LT;
                scanner_end = 1;
                break; 
            }
            case 20:{
                ch = fgetc(f_input);
                scanner_result->token[++token_pointer] = ch;
                state = 21;
                break;
            }
            case 21:{
                ch = fgetc(f_input);
                scanner_result->token[++token_pointer] = ch;
                if(ch == '"'){
                    state = 22;
                }
                else{
                    state = 21;
                }
                break;
            }
            case 22:{
                ch = fgetc(f_input);
                scanner_result->token[++token_pointer] = ch;
                state = 29;
                break;
            }
            case 23:{
                if(!feof(f_input)){
                    fseek(f_input,-1,SEEK_CUR);
                };
                switch (scanner_result->token[token_pointer-1])
                {
                case '+': scanner_result->code = OP_PLUS;break;
                case '-': scanner_result->code = OP_MINUS;break;
                case '*': scanner_result->code = OP_MULT;break;
                case '/': scanner_result->code = OP_DIVISION;break;
                default:
                    break;
                }
                scanner_result->token[token_pointer]='\0';
                scanner_end = 1;
                break;
            }
            case 24:{
                if(!feof(f_input)){
                    fseek(f_input,-1,SEEK_CUR);
                }
                switch (scanner_result->token[token_pointer-1])
                {
                case ',': scanner_result->code = SEP_COMMA;break;
                case ';': scanner_result->code = SEP_SEMICOLON;break;
                case ':': scanner_result->code = SEP_COLON;break;
                case '(': scanner_result->code = SEP_L_PARENTHESES;break;
                case ')': scanner_result->code = SEP_R_PARENTHESES;break;
                case '[': scanner_result->code = SEP_L_BRACKET;break;
                case ']': scanner_result->code = SEP_R_BRACKET;break;
                case '{': scanner_result->code = SEP_L_BRACES;break;
                case '}': scanner_result->code = SEP_R_BRACES;break;
                default:
                    break;
                }
                scanner_result->token[token_pointer]='\0';
                scanner_end = 1;
                break;
            }
            case 25:{
                if(!feof(f_input)){
                    fseek(f_input,-1,SEEK_CUR);
                }
                scanner_result->token[token_pointer]='\0';
                scanner_result->code = CMP_EQ;
                scanner_end = 1;
                break;
            }
            case 26:{
                if(!feof(f_input)){
                    fseek(f_input,-1,SEEK_CUR);
                }
                scanner_result->token[token_pointer]='\0';
                scanner_result->code = CMP_NEQ;
                scanner_end = 1;
                break;
            }
            case 27:{
                if(!feof(f_input)){
                    fseek(f_input,-1,SEEK_CUR);
                }
                scanner_result->token[token_pointer]='\0';
                scanner_result->code = CMP_GE;
                scanner_end = 1;
                break;
            }
            case 28:{
                if(!feof(f_input)){
                    fseek(f_input,-1,SEEK_CUR);
                }
                scanner_result->token[token_pointer]='\0';
                scanner_result->code = CMP_LE;
                scanner_end = 1;
                break;
            }
            case 29:{
                if(!feof(f_input)){
                    fseek(f_input,-1,SEEK_CUR);
                }
                scanner_result->token[token_pointer]='\0';
                scanner_result->code = VALUE_STRING;
                scanner_end = 1;
                break;
            }
        }
    }
}

int if_key(char* word){
    //TODO:将来考虑使用键值对的方式进行匹配，不过c语言没有map，要去找别人的轮子
    for(int i =0;i<key_word_num;i++){
        if(strcmp(word,key_word[i]) == 0){
            return i+5;
        }
    }
    return 1;
}

void symbol_insert(struct Symbol_Table *symbol_table,struct Scanner_Ret result){
    if(symbol_table->first_item == NULL){
        struct Symbol_Table_Item* new_item = (struct Symbol_Table_Item*)malloc(sizeof(struct Symbol_Table_Item));
        strcpy(new_item->name,result.token);
        new_item->next_item = NULL;
        new_item->attribute = -1;
        new_item->offset = 0; 
        symbol_table->first_item = new_item;
        symbol_table->item_number = 1;
    }
    else{
        struct Symbol_Table_Item*p = symbol_table->first_item;
        struct Symbol_Table_Item*q = symbol_table->first_item;
        while(p!=NULL){
            if(strcmp(p->name,result.token) == 0){
                return ;
            }
            q = p;
            p = p->next_item;
        }

        struct Symbol_Table_Item* new_item = (struct Symbol_Table_Item*)malloc(sizeof(struct Symbol_Table_Item));
        strcpy(new_item->name,result.token);
        new_item->next_item = NULL;
        new_item->attribute = -1;
        new_item->offset = 0;


        q->next_item = new_item;
        symbol_table->item_number++;
    }
    return ;
}

void write_token(FILE* f_token,struct Scanner_Ret scanner_result){
    fprintf(f_token,"(%d,",scanner_result.code);
    fprintf(f_token,"%s)\n",scanner_result.token);
}

void write_symbol_table(FILE* f_table,struct Symbol_Table *symbol_table){
    struct Symbol_Table_Item*p = symbol_table->first_item;
    while(p!=NULL){
        fputc('(',f_table);
        fprintf(f_table,"%s,-1,?)\n",p->name);
        p = p->next_item;
    }
}

void free_symbol_table(struct Symbol_Table *symbol_table){
    struct Symbol_Table_Item*p = symbol_table->first_item;
    struct Symbol_Table_Item*temp;
    while(p!=NULL){
        temp=p;
        p=p->next_item;
        free(temp);    
    }
}