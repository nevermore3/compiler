//基础函数库
#include "SCBase.h"

//记号类型
const char * ETokenStr[ttError + 1] =
{
    "ttInteger",
    "ttString",
    "ttOperator",
    "ttKeyword",
    "ttWord",
    "ttError"
};

//字符类型
const char * ECharStr[ctError + 1] =
{
    "ctSpace",
    "ctZero",
    "ctNumber",
    "ctLetter",
    "ctString",
    "ctSign",
    "ctError"
};

//符号类型
const char * ESymbolStr[stVoid + 1] =
{
    "Vint",
    "Vstr",
    "Vbol",
    "Cint",
    "Cstr",
    "Cbol",
    "Func",
    "Void"
};

//表达式的元素类型
const char * EExprElementStr[etError + 1] =
{
    "LogicNot",
    "Sign",
    "MulDiv",
    "PluMin",
    "Compare",
    "CompareEqu",
    "LogicAnd",
    "LogicOr",
    "LBracket",
    "RBracket",
    "Comma",
    "Integer",
    "String",
    "Boolean",
    "ConstName",
    "VarName",
    "FuncName",
    "FuncVarName",
    "Error"
};

//表达式的运算符类型
const char * EOperatorStr[otVoid + 1] =
{
    "LBra",
    "Call",
    "Comma",
    "SgPlu",
    "SgMin",
    "Plu",
    "Min",
    "Mul",
    "Div",
    "Mod",
    "More",
    "Less",
    "MrEqu",
    "LsEqu",
    "NoEqu",
    "Equ",
    "Or",
    "And",
    "Not",
    "Asign",
    "Ret",
    "Goto",
    "GoNot",
    "Void"
};
const char * EOperatorSign[otVoid + 1] =
{
    "(",
    "cal",
    ",",
    "+",
    "-",
    "+",
    "-",
    "*",
    "/",
    "%",
    ">",
    "<",
    ">=",
    "<=",
    "!=",
    "==",
    "||",
    "&&",
    "!",
    "=",
    "ret",
    "jmp",
    "jnt",
    ""
};

//函数的语法结构类型
const char * EStatementStr[ftError + 1] = 
{
    "If",
    "For",
    "While",
    "DoWhile",
    "Switch",
    "Error"
};

