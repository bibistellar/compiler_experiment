#ifndef DEFINE_FILE
#define DEFINE_FILE

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <stack>
#include <cstring>

#define MAXSIZE 256
#define MAXPRODUCTION 1024
#define key_word_num 12
#define MAX_NAME_LENGTH 1024

//词法分析器编码
enum WORD_CODE{
    ID = 1,
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_STRING,
    KEY_INT,//5
    KEY_FLOAT,//6
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
    KEY_RETURN,
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
    SEP_COMMA,//,
    FILE_END//$文件结束符号
}word_code;

//编码分类，便于缩减状态
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

#endif

