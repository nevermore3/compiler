//------------------------------------------------------
// SC动态脚本语言的编译器（虚拟机）
//   作者：张毅27651
//   时间：2008-12-09
//------------------------------------------------------

#ifndef _SC_VIRTUAL_MACHINE_H
#define _SC_VIRTUAL_MACHINE_H

//语法分析器
#include "SCParser.h"

//函数调用栈
#include "SCCallStack.h"


//------------------------------------------------------
// 虚拟机类
//------------------------------------------------------
struct CSCVirtualMachine
{
    CSCSymbolTable symbols; //符号表
    CSCCallStack callstk;   //函数调用栈

    //初始化符号表，LoadScript的私有函数
    int InitSymbols();
    
    //初始化函数调用栈，RunScript的私有函数
    int InitCallstk();

    //加载与编译代码文件（包括词法分析、语法分析）
    int LoadScript(const char *filename);

    //执行中间代码
    int RunScript();
};

#endif  /* _SC_VIRTUAL_MACHINE_H */

