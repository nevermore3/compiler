//符号表
#include "SCSymbolTable.h"


//------------------------------------------------------
// 表达式运算符栈
//------------------------------------------------------

//添加一条运算符
CSCOperator &CSCOperatorStack::AddOper(const EExprElementType type,
    const CSCLexicaler &lex)
{
    Add();
    CSCOperator &oper = Last();
    oper.type = type;
    oper.lin = lex.lin;
    oper.col = lex.col - 1;
    return oper;
}

//------------------------------------------------------
// 表达式符号
//------------------------------------------------------

//是不是整数
Bool CSCCodeSymbol::IsInt()
{
    return ((type == stVarInteger) || (type == stConstInteger));
}

//是不是字符串
Bool CSCCodeSymbol::IsStr()
{
    return ((type == stVarString) || (type == stConstString));
}

//是不是布尔值
Bool CSCCodeSymbol::IsBool()
{
    return ((type == stVarBoolean) || (type == stConstBoolean));
}

//是不是变量
Bool CSCCodeSymbol::IsVar()
{
    return ((type == stVarInteger) || (type == stVarString) || 
            (type == stVarBoolean));
}

//返回Var类型
ESymbolType CSCCodeSymbol::VarType()
{
    switch (type)
    {
    case stConstInteger:
        return stVarInteger;
    case stConstString:
        return stVarString;
    case stConstBoolean:
        return stVarBoolean;
    case stFunction:
        return stVoid;
    default:
        return type;
    }
}

//用于调试表达式符号
void CSCCodeSymbol::Debug() const
{
    if (symbol == NULL)
    {
        //全局常量或常数
        switch (type)
        {
        case stConstInteger : //整数常量
            printf("%d", value);
            break;
        case stConstString  : //字符串常量
            printf("\"%s\"", str.S());
            break;
        case stConstBoolean : //布尔值常量
            printf("%s", (value ? "true" : "false"));
            break;
        case stVarInteger   : //整数临时变量
        case stVarString    : //字符临时串变量
        case stVarBoolean   : //布尔临时值变量
            printf("L%d:%s:%s", value, ESymbolStr[type], str.S());
            break;
        case stVoid         : //未指定
            break;
        default:
            printf("BadConst[%s]", ESymbolStr[type]);
            break;
        }
    }
    else
    {
        symbol->Debug(False);
    }
}


//------------------------------------------------------
// 表达式符号栈
//------------------------------------------------------

//添加一条符号
CSCCodeSymbol &CSCSymbolStack::AddSymb()
{
    Add();
    return Last();
}


//------------------------------------------------------
// 中间代码(逆波兰式)
//------------------------------------------------------

//用于调试中间代码
void CSCCode::Debug() const
{
    printf("[%d-%d]%s[%s],\t", lin + 1, col + 1,
        EOperatorStr[oper], EOperatorSign[oper]);
    symb1.Debug();
    printf(",\t");
    symb2.Debug();
    printf(",\t");
    result.Debug();
    printf("\r\n");
}


//------------------------------------------------------
// 中间代码表(逆波兰式表)
//------------------------------------------------------

//添加一条指令
CSCCode &CSCCodeTable::AddCode(const EOperatorType oper)
{
    Add();
    CSCCode &code = Last();
    code.oper = oper;
    return code;
}

//添加一条指令
CSCCode &CSCCodeTable::AddCode(const EOperatorType oper,
    const CSCLexicaler &lex)
{
    CSCCode &code = AddCode(oper);
    code.lin = lex.lin;
    code.col = lex.col;
    return code;
}
    
//添加一条指令
CSCCode &CSCCodeTable::AddCode(const CSCOperator &oper)
{
    CSCCode &code = AddCode(oper.oper);
    code.lin = oper.lin;
    code.col = oper.col;
    return code;
}

//用于调试中间代码表
void CSCCodeTable::Debug() const
{
    //printf("CSCCodeTable : %d\r\n", GetCount());
    for (int i = 0; i < GetCount(); i++)
    {
        printf("       %.03d. ", i);
        Get(i).Debug();
    }
    //printf("\r\n");
}


//------------------------------------------------------
// 函数语法结构分析栈
//------------------------------------------------------

//添加一条函数语法结构
CSCState &CSCStateStack::AddState(const EStatementType type, const CSCLexicaler &lex)
{
    Add();
    CSCState &state = Last();
    state.type = type;
    state.lin = lex.lin;
    state.col = lex.col;
    return state;
}
