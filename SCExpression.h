//------------------------------------------------------
// SC动态脚本语言的编译器（表达式解析）
//   作者：张毅27651
//   时间：2009-04-03
//------------------------------------------------------

#ifndef _SC_EXPRESSION_H
#define _SC_EXPRESSION_H

//词法分析器
#include "SCLexicaler.h"

//符号表
#include "SCSymbolTable.h"

//引用语法分析器类
struct CSCParser;

    
//------------------------------------------------------
// 表达式解析类
//------------------------------------------------------
struct CSCExpression
{
    CSCParser *parser;            //语法分析器，被CSCParser初始化
    EExprElementType curtype;     //当前表达式元素的类型
    CSCOperatorStack operatorstk; //表达式的运算符栈
    CSCSymbolStack symbolstk;     //表达式的符号栈
        
    //表达式运算，是表达式解析类的唯一入口
    //needtpye: stConst...要求返回常量；
    // stVar...要求返回常变量；stVoid不返回，其他不限制
    int Calc(const ESymbolType needtype);

    //获得当前记号的表达式元素类型，Calc的私有函数
    int GetNextElem();

    //当前记号进运算符栈，Calc的私有函数
    void PushOperator();
    
    //当前记号进符号栈，Calc的私有函数
    void PushSymbol();
    void PushSymbol(const CSCCodeSymbol &symbol);
    
    //出栈一个逆波兰式，Calc的私有函数
    int PopAOper();

    //一直出栈到与oper匹配的运算符，Calc的私有函数
    int PopWith(const EExprElementType type);
    
    //所有表达式都出栈，Calc的私有函数
    int PopAll();

    //出栈一个单目运算，PopAOper的私有函数
    int PopOne(CSCCode &code, const CSCOperator &oper);
    
    //出栈一个双目运算，PopAOper的私有函数
    int PopTwo(CSCCode &code, const CSCOperator &oper);
    
    //出栈一个函数，PopAOper的私有函数
    int PopFunc(CSCCode &code);
};


#endif  /* _SC_EXPRESSION_H */

