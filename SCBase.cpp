//����������
#include "SCBase.h"

//�Ǻ�����
const char * ETokenStr[ttError + 1] =
{
    "ttInteger",
    "ttString",
    "ttOperator",
    "ttKeyword",
    "ttWord",
    "ttError"
};

//�ַ�����
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

//��������
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

//���ʽ��Ԫ������
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

//���ʽ�����������
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

//�������﷨�ṹ����
const char * EStatementStr[ftError + 1] = 
{
    "If",
    "For",
    "While",
    "DoWhile",
    "Switch",
    "Error"
};

