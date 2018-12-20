//------------------------------------------------------
// SC动态脚本语言的编译器（中间代码表）
//   作者：张毅27651
//   时间：2009-04-09
//------------------------------------------------------

#ifndef _SC_CODE_TABLE_H
#define _SC_CODE_TABLE_H

//基础定义
#include "SCBase.h"

//词法分析器
#include "SCLexicaler.h"


//------------------------------------------------------
// 表达式运算符
//------------------------------------------------------
struct CSCOperator
{
    EExprElementType type;//运算符大类型
    EOperatorType oper;   //运算符具体类型
    int lin;              //记号所在行号，从0开始
    int col;              //记号所在列号，从0开始
    
    //无参构造函数
    CSCOperator() {type = etError; oper = otVoid; lin = col = 0;}
};


//------------------------------------------------------
// 表达式运算符栈
//------------------------------------------------------
struct CSCOperatorStack : public CNVector<CSCOperator>
{
    //添加一条运算符
    CSCOperator &AddOper(const EExprElementType type,
        const CSCLexicaler &lex);
};


struct CSCSymbol;

//------------------------------------------------------
// 表达式符号
//------------------------------------------------------
struct CSCCodeSymbol
{
    CSCSymbol *symbol; //指向全局或函数的符号表的指针
    ESymbolType type;  //符号类型
    int value;         //整型、布尔型常数
    CNString str;      //字符串型常数
    
    //无参构造函数
    CSCCodeSymbol() {Clear();}

    //清空
    void Clear()
    {
        symbol = NULL; 
        type = stVoid; 
        value = 0; 
        str = "";
    }
    
    //判断符号类型
    Bool IsInt();         //是不是整数
    Bool IsStr();         //是不是字符串
    Bool IsBool();        //是不是布尔值
    Bool IsVar();         //是不是变量
    ESymbolType VarType();//返回Var类型

    //用于调试
    void Debug() const;
};


//------------------------------------------------------
// 表达式符号栈
//------------------------------------------------------
struct CSCSymbolStack : public CNVector<CSCCodeSymbol>
{
    //添加一条符号
    CSCCodeSymbol &AddSymb();
    
    //用于调试
    void Debug() const;
};


//------------------------------------------------------
// 中间代码(逆波兰式)
//------------------------------------------------------
struct CSCCode
{
    EOperatorType oper;   //运算符
    CSCCodeSymbol symb1;  //值1
    CSCCodeSymbol symb2;  //值2
    CSCCodeSymbol result; //结果
    int lin;              //记号所在行号，从0开始
    int col;              //记号所在列号，从0开始
    
    //无参构造函数
    CSCCode() {oper = otVoid, lin = col = 0;}
    
    //用于调试
    void Debug() const;
};


//------------------------------------------------------
// 中间代码表(逆波兰式表)
//------------------------------------------------------
struct CSCCodeTable : public CNVector<CSCCode>
{
    //添加一条指令
    CSCCode &AddCode(const EOperatorType oper);
    CSCCode &AddCode(const EOperatorType oper,
        const CSCLexicaler &lex);
    CSCCode &AddCode(const CSCOperator &oper);
    
    //用于调试
    void Debug() const;
};


//------------------------------------------------------
// 函数语法结构分析
//------------------------------------------------------
struct CSCState
{
    EStatementType type; //函数语法结构类型
    int breakpos;    //退出的指令位置
    int contipos;    //继续循环的指令位置
    int lin;         //记号所在行号，从0开始
    int col;         //记号所在列号，从0开始
    
    //无参构造函数
    CSCState() {breakpos = contipos = lin = col = 0;}
};

//------------------------------------------------------
// 函数语法结构分析栈
//------------------------------------------------------
struct CSCStateStack : public CNVector<CSCState>
{
    //添加一条函数语法结构
    CSCState &AddState(const EStatementType type, const CSCLexicaler &lex);
};


#endif  /* _SC_CODE_TABLE_H */

