//------------------------------------------------------
// SC动态脚本语言的编译器（本编译器的基础函数库）
//   作者：张毅27651
//   时间：2008-12-09
//------------------------------------------------------

#ifndef _SC_BASE_H
#define _SC_BASE_H

//基础函数库
#include "normal/normal.h"

//返回码定义
#define M_SC_OK  0   //执行成功(如:执行函数成功)
#define M_SC_ERR 1   //执行失败
#define M_SC_POK 2   //执行阶段性成功(如:执行函数的1条指令成功)
#define M_SC_BRK 3   //执行被断点断住
#define M_SC_FUC 4   //执行到调用函数，请上级继续操作
#define M_SC_QUT 5   //强制退出程序执行

//临时变量的宏
#define M_SC_MAX_TEMP 100   //函数最多申请的某种临时变量数量
#define M_SC_FREE     0     //未被使用的临时变量
#define M_SC_USE      1     //已经被使用的临时变量

//记号类型
typedef enum
{
    ttInteger = 0,  //整数
    ttString,       //字符串
    ttOperator,     //操作符
    ttKeyword,      //关键字
    ttWord,         //单词
    ttError         //出错
} ETokenType;
extern const char * ETokenStr[ttError + 1];

//字符类型
typedef enum
{
    ctSpace = 0,    //空格
    ctZero,         //'\0'
    ctNumber,       //数字：0-9
    ctLetter,       //字母：a-z, A-Z, _
    ctString,       //双引号：'\"'
    ctSign,         //操作符或注释：-!%&()*,/;{|}+<=>
    ctError         //出错
} ECharType;
extern const char * ECharStr[ctError + 1];

//符号类型
typedef enum
{
    stVarInteger = 0,//整数变量
    stVarString,     //字符串变量
    stVarBoolean,    //布尔值变量
    stConstInteger,  //整数常量
    stConstString,   //字符串常量
    stConstBoolean,  //布尔值常量
    stFunction,      //函数
    stVoid           //未指定
} ESymbolType;
extern const char * ESymbolStr[stVoid + 1];

//表达式的元素类型
typedef enum
{
    etLogicNot = 0, //逻辑非运算!
    etSign,         //正负号+、-
    etMulDiv,       //乘除号*、/、%
    etPluMin,       //加减号+、-
    etCompare,      //比较符号>、<、>=、<=
    etCompareEqu,   //等于比较符号!=、==
    etLogicAnd,     //逻辑运算&&
    etLogicOr,      //逻辑运算||
    etLBracket,     //左括号(
    etRBracket,     //右括号)
    etComma,        //逗号,
    etInteger,      //整数常数
    etString,       //字符串常数
    etBoolean,      //布尔常数
    etConstName,    //常量名
    etVarName,      //变量名
    etFuncName,     //函数名
    etFuncVarName,  //函数参数或局部变量名
    etError         //非期望字符
} EExprElementType;
extern const char * EExprElementStr[etError + 1];

//表达式的运算符类型
typedef enum
{
    otLBracket = 0, //左括号(
    otFuncLBra,     //函数左括号(
    otComma,        //逗号,
    otSignPlu,      //正号+
    otSignMin,      //负号-
    otPlu,          //加号+
    otMin,          //减号-
    otMul,          //乘号*
    otDiv,          //除号/
    otMod,          //模号%
    otMore,         //>
    otLess,         //<
    otMoreEqu,      //>=
    otLessEqu,      //<=
    otNotEqu,       //!=
    otEqu,          //==
    otOr,           //||
    otAnd,          //&&
    otNot,          //!
    otAssign,       //赋值
    otReturn,       //返回
    otGoto,         //无条件跳转
    otGoNot,        //条件为否时跳转
    otVoid          //无操作
} EOperatorType;
extern const char * EOperatorStr[otVoid + 1];
extern const char * EOperatorSign[otVoid + 1];

//函数的语法结构类型
typedef enum
{
    ftIf = 0,  //If条件语句
    ftFor,     //For循环语句
    ftWhile,   //While循环语句
    ftDoWhile, //Do...While循环语句
    ftSwitch,  //Switch分支语句
    ftError    //无意义
} EStatementType;
extern const char * EStatementStr[ftError + 1];

#endif  /* _SC_BASE_H */

