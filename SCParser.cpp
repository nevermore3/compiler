//语法分析器
#include "SCParser.h"


//------------------------------------------------------
// 语法分析的局部宏
//------------------------------------------------------

//获得下一个记号的宏
#define GET_NEXT_TOKEN \
    if (lex.GetNextToken() != M_SC_POK) \
    { \
        lex.PrintError(201, "unexpect file end or file error!"); \
        return M_SC_ERR; \
    }

//判断是需要的记号的宏
#define TkIS(t) (lex.token.str == t)

//判断不是需要的记号的宏
#define TkNOT(t) (lex.token.str != t)

//判断是不是需要的记号的宏
#define EXPECT_TOKEN(t1) \
    if TkNOT(t1) \
    { \
        lex.PrintError(202, "expect token \"%s\"!", t1); \
        return M_SC_ERR; \
    }
#define EXPECT_TOKEN2(t1, t2) \
    if (TkNOT(t1) && TkNOT(t2)) \
    { \
        lex.PrintError(202, "expect token \"%s|%s\"!", t1, t2); \
        return M_SC_ERR; \
    }
#define EXPECT_TOKEN3(t1, t2, t3) \
    if (TkNOT(t1) && TkNOT(t2) && TkNOT(t3)) \
    { \
        lex.PrintError(202, "expect token \"%s|%s|%s\"!", t1, t2, t3); \
        return M_SC_ERR; \
    }
#define EXPECT_TOKEN4(t1, t2, t3, t4) \
    if (TkNOT(t1) && TkNOT(t2) && TkNOT(t3) && TkNOT(t4)) \
    { \
        lex.PrintError(202, "expect token \"%s|%s|%s|%s\"!", t1, t2, t3, t4); \
        return M_SC_ERR; \
    }

//获得下一个记号并判断是不是需要的记号的宏
#define GET_EXPECT_TOKEN(t1) \
    GET_NEXT_TOKEN \
    EXPECT_TOKEN(t1)
#define GET_EXPECT_TOKEN2(t1, t2) \
    GET_NEXT_TOKEN \
    EXPECT_TOKEN2(t1, t2)
#define GET_EXPECT_TOKEN3(t1, t2, t3) \
    GET_NEXT_TOKEN \
    EXPECT_TOKEN3(t1, t2, t3)

//表达式的宏
#define EXPRESSION(type) \
    expr.parser = this; \
    if (expr.Calc(type) != M_SC_OK) \
    { \
        return M_SC_ERR; \
    }

//表达式结果
#define EXPR_RESULT (expr.symbolstk[0])

//语法结构里的代码块
#define STATEMENT_GROUP \
    if (StatementGroup() != M_SC_OK) \
    { \
        return M_SC_ERR; \
    }


//------------------------------------------------------
// 语法分析类
//------------------------------------------------------

//执行语法分析，CSCParser为唯一入口函数
int CSCParser::Parse(const char *filename)
{
    //返回结果
    func = symbols->Get(0).func();

    //词法分析器词法分析器加载文件
    if (lex.LoadScript(filename) != M_SC_OK)
    {
        return M_SC_ERR;
    }

    //依次读取记号，成功返回M_SC_POK，失败返回M_SC_ERR，文件结束返回M_SC_OK
    while (lex.GetNextToken() == M_SC_POK)
    {
        //处理当前语法块，可能是全局变量、常量或者函数
        if (Block() != M_SC_OK)
        {
            return M_SC_ERR;
        }
    }

    //返回结果
    if (lex.errors > 0)
    {
        printf("\r\nParse \"%s\" find %d error(s)\r\n\r\n", filename, lex.errors);
        return M_SC_ERR;
    }
    else
    {
        return M_SC_OK;
    }
}


//------------------------------------------------------
// Parse的私有函数，用来处理各种Block
//------------------------------------------------------

//处理当前语法块，可能是全局变量、常量或者函数
int CSCParser::Block()
{
    //语法定义：
    //全局变量：(int|string|boolean) var_name[ = expression];
    //全局常量：const (int|string|boolean) const_name = expression;
    //函    数：(void|数据类型) func_name(数据类型 para_name, ...)(;|{函数体})

    //首先申请一个symbol用来保存当前分析的符号
    symbol = new CSCSymbol;

    //判断是不是常量
    if (lex.token.str == "const")
    {
        //启动常量的语法制导
        if (Const() != M_SC_OK)
        {
            return M_SC_ERR;
        }
    }
    else
    {
        //启动变量或函数的语法制导
        if (VarOrFunction() != M_SC_OK)
        {
            return M_SC_ERR;
        }
    }

    //添加符号，返回结果
    symbols->Add(symbol);
    return M_SC_OK;
}

//分析全局常量，Block的私有函数
int CSCParser::Const()
{
    //语法定义：
    //全局常量：const (int|string|boolean) const_name = expression;

    //获得数据类型：(int|string|boolean)
    GET_EXPECT_TOKEN3("int", "string", "boolean");
    if (lex.token.str == "int")
    {
        symbol->Init("", stConstInteger);
    }
    else
    if (lex.token.str == "string")
    {
        symbol->Init("", stConstString);
    }
    else
    {
        symbol->Init("", stConstBoolean);
    }

    //获得常量名：ConstName
    if (SymbolGetName(symbol) != M_SC_OK)
    {
        return M_SC_ERR;
    }

    //获得等号：=
    GET_EXPECT_TOKEN("=");

    //获得默认值表达式：Expression
    GET_NEXT_TOKEN;
    EXPRESSION(symbol->type);

    //添加默认值
    if (symbol->type == stConstString)
    {
        *(symbol->str()) = EXPR_RESULT.str;
    }
    else
    {
        symbol->value = EXPR_RESULT.value;
    }
        
    //获得末尾的分号
    EXPECT_TOKEN(";");

    //成功返回结果
    return M_SC_OK;
}

//分析全局变量或函数，Block的私有函数
int CSCParser::VarOrFunction()
{
    //语法定义：
    //全局变量：(int|string|boolean) var_name[ = expression];
    //函    数：(void|数据类型) func_name(数据类型 para_name, ...)(;|{函数体})

    //获得数据类型：(void|int|string|boolean)
    EXPECT_TOKEN4("void", "int", "string", "boolean");
    if (lex.token.str == "void")
    {
        symbol->Init("", stFunction);
    }
    else
    if (lex.token.str == "int")
    {
        symbol->Init("", stVarInteger);
    }
    else
    if (lex.token.str == "string")
    {
        symbol->Init("", stVarString);
    }
    else
    {
        symbol->Init("", stVarBoolean);
    }

    //获得变量名或函数名：(VarName|FuncName)
    if (SymbolGetName(symbol) != M_SC_OK)
    {
        return M_SC_ERR;
    }
    if (symbol->type == stFunction)
    {
        symbol->func()->name = symbol->name;
    }

    //获得下一个字符：(=|;|()
    GET_EXPECT_TOKEN3("=", ";", "(");
    if (lex.token.str == "=")
    {
        //有默认值全局变量，获得默认值表达式：Expression
        return VarAssign(symbol, -1);
    }
    else
    if (lex.token.str == ";")
    {
        //无默认值全局变量，直接返回
        return M_SC_OK;
    }
    else
    {
        //函数，调用函数语法制导程序
        if (FunctionDefine() != M_SC_OK)
        {
            return M_SC_ERR;
        }

        //分析完函数，就置回全局入口函数
        func = symbols->Get(0).func();
        return M_SC_OK;
    }
}


//------------------------------------------------------
// VarOrFunction的私有函数，用来分析函数
//------------------------------------------------------

//分析函数定义，VarOrFunction的私有函数
int CSCParser::FunctionDefine()
{
    //语法定义：
    //函    数：(void|数据类型) func_name(数据类型 para_name, ...)(;|{函数体})
    ESymbolType type;   //函数返回值类型
    CNString str;       //参数的名称
    CSCSymbol *parasymb;//参数的符号
    
    //首先初始化为函数，并添加返回值到函数的符号表的首位
    if (symbol->type != stFunction)
    {
        type = symbol->type;
        symbol->Init(symbol->name, stFunction);
        symbol->func()->symbols[0].Init("@return", type);
    }
    else
    {
        symbol->func()->symbols[0].lin = symbol->lin;
        symbol->func()->symbols[0].col = symbol->col;
    }
    func = symbol->func();
    
    //判断下一个符号是不是参数
    GET_NEXT_TOKEN;
    while (lex.token.str != ")")
    {
        //参数的数据类型：(int|string|boolean)
        EXPECT_TOKEN3("int", "string", "boolean");

        //创建参数
        parasymb = new CSCSymbol;

        //参数类型
        if (lex.token.str == "int")
        {
            parasymb->Init("", stVarInteger);
        }
        else
        if (lex.token.str == "string")
        {
            parasymb->Init("", stVarString);
        }
        else
        {
            parasymb->Init("", stVarBoolean);
        }
        
        //参数的名称：ParaName
        if (SymbolGetName(parasymb) != M_SC_OK)
        {
            delete parasymb;
            return M_SC_ERR;
        }

        //添加到列表
        func->symbols.Add(parasymb);
        func->paras++;

        //获得参数末尾的
        GET_EXPECT_TOKEN2(")", ",");
        if (lex.token.str == ",")
        {
            GET_NEXT_TOKEN;
        }
        else
        {
            break;
        }
    }

    //获得函数体：(;|{函数体})
    GET_EXPECT_TOKEN2(";", "{");
    if (lex.token.str == ";")
    {
        //函数的引用定义，直接返回即可
        func->syscall = True;
        return M_SC_OK;
    }

    //返回函数体的编译结果
    return FunctionBody();
}

//分析函数体，VarOrFunction的私有函数
int CSCParser::FunctionBody()
{
    //初始化函数语法结构分析栈
    statestk.Clear();

    //函数体就是个大的代码段
    if (StatementGroup() != M_SC_OK)
    {
        return M_SC_ERR;
    }

    //分析编译结果
    if (statestk.GetCount() != 0)
    {
        lex.PrintError(205, "line: %d, statement \"%s\" uncomplete",
            statestk.Last().lin + 1, EStatementStr[statestk.Last().type]);
        return M_SC_ERR;
    }
        
    //处理返回值，但这个判断不够完备，凑合用
    if ((func->symbols[0].type != stVoid) &&
        ((func->codes.GetCount() == 0) || (func->codes.Last().oper != otReturn)))
    {
        lex.PrintError(206, "not all control paths return a value");
        return M_SC_ERR;
    }

    //追加一条返回指令，避免跳转到函数尾失败
    func->codes.AddCode(otReturn, lex);
    return M_SC_OK;
}


//------------------------------------------------------
// 符号(常变量/函数名)的各种处理
//------------------------------------------------------

//查找现有符号，各处均可被调用
EExprElementType CSCParser::SymbolLookup()
{
    //判断是不是函数的局部变量，优先判定为局部变量
    tempsymbol = func->symbols.Lookup(lex.token.str);
    if (tempsymbol != NULL)
    {
        switch (tempsymbol->type)
        {
        case stVarInteger:
        case stVarString:
        case stVarBoolean:
            return etFuncVarName;
        }
    }

    //判断全局变量或函数
    tempsymbol = symbols->Lookup(lex.token.str);
    if (tempsymbol != NULL)
    {
        switch (tempsymbol->type)
        {
        case stVarInteger:
        case stVarString:
        case stVarBoolean:
            return etVarName;
        case stConstInteger:
        case stConstString:
        case stConstBoolean:
            return etConstName;
        case stFunction:
            return etFuncName;
        }
    }

    //找不到，返回失败
    return etError;
}

//从lex获得符号名
int CSCParser::SymbolGetName(CSCSymbol *symb)
{
    //获得符号名称
    GET_NEXT_TOKEN
    if (lex.token.type != ttWord)
    {
        lex.PrintError(203, "\"%s\" is a invalid name!", 
            lex.token.str.S());
        return M_SC_ERR;
    }

    //保存符号位置信息
    symb->lin = lex.token.lin;
    symb->col = lex.token.col;
    symb->name = lex.token.str;
    
    //判断符号是否重名
    EExprElementType type = SymbolLookup();
    if (func == symbols->Get(0).func())
    {
        //全局符号
        if (type == etError)
        {
            return M_SC_OK;
        }
    }
    else
    {
        //函数的局部符号
        if ((type != etFuncVarName) && (type != etFuncName))
        {
            return M_SC_OK;
        }
    }
    lex.PrintError(204, "\"%s\" repeat to Line:%d, Col:%d!",
        lex.token.str.S(), tempsymbol->lin + 1, tempsymbol->col + 1);
    return M_SC_ERR;
}


//------------------------------------------------------
// 变量处理，尤其是临时变量
//------------------------------------------------------

//变量赋值语句，通用函数，-1表示全局变量，否则为局部变量下标
int CSCParser::VarAssign(CSCSymbol *symb, const int value)
{
    //表达式解析
    int lin = lex.lin;
    int col = lex.col;
    GET_NEXT_TOKEN;
    EXPRESSION(symb->type);

    //增加一条赋值到该变量的指令
    CSCCode &code = func->codes.AddCode(otAssign);
    code.lin = lin;
    code.col = col;
    code.symb1 = EXPR_RESULT;
    code.result.type = symb->type;
    if (value == -1)
    {
        //全局变量
        code.result.symbol = symb;
    }
    else
    {
        //局部变量
        code.result.value = value;
        code.result.str = symb->name;
    }

    //表达式末尾后;
    EXPECT_TOKEN(";");
    
    //成功返回结果
    return M_SC_OK;
}

//申请临时变量，value是临时数组下表，lin是占用标志
void CSCParser::VarAllocTemp(const ESymbolType type, CSCCodeSymbol &symb)
{
    //局部变量定义
    CNString name;  //临时变量名
    int i;          //通用循环变量
    CSCSymbol *temp;//当前临时变量

    //为临时变量起个名字，只是便于调试
    switch (type)
    {
    case stVarInteger:
        name = "@i";
        break;
    case stVarString:
        name = "@s";
        break;
    case stVarBoolean:
        name = "@b";
        break;
    default:    
        symb.type = stVoid;
        return;
    }

    //初始化临时变量
    symb.type = type;
    symb.symbol = NULL;

    //搜索已释放临时变量，失败则申请新的
    for (i = 1; ;i++)
    {
        temp = func->symbols.Lookup(name + i);
        if (temp == NULL)
        {
            //没有这个临时变量，就申请一个新的
            temp = &(func->symbols.AddSymb(name + i, type));
            temp->lin = M_SC_USE;
            symb.value = func->symbols.GetCount() - 1;
            symb.str = temp->name;
            return;
        }
        else
        if (temp->lin == M_SC_FREE)
        {
            temp->lin = M_SC_USE;
            symb.value = func->symbols.IndexOf(*temp);
            symb.str = temp->name;
            return;
        }
    }
}

//释放临时变量
void CSCParser::VarFreeTemp(const CSCCodeSymbol &symb)
{
    //入口判断，不能是全局变量或函数
    if (symb.symbol != NULL)
    {
        return;
    }

    //释放临时变量
    if ((symb.type == stVarInteger) ||
        (symb.type == stVarString) ||
        (symb.type == stVarBoolean))
    {
        func->symbols[symb.value].lin = M_SC_FREE;
    }
}

//局部变量定义
int CSCParser::VarLocalDefine()
{
    //语法定义：
    //局部变量：(int|string|boolean) var_name[ = expression];

    //首先申请一个symbol用来保存当前分析的符号
    CSCSymbol *locsymb = new CSCSymbol;

    //获得数据类型：(int|string|boolean)
    if (lex.token.str == "int")
    {
        locsymb->Init("", stVarInteger);
    }
    else
    if (lex.token.str == "string")
    {
        locsymb->Init("", stVarString);
    }
    else
    {
        locsymb->Init("", stVarBoolean);
    }

    //获得变量名或函数名：(VarName)
    if (SymbolGetName(locsymb) != M_SC_OK)
    {
        delete locsymb;
        return M_SC_ERR;
    }

    //添加到列表
    func->symbols.Add(locsymb);
    
    //获得下一个字符：(=|;)
    GET_EXPECT_TOKEN2("=", ";");
    if (lex.token.str == "=")
    {
        //有默认值局部变量，获得默认值表达式：Expression
        if (VarAssign(locsymb, func->symbols.GetCount() - 1) != M_SC_OK)
        {
            return M_SC_ERR;
        }
    }

    //成功返回结果
    return M_SC_OK;
}


//------------------------------------------------------
// 编译语句组和语句
//------------------------------------------------------

//编译语句组
int CSCParser::StatementGroup()
{
    //依次读取记号，成功返回M_SC_POK，失败返回M_SC_ERR，文件结束返回M_SC_OK
    int result;
    while ((result = lex.GetNextToken()) == M_SC_POK)
    {
        //处理当前语法块，可能是局部变量、函数或if/while/for/switch
        switch (Statement())
        {
        case M_SC_ERR: //出错
            return M_SC_ERR;
        case M_SC_POK: //遇到"}"成功返回
            return M_SC_OK;
        }
    }

    //判断是不是代码分析完了也找不到"}"
    if (result == M_SC_OK)
    {
        lex.PrintError(232, "state need \"}\" for end");
    }

    //走到这里肯定是出错了
    return M_SC_ERR;
}

//编译一条语句
int CSCParser::Statement()
{
    //分析起始符
    if (TkIS("}"))
    {
        //遇到"}"成功返回
        return M_SC_POK;
    }
    else
    if (TkIS("int") || TkIS("string") || TkIS("boolean"))
    {
        //局部变量定义
        return VarLocalDefine();
    }
    else
    if (TkIS("return"))
    {
        //函数返回
        return KeyReturn();
    }
    else
    if (TkIS("if"))
    {
        //if条件语句
        return StatementIf();
    }
    else
    if (TkIS("for"))
    {
        //for循环语句
        return StatementFor();
    }
    else
    if (TkIS("while"))
    {
        //while循环语句
        return StatementWhile();
    }
    else
    if (TkIS("do"))
    {
        //do...while循环语句
        return StatementDoWhile();
    }
    else
    if (TkIS("switch"))
    {
        //switch分支语句
        return StatementSwitch();
    }
    else
    if (TkIS("break"))
    {
        //Break语句给for/while/switch
        return KeyBreak();
    }
    else
    if (TkIS("continue"))
    {
        //Continue语句给for/while
        return KeyContinue();
    }
    else
    if (TkIS("case"))
    {
        //Case语句给while
        return KeyCase();
    }
    else
    if (TkIS("default"))
    {
        //Default语句给while
        return KeyDefault();
    }
    else
    {
        switch (SymbolLookup())
        {
            case etVarName    : //全局变量名
                GET_EXPECT_TOKEN("=")
                return VarAssign(tempsymbol, -1);
            case etFuncVarName: //函数参数或局部变量名
                GET_EXPECT_TOKEN("=")
                return VarAssign(tempsymbol, 
                    func->symbols.IndexOf(*tempsymbol));
            case etFuncName   : //函数名
                //函数调用
                EXPRESSION(stFunction);
                EXPECT_TOKEN(";");
                return M_SC_OK;
        }

        //分析失败
        lex.PrintError(230, "unexpect word \"%s\"", lex.token.str.S());
        return M_SC_ERR;
    }
}

//查找五大语句定义
int CSCParser::StatementLookup(const char *type)
{
    Bool find;
    for (int i = statestk.GetCount() - 1; i >= 0; i--)
    {
        if (!strcmp(type, "break"))
        {
            find = (statestk[i].type != ftIf);
        }
        else
        if (!strcmp(type, "continue"))
        {
            find = ((statestk[i].type != ftIf) && (statestk[i].type != ftSwitch));
        }
        else
        {
            find = (statestk[i].type == ftSwitch);
        }
        if (find)
        {
            return i;
        }
    }
    return -1;
}

//添加五大语句到分析栈
CSCState &CSCParser::StatementAdd(const EStatementType type)
{
    //语法结构进栈
    CSCState &state = statestk.AddState(type, lex);

    //添加入口命令
    
    //001  goto (003)
    state.breakpos = func->codes.GetCount() + 1;
    CodeAddGoto(state.breakpos + 1);
    
    //002  goto (breakpos:012)
    CodeAddGoto(0);

    //返回结果
    return state;
}


//------------------------------------------------------
// 分析五大语句，Statement的私有函数
//------------------------------------------------------

//if条件语句
int CSCParser::StatementIf()
{
    //语法定义：
    /*
    if (bool_expression) 
    { 
        statement 
    }
    [else [if (bool_expression)] 
    { 
        statement 
    }]
    */
    CSCState &state = StatementAdd(ftIf);
    
    //分析: if (bool_expression)
    GET_EXPECT_TOKEN("(");
    EXPRESSION(stVarBoolean)
    state.contipos = func->codes.GetCount();
    CodeAddGoNot(0);

    //分析正确的条件分支
    EXPECT_TOKEN("{")
    STATEMENT_GROUP;
    CodeAddGoto(state.breakpos);

    //else分支
    GET_NEXT_TOKEN
    while (TkIS("else"))
    {
        GET_EXPECT_TOKEN2("if", "{")
        CodeSetGoNot(state.contipos);
        if (TkIS("if"))
        {
            //else if条件分支
            GET_EXPECT_TOKEN("(");
            EXPRESSION(stVarBoolean)
            state.contipos = func->codes.GetCount();
            CodeAddGoNot(0);

            //分析正确的条件分支
            EXPECT_TOKEN("{")
            STATEMENT_GROUP;
            CodeAddGoto(state.breakpos);
            GET_NEXT_TOKEN
        }
        else
        {
            //纯else条件分支
            EXPECT_TOKEN("{")
            STATEMENT_GROUP;
            
            //删除语法结构并成功返回
            CodeAddBreak();
            return M_SC_OK;
        }
    }
    
    //分析到这里肯定是多取了下一个结构的token，直接运算
    CodeSetGoNot(state.contipos);
    CodeAddBreak();
    return Statement();
}

//for循环语句
int CSCParser::StatementFor()
{
    //语法定义：
    /*
    for ([var_1 = int_expr]; [bool_expr]; [var_2(++|--)])
    {
        statement
        break;
        continue; 
    }
    */
    CSCState &state = StatementAdd(ftFor);

    //分析for的第一个参数: 赋值[var_1 = int_expr]
    GET_EXPECT_TOKEN("(");
    GET_NEXT_TOKEN
    if (TkNOT(";"))
    {
        int pos = 0;
        switch (SymbolLookup())
        {
            case etVarName    : //全局变量名
                pos = -1;
                break;
            case etFuncVarName: //函数参数或局部变量名
                pos = func->symbols.IndexOf(*tempsymbol);
                break;
            default:
                lex.PrintError(231, "\"for\" para 1 must be empty or \"varname=expression\"");
                return M_SC_ERR;
        }
        GET_EXPECT_TOKEN("=")
        if (VarAssign(tempsymbol, pos) != M_SC_OK)
        {
            return M_SC_ERR;
        }
    }

    //跳过累加，直接进入入口判断
    CodeAddGoto(func->codes.GetCount() + 2);
    state.contipos = func->codes.GetCount();
    CSCCode &code = func->codes.AddCode(otPlu, lex);

    //分析for的第二个参数: 入口判断[bool_expr]
    GET_NEXT_TOKEN
    if (TkNOT(";"))
    {
        EXPRESSION(stVarBoolean)
        CodeAddGoNot(state.breakpos);
    }
    
    //分析for的第三个参数: 累加累减[var_2(++|--)]
    GET_NEXT_TOKEN
    if (TkNOT(")"))
    {
        //查找循环变量
        switch (SymbolLookup())
        {
            case etVarName    : //全局变量名
                code.symb1.symbol = tempsymbol;
                break;
            case etFuncVarName: //函数参数或局部变量名
                code.symb1.value = func->symbols.IndexOf(*tempsymbol);
                code.symb1.str = tempsymbol->name;
                break;
            default:
                lex.PrintError(232, "\"for\" para 3 must be \"[varname(++|--)]\"");
                return M_SC_ERR;
        }
        code.symb1.type = tempsymbol->type;
        code.result = code.symb1;

        //分析累加类型
        code.symb2.type = stConstInteger;
        code.symb2.value = 1;
        GET_EXPECT_TOKEN2("+", "-")
        int lin = lex.lin;
        int col = lex.col;
        if (TkIS("+"))
        {
            GET_EXPECT_TOKEN("+")
        }
        else
        {
            code.oper = otMin;
            GET_EXPECT_TOKEN("-")
        }
        if ((lex.lin != lin) || (lex.col != col + 1))
        {
            lex.PrintError(232, "\"for\" para 3 must be \"[varname(++|--)]\"");
            return M_SC_ERR;
        }
        GET_EXPECT_TOKEN(")")
    }
    else
    {
        code.oper = otVoid;
    }
    
    //分析语句代码块
    GET_EXPECT_TOKEN("{")
    STATEMENT_GROUP;
    CodeAddGoto(state.contipos);
    
    //删除函数分析结构后成功返回
    CodeAddBreak();
    return M_SC_OK;
}

//while循环语句
int CSCParser::StatementWhile()
{
    //语法定义：
    /*
    while (bool_expression)
    {
        statement
        break;
        continue; 
    }
    */
    CSCState &state = StatementAdd(ftWhile);

    //分析: while (bool_expression)
    GET_EXPECT_TOKEN("(");
    state.contipos = func->codes.GetCount();
    EXPRESSION(stVarBoolean)
    CodeAddGoNot(state.breakpos);

    //分析语句代码块
    EXPECT_TOKEN("{")
    STATEMENT_GROUP;
    CodeAddGoto(state.contipos);
    
    //删除函数分析结构后成功返回
    CodeAddBreak();
    return M_SC_OK;
}

//do...while循环语句
int CSCParser::StatementDoWhile()
{
    //语法定义：
    /*
    do
    {
        statement
        break;
        continue; 
    } while (bool_expression);
    */
    CSCState &state = StatementAdd(ftDoWhile);

    //分析语句代码块
    GET_EXPECT_TOKEN("{")
    state.contipos = func->codes.GetCount();
    STATEMENT_GROUP;
    
    //分析: while (bool_expression);
    GET_EXPECT_TOKEN("while");
    GET_EXPECT_TOKEN("(");
    EXPRESSION(stVarBoolean)
    CodeAddGoNot(func->codes.GetCount() + 2);
    CodeAddGoto(state.contipos);
    
    //删除函数分析结构后成功返回
    CodeAddBreak();
    return M_SC_OK;
}

//switch分支语句
int CSCParser::StatementSwitch()
{
    //语法定义：
    /*
    switch (int_var_name) {
        case const_int_expression: 
            statement
            break;
        default:
            statement
            [break;]
    }
    */
    CSCState &state = StatementAdd(ftSwitch);

    //添加一个分支比较指令
    CSCCode &code = func->codes.AddCode(otEqu, lex);
    code.symb2.type = stConstInteger;
    VarAllocTemp(stVarBoolean, code.result);
    
    //分析枚举值switch (int_var_name)
    GET_EXPECT_TOKEN("(")
    GET_NEXT_TOKEN
    EExprElementType type = SymbolLookup();
    if (((type == etVarName) || (type == etFuncVarName)) &&
        (tempsymbol->type == stVarInteger))
    {
        code.symb1.type = tempsymbol->type;
        if (type == etVarName)
        {
            code.symb1.symbol = tempsymbol;
        }
        else
        {
            code.symb1.value = func->symbols.IndexOf(*tempsymbol);
            code.symb1.str = tempsymbol->name;
        }
    }
    else
    {
        lex.PrintError(233, "\"switch\" value must be a integer variable");
        return M_SC_ERR;
    }
    GET_EXPECT_TOKEN(")")

    //分析语句代码块
    GET_EXPECT_TOKEN("{")
    STATEMENT_GROUP;
    if (state.contipos == 0)
    {
        lex.PrintError(234, "\"switch\" need one \"case\" at least");
        return M_SC_ERR;
    }
    
    //删除函数分析结构后成功返回
    CodeAddBreak();
    VarFreeTemp(code.result);
    return M_SC_OK;
}


//------------------------------------------------------
// 分析五大语句的关键字
//------------------------------------------------------

//退出函数
int CSCParser::KeyReturn()
{
    //判断函数有没有返回值
    int lin = lex.lin;
    int col = lex.col;
    if (func->symbols[0].type == stVoid)
    {
        //没有返回值
        GET_NEXT_TOKEN
        if (TkNOT(";"))
        {
            lex.PrintError(231, "void function can't return value");
            return M_SC_ERR;
        }
    }
    else
    {
        //有返回值
        if (VarAssign(&(func->symbols[0]), 0) != M_SC_OK)
        {
            return M_SC_ERR;
        }
    }

    //增加退出函数的指令
    CSCCode &code = func->codes.AddCode(otReturn);
    code.lin = lin;
    code.col = col;
    return M_SC_OK;
}

//退出五大语句
int CSCParser::KeyBreak()
{
    //查找Break哪个循环
    int pos = StatementLookup("break");
    if (pos == -1)
    {
        lex.PrintError(234, "\"break\" without \"for/while/switch\"");
        return M_SC_ERR;
    }

    //添加Break指令
    CodeAddGoto(statestk[pos].breakpos);
    
    //成功返回
    GET_EXPECT_TOKEN(";")
    return M_SC_OK;
}

//继续循环五大语句
int CSCParser::KeyContinue()
{
    //查找Continue哪个循环
    int pos = StatementLookup("continue");
    if (pos == -1)
    {
        lex.PrintError(234, "\"continue\" without \"for/while\"");
        return M_SC_ERR;
    }

    //添加Continue指令
    CodeAddGoto(statestk[pos].contipos);
    
    //成功返回
    GET_EXPECT_TOKEN(";")
    return M_SC_OK;
}

//Switch的分支
int CSCParser::KeyCase()
{
    //查找Case哪个Switch
    int pos = StatementLookup("case");
    if (pos == -1)
    {
        lex.PrintError(234, "\"case\" without \"switch\"");
        return M_SC_ERR;
    }

    //入口条件判断
    if (statestk[pos].contipos == -1)
    {
        lex.PrintError(235, "\"case\" must brefore \"default\" in \"switch\"");
        return M_SC_ERR;
    }
    else
    if ((statestk[pos].contipos == 0) &&
        (func->codes.GetCount() > statestk[pos].breakpos + 2))
    {
        lex.PrintError(236, "first of \"switch\" must be \"case\"");
        return M_SC_ERR;
    }
        
    //计算整数常量表达式
    GET_NEXT_TOKEN;
    EXPRESSION(stConstInteger);

    //添加常量比较表达式
    CSCCode *code1 = &(func->codes[statestk[pos].breakpos + 1]);    
    CSCCode *code = code1;
    if (statestk[pos].contipos > 0)
    {
        func->codes[statestk[pos].contipos].result.value = func->codes.GetCount();
        code = &(func->codes.AddCode(otEqu, lex));
        code->symb1 = code1->symb1;
        code->symb2 = code1->symb2;
        code->result = code1->result;
    }
    code->symb2.value = EXPR_RESULT.value;

    //增加跳转指令
    statestk[pos].contipos = func->codes.GetCount();
    code1 = &(func->codes.AddCode(otGoNot, lex));
    code1->symb1 = code->result;
    code1->result.type = stConstInteger;

    //成功返回
    EXPECT_TOKEN(":")
    return M_SC_OK;
}

//Switch的默认分支
int CSCParser::KeyDefault()
{
    //查找Default哪个Switch
    int pos = StatementLookup("default");
    if (pos == -1)
    {
        lex.PrintError(234, "\"default\" without \"switch\"");
        return M_SC_ERR;
    }

    //入口条件判断
    if (statestk[pos].contipos == 0)
    {
        lex.PrintError(237, "at least has one \"case\" before \"default\"");
        return M_SC_ERR;
    }
    else
    if (statestk[pos].contipos == -1)
    {
        lex.PrintError(238, "already has a \"default\" before this one");
        return M_SC_ERR;
    }

    //定义跳转位置
    func->codes[statestk[pos].contipos].result.value = func->codes.GetCount();
    statestk[pos].contipos = -1;

    //直接返回即可
    GET_EXPECT_TOKEN(":")
    return M_SC_OK;
}


//------------------------------------------------------
// 为五大语句添加中间代码
//------------------------------------------------------

//无条件跳转
void CSCParser::CodeAddGoto(const int pos)
{
    CSCCode &code = func->codes.AddCode(otGoto, lex);
    code.result.type = stConstInteger;
    code.result.value = pos;
}

//有条件跳转
void CSCParser::CodeAddGoNot(const int pos)
{
    CSCCode &code = func->codes.AddCode(otGoNot, lex);
    code.symb1 = EXPR_RESULT;
    code.result.type = stConstInteger;
    code.result.value = pos;
}

//设置有条件跳转位置
void CSCParser::CodeSetGoNot(const int pos)
{
    func->codes[pos].result.value = func->codes.GetCount();
}

//退出五大语句
void CSCParser::CodeAddBreak()
{
    CodeSetGoNot(statestk.Last().breakpos);
    statestk.DeleteLast();
}

