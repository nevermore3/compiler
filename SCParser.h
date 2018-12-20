//------------------------------------------------------
// SC动态脚本语言的编译器（语法分析器）
//   作者：张毅27651
//   时间：2008-12-09
//------------------------------------------------------

#ifndef _SC_PARSER_H
#define _SC_PARSER_H

//表达式解析类
#include "SCExpression.h"

//引用语法分析器类
struct CSCParser;


//------------------------------------------------------
// 语法分析器类
//------------------------------------------------------
struct CSCParser
{
    CSCLexicaler lex;        //词法分析器
    CSCExpression expr;      //表达式
    CSCSymbolTable *symbols; //符号表，被虚拟机初始化
    CSCSymbol *symbol;       //当前分析的符号
    CSCSymbol *tempsymbol;   //用来查找的符号
    CSCFunction *func;       //当前函数的指针
    CSCStateStack statestk;  //函数语法结构分析栈

    //执行语法分析，CSCParser为唯一入口函数
    int Parse(const char *filename);

    //Parse的私有函数，用来处理各种Block
    int Block();         //Block的分流函数
    int Const();         //分析全局常量
    int VarOrFunction(); //分析全局变量或函数

    //VarOrFunction的私有函数，用来分析函数
    int FunctionDefine(); //分析函数定义
    int FunctionBody();   //分析函数体

    //符号(常变量/函数名)的各种处理
    EExprElementType SymbolLookup();   //查找现有符号
    int SymbolGetName(CSCSymbol *symb);//从lex获得符号名
    
    //变量处理，尤其是临时变量
    //对于临时变量: value是临时数组下表, lin是占用标志
    int VarAssign(CSCSymbol *symb, const int value);//变量赋值
    void VarAllocTemp(const ESymbolType type, CSCCodeSymbol &symb);
    void VarFreeTemp(const CSCCodeSymbol &symb);//释放临时变量
    int VarLocalDefine(); //局部变量定义

    //编译语句组和语句
    int StatementGroup(); //编译语句组
    int Statement();      //编译语句
    int StatementLookup(const char *type);//查找五大语句定义
    CSCState &StatementAdd(const EStatementType type);//添加五大语句
    
    //分析五大语句，Statement的私有函数
    int StatementIf();
    int StatementFor();
    int StatementWhile();
    int StatementDoWhile();
    int StatementSwitch();
    
    //分析五大语句的关键字
    int KeyReturn();  //退出函数
    int KeyBreak();   //退出五大语句
    int KeyContinue();//继续循环五大语句
    int KeyCase();    //Switch的分支
    int KeyDefault(); //Switch的默认分支

    //为五大语句添加中间代码
    void CodeAddGoto(const int pos);  //无条件跳转
    void CodeAddGoNot(const int pos); //有条件跳转
    void CodeSetGoNot(const int pos); //设置有条件跳转位置
    void CodeAddBreak();              //退出五大语句
};


#endif  /* _SC_PARSER_H */

