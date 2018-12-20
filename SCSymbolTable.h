//------------------------------------------------------
// SC动态脚本语言的编译器（符号表）
//   作者：张毅27651
//   时间：2008-12-16
//------------------------------------------------------

#ifndef _SC_SYMBOL_TABLE_H
#define _SC_SYMBOL_TABLE_H

//中间代码表类
#include "SCCodeTable.h"

//引用函数类
struct CSCFunction;


//------------------------------------------------------
// 符号的定义
//------------------------------------------------------
struct CSCSymbol
{
    CNString name;   //符号的名称
    ESymbolType type;//符号类型
    int value;       //符号的取值
    int lin;         //记号所在行号，从0开始
    int col;         //记号所在列号，从0开始

    //初始化
    void Init(const char *_name, const ESymbolType _type);
    
    //清空内存
    void Clear();
    
    //字符串指针
    CNString * str() {return (CNString *)value;}

    //函数指针
    CSCFunction * func() {return (CSCFunction *)value;}

    //无参构造函数
    CSCSymbol() {type = stVoid; value = lin = col = 0;}
    
    //析构函数
    virtual ~CSCSymbol() {Clear();}
    
    //重载比较运算符号，供模板类CNList搜索符号
    Bool operator <  (const CSCSymbol &right) const {return name <  right.name;}
    Bool operator == (const CSCSymbol &right) const {return name == right.name;}
    Bool operator >  (const CSCSymbol &right) const {return name >  right.name;}

    //用于调试
    void Debug(const Bool showdetail = True);
};


//------------------------------------------------------
// 符号表
//------------------------------------------------------
struct CSCSymbolTable : public CNList<CSCSymbol>
{
    //添加一个符号
    CSCSymbol &AddSymb(CSCSymbol &_symb);
    CSCSymbol &AddSymb(const char *name, const ESymbolType type);
    CSCSymbol &AddSymb(const char *name, const ESymbolType type,
        const CSCLexicaler &lex); 

    //根据名称查找符号，失败返回0
    CSCSymbol * Lookup(const char *name);

    //用于调试，showsys定时是否显示系统函数与变量
    void Debug(const Bool showsyscall = False);
};


//------------------------------------------------------
// 函数类
//------------------------------------------------------
struct CSCFunction
{
    CSCSymbolTable symbols; //函数的局部变量
    CSCCodeTable codes;     //函数的中间代码表
    CNString name;          //函数的名称
    int paras;              //参数数量，无参数的取值0
    Bool syscall;           //是不是系统内部函数

    //无参构造函数
    CSCFunction() {paras = syscall = 0;}
    
    //用于调试
    void Debug();
};

#endif  /* _SC_SYMBOL_TABLE_H */

