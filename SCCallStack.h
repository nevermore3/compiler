//------------------------------------------------------
// SC动态脚本语言的编译器（函数调用栈）
//   作者：张毅27651
//   时间：2009-05-05
//------------------------------------------------------

#ifndef _SC_CALL_STACK_H
#define _SC_CALL_STACK_H

//符号表
#include "SCSymbolTable.h"


//------------------------------------------------------
// 函数调用栈元素
//------------------------------------------------------
struct CSCCall
{
    CSCFunction *func;     //调用的函数指针
    CSCSymbolTable symbols;//函数的局部变量
    int pos;               //函数当前执行的代码表下标
    
    //无参构造函数
    CSCCall() {func = Null; pos = 0;}
    
    //用于调试
    void Debug();
    
    //系统调用的内部函数
    int SysCall();

    //执行中间代码，M_SC_OK:成功，ERR:失败，FUC:执行到调用函数
    int Run();

    //执行一条指令，Run的私有函数
    int RunCode(CSCCode &code);

    //整数运算
    int GetInt(CSCCodeSymbol &symb);
    void SetInt(CSCCodeSymbol &symb, int _value);

    //字符串运算
    CNString GetStr(CSCCodeSymbol &symb);
    void SetStr(CSCCodeSymbol &symb, CNString _str);
};


//------------------------------------------------------
// 函数调用栈
//------------------------------------------------------
struct CSCCallStack : public CNVector<CSCCall>
{
    //添加一个函数
    CSCCall &PushCall(CSCFunction *func);
    
    //删除一个函数
    void PopCall();
    
    //用于调试
    void Debug();

    //执行中间代码，M_SC_OK:成功，ERR:失败
    int Run();
};


#endif  /* _SC_CALL_STACK_H */

