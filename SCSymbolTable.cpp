//符号表，是语法分析的成员
#include "SCSymbolTable.h"


//------------------------------------------------------
// 符号的定义
//------------------------------------------------------

//初始化
void CSCSymbol::Init(const char *_name, const ESymbolType _type)
{
    //首先清空原有内存
    Clear();
    
    //然后赋值
    name = _name;
    type = _type;
    
    //为字符串申请内存
    if ((type == stVarString) || (type == stConstString))
    {
        value = (int) new CNString;
    }
    
    //为函数申请内存
    if (type == stFunction)
    {
        value = (int) new CSCFunction;
        func()->symbols.AddSymb("@retrun", stVoid);
        func()->symbols[0].lin = lin;
        func()->symbols[0].col = col;
        func()->name = name;
    }
}

//清空内存
void CSCSymbol::Clear()
{
    //为字符串释放内存
    if ((value != 0) && ((type == stVarString) || (type == stConstString)))
    {
        delete str();
        value = 0;
    }
    
    //为函数释放内存
    if ((value != 0) && (type == stFunction))
    {
        delete func();
        value = 0;
    }
}

//用于调试
void CSCSymbol::Debug(const Bool showdetail)
{
    if (showdetail)
    {
        //详细打印
        printf("[%d-%d]-%s:", lin + 1, col + 1, ESymbolStr[type]);
        switch (type)
        {
        case stVarString:
        case stConstString:
            printf("%s=\"%s\"", name.S(), str()->S());
            break;
        case stVarBoolean:
        case stConstBoolean:
            printf("%s=%s", name.S(), (value ? "true" : "false"));
            break;
        case stFunction:
            printf("%s()", name.S());
            func()->Debug();
            break;
        default:
            printf("%s=%d", name.S(), value);
            break;
        }
    }
    else
    {
        //简单打印
        switch (type)
        {
        case stVarInteger:
        case stVarString:
        case stVarBoolean:
            printf("%s:%s", ESymbolStr[type], name.S());
            break;
        case stConstInteger:
            printf("%d", value);
            break;
        case stConstString:
            printf("\"%s\"", str()->S());
            break;
        case stConstBoolean:
            printf("%s", (value ? "true" : "false"));
            break;
        case stFunction:
            printf("%s()", name.S());
        }
    }
}

//用于调试
void CSCFunction::Debug()
{
    //打印返回值
    printf("\r\n       return : ");
    symbols[0].Debug();
    printf("\r\n");

    //打印参数
    for (int i = 1; i <= paras; i++)
    {
        printf("       para %d : ", i);
        symbols[i].Debug();
        printf("\r\n");
    }

    //打印局部变量
    for (; i < symbols.GetCount(); i++)
    {
        printf("    loc_var %d : ", i);
        symbols[i].Debug();
        printf("\r\n");
    }

    //打印中间代码
    if (syscall)
    {
        printf("       system call\r\n");
    }
    else
    {
        printf("       codes  :\r\n");
        codes.Debug();
    }
}


//------------------------------------------------------
// 符号表
//------------------------------------------------------

//添加一个符号
CSCSymbol &CSCSymbolTable::AddSymb(CSCSymbol &_symb)
{
    Add();
    CSCSymbol &symb = Last();
    symb.Init(_symb.name, _symb.type);
    symb.lin = _symb.lin;
    symb.col = _symb.col;
    return symb;
}

//添加一个符号
CSCSymbol &CSCSymbolTable::AddSymb(const char *name, const ESymbolType type)
{
    Add();
    CSCSymbol &symb = Last();
    symb.Init(name, type);
    symb.lin = 0;
    symb.col = 0;
    return symb;
}

//添加一个符号
CSCSymbol &CSCSymbolTable::AddSymb(const char *name, 
    const ESymbolType type, const CSCLexicaler &lex)
{
    CSCSymbol &symb = AddSymb(name, type);
    symb.lin = lex.lin;
    symb.col = lex.col;
    if (type == stFunction)
    {
        symb.func()->symbols[0].lin = symb.lin;
        symb.func()->symbols[0].col = symb.col;
    }
    return symb;
}

//根据名称查找符号，失败返回0
CSCSymbol *CSCSymbolTable::Lookup(const char *name)
{
    CSCSymbol symbol;
    symbol.Init(name, stVoid);
    int index = IndexOf(symbol);
    if (index == -1)
    {
        return NULL;
    }
    else
    {
        return m_List[index];
    }
}

//用于调试，showsys定时是否显示系统函数与变量
void CSCSymbolTable::Debug(const Bool showsyscall)
{
    printf("CSCSymbolTable : %d\r\n", GetCount());
    for (int i = 0; i < GetCount(); i++)
    {
        if ((!showsyscall) && (Get(i).type == stFunction) &&
            (Get(i).func()->syscall))
        {
            continue;
        }
        printf("  %d. ", i + 1);
        Get(i).Debug();
        printf("\r\n");
    }
    printf("\r\n");
}

