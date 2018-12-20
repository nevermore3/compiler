//------------------------------------------------------
// SC动态脚本语言的编译器（词法分析器）
//   作者：张毅27651
//   时间：2008-12-09
//------------------------------------------------------

#ifndef _SC_LEXICALER_H
#define _SC_LEXICALER_H

//基础函数库
#include "SCBase.h"


//------------------------------------------------------
// 记号类
//------------------------------------------------------
struct CSCToken
{
    ETokenType type;//记号类型
    CNString str;   //记号的值
    int lin;        //记号所在行号，从0开始
    int col;        //记号所在列号，从0开始
};


//------------------------------------------------------
// 词法分析器类
//------------------------------------------------------
struct CSCLexicaler
{
    CNStringList list;     //代码文件
    CNStringList operators;//操作符列表
    CNStringList keywords; //关键字列表
    int lin;               //当前行号，从0开始
    int col;               //当前列号，从0开始
    CSCToken token;        //当前记号
    int errors;            //打印的错误数量

    //加载代码文件
    int LoadScript(const char *filename);

    //依次读取记号，成功返回M_SC_POK，失败返回M_SC_ERR，文件结束返回M_SC_OK
    int GetNextToken();

    //跳到下一行，是GetNextToken的私有函数
    void JumpLine();

    //初始化token，是GetNextToken的私有函数
    void InitToken(ETokenType type);

    //获得字符类型，是GetNextToken的私有函数
    ECharType GetCharType(char c);

    //语法制导方式获得整数，是GetNextToken的私有函数
    int GetInteger();

    //语法制导方式获得字符串，是GetNextToken的私有函数
    int GetString();

    //语法制导方式获得单词，是GetNextToken的私有函数
    int GetWord();

    //语法制导方式获得操作符，是GetNextToken的私有函数
    int GetOperator();

    //打印信息，也允许语法分析和虚拟机调用此函数
    void Printf(const char* format, ...);

    //打印错误信息，也允许语法分析和虚拟机调用此函数
    void PrintError(const int ID, const char* format, ...);

    //自定义行号打印错误打印错误信息
    void PrintErrorLin(const int lin, const int col, 
        const int ID, const char* format, ...);

    //用于调试，打印当前记号信息
    void Debug();
};


#endif  /* _SC_LEXICALER_H */

