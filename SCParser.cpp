//�﷨������
#include "SCParser.h"


//------------------------------------------------------
// �﷨�����ľֲ���
//------------------------------------------------------

//�����һ���Ǻŵĺ�
#define GET_NEXT_TOKEN \
    if (lex.GetNextToken() != M_SC_POK) \
    { \
        lex.PrintError(201, "unexpect file end or file error!"); \
        return M_SC_ERR; \
    }

//�ж�����Ҫ�ļǺŵĺ�
#define TkIS(t) (lex.token.str == t)

//�жϲ�����Ҫ�ļǺŵĺ�
#define TkNOT(t) (lex.token.str != t)

//�ж��ǲ�����Ҫ�ļǺŵĺ�
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

//�����һ���ǺŲ��ж��ǲ�����Ҫ�ļǺŵĺ�
#define GET_EXPECT_TOKEN(t1) \
    GET_NEXT_TOKEN \
    EXPECT_TOKEN(t1)
#define GET_EXPECT_TOKEN2(t1, t2) \
    GET_NEXT_TOKEN \
    EXPECT_TOKEN2(t1, t2)
#define GET_EXPECT_TOKEN3(t1, t2, t3) \
    GET_NEXT_TOKEN \
    EXPECT_TOKEN3(t1, t2, t3)

//���ʽ�ĺ�
#define EXPRESSION(type) \
    expr.parser = this; \
    if (expr.Calc(type) != M_SC_OK) \
    { \
        return M_SC_ERR; \
    }

//���ʽ���
#define EXPR_RESULT (expr.symbolstk[0])

//�﷨�ṹ��Ĵ����
#define STATEMENT_GROUP \
    if (StatementGroup() != M_SC_OK) \
    { \
        return M_SC_ERR; \
    }


//------------------------------------------------------
// �﷨������
//------------------------------------------------------

//ִ���﷨������CSCParserΪΨһ��ں���
int CSCParser::Parse(const char *filename)
{
    //���ؽ��
    func = symbols->Get(0).func();

    //�ʷ��������ʷ������������ļ�
    if (lex.LoadScript(filename) != M_SC_OK)
    {
        return M_SC_ERR;
    }

    //���ζ�ȡ�Ǻţ��ɹ�����M_SC_POK��ʧ�ܷ���M_SC_ERR���ļ���������M_SC_OK
    while (lex.GetNextToken() == M_SC_POK)
    {
        //����ǰ�﷨�飬������ȫ�ֱ������������ߺ���
        if (Block() != M_SC_OK)
        {
            return M_SC_ERR;
        }
    }

    //���ؽ��
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
// Parse��˽�к����������������Block
//------------------------------------------------------

//����ǰ�﷨�飬������ȫ�ֱ������������ߺ���
int CSCParser::Block()
{
    //�﷨���壺
    //ȫ�ֱ�����(int|string|boolean) var_name[ = expression];
    //ȫ�ֳ�����const (int|string|boolean) const_name = expression;
    //��    ����(void|��������) func_name(�������� para_name, ...)(;|{������})

    //��������һ��symbol�������浱ǰ�����ķ���
    symbol = new CSCSymbol;

    //�ж��ǲ��ǳ���
    if (lex.token.str == "const")
    {
        //�����������﷨�Ƶ�
        if (Const() != M_SC_OK)
        {
            return M_SC_ERR;
        }
    }
    else
    {
        //���������������﷨�Ƶ�
        if (VarOrFunction() != M_SC_OK)
        {
            return M_SC_ERR;
        }
    }

    //��ӷ��ţ����ؽ��
    symbols->Add(symbol);
    return M_SC_OK;
}

//����ȫ�ֳ�����Block��˽�к���
int CSCParser::Const()
{
    //�﷨���壺
    //ȫ�ֳ�����const (int|string|boolean) const_name = expression;

    //����������ͣ�(int|string|boolean)
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

    //��ó�������ConstName
    if (SymbolGetName(symbol) != M_SC_OK)
    {
        return M_SC_ERR;
    }

    //��õȺţ�=
    GET_EXPECT_TOKEN("=");

    //���Ĭ��ֵ���ʽ��Expression
    GET_NEXT_TOKEN;
    EXPRESSION(symbol->type);

    //���Ĭ��ֵ
    if (symbol->type == stConstString)
    {
        *(symbol->str()) = EXPR_RESULT.str;
    }
    else
    {
        symbol->value = EXPR_RESULT.value;
    }
        
    //���ĩβ�ķֺ�
    EXPECT_TOKEN(";");

    //�ɹ����ؽ��
    return M_SC_OK;
}

//����ȫ�ֱ���������Block��˽�к���
int CSCParser::VarOrFunction()
{
    //�﷨���壺
    //ȫ�ֱ�����(int|string|boolean) var_name[ = expression];
    //��    ����(void|��������) func_name(�������� para_name, ...)(;|{������})

    //����������ͣ�(void|int|string|boolean)
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

    //��ñ�������������(VarName|FuncName)
    if (SymbolGetName(symbol) != M_SC_OK)
    {
        return M_SC_ERR;
    }
    if (symbol->type == stFunction)
    {
        symbol->func()->name = symbol->name;
    }

    //�����һ���ַ���(=|;|()
    GET_EXPECT_TOKEN3("=", ";", "(");
    if (lex.token.str == "=")
    {
        //��Ĭ��ֵȫ�ֱ��������Ĭ��ֵ���ʽ��Expression
        return VarAssign(symbol, -1);
    }
    else
    if (lex.token.str == ";")
    {
        //��Ĭ��ֵȫ�ֱ�����ֱ�ӷ���
        return M_SC_OK;
    }
    else
    {
        //���������ú����﷨�Ƶ�����
        if (FunctionDefine() != M_SC_OK)
        {
            return M_SC_ERR;
        }

        //�����꺯�������û�ȫ����ں���
        func = symbols->Get(0).func();
        return M_SC_OK;
    }
}


//------------------------------------------------------
// VarOrFunction��˽�к�����������������
//------------------------------------------------------

//�����������壬VarOrFunction��˽�к���
int CSCParser::FunctionDefine()
{
    //�﷨���壺
    //��    ����(void|��������) func_name(�������� para_name, ...)(;|{������})
    ESymbolType type;   //��������ֵ����
    CNString str;       //����������
    CSCSymbol *parasymb;//�����ķ���
    
    //���ȳ�ʼ��Ϊ����������ӷ���ֵ�������ķ��ű����λ
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
    
    //�ж���һ�������ǲ��ǲ���
    GET_NEXT_TOKEN;
    while (lex.token.str != ")")
    {
        //�������������ͣ�(int|string|boolean)
        EXPECT_TOKEN3("int", "string", "boolean");

        //��������
        parasymb = new CSCSymbol;

        //��������
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
        
        //���������ƣ�ParaName
        if (SymbolGetName(parasymb) != M_SC_OK)
        {
            delete parasymb;
            return M_SC_ERR;
        }

        //��ӵ��б�
        func->symbols.Add(parasymb);
        func->paras++;

        //��ò���ĩβ��
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

    //��ú����壺(;|{������})
    GET_EXPECT_TOKEN2(";", "{");
    if (lex.token.str == ";")
    {
        //���������ö��壬ֱ�ӷ��ؼ���
        func->syscall = True;
        return M_SC_OK;
    }

    //���غ�����ı�����
    return FunctionBody();
}

//���������壬VarOrFunction��˽�к���
int CSCParser::FunctionBody()
{
    //��ʼ�������﷨�ṹ����ջ
    statestk.Clear();

    //��������Ǹ���Ĵ����
    if (StatementGroup() != M_SC_OK)
    {
        return M_SC_ERR;
    }

    //����������
    if (statestk.GetCount() != 0)
    {
        lex.PrintError(205, "line: %d, statement \"%s\" uncomplete",
            statestk.Last().lin + 1, EStatementStr[statestk.Last().type]);
        return M_SC_ERR;
    }
        
    //������ֵ��������жϲ����걸���պ���
    if ((func->symbols[0].type != stVoid) &&
        ((func->codes.GetCount() == 0) || (func->codes.Last().oper != otReturn)))
    {
        lex.PrintError(206, "not all control paths return a value");
        return M_SC_ERR;
    }

    //׷��һ������ָ�������ת������βʧ��
    func->codes.AddCode(otReturn, lex);
    return M_SC_OK;
}


//------------------------------------------------------
// ����(������/������)�ĸ��ִ���
//------------------------------------------------------

//�������з��ţ��������ɱ�����
EExprElementType CSCParser::SymbolLookup()
{
    //�ж��ǲ��Ǻ����ľֲ������������ж�Ϊ�ֲ�����
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

    //�ж�ȫ�ֱ�������
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

    //�Ҳ���������ʧ��
    return etError;
}

//��lex��÷�����
int CSCParser::SymbolGetName(CSCSymbol *symb)
{
    //��÷�������
    GET_NEXT_TOKEN
    if (lex.token.type != ttWord)
    {
        lex.PrintError(203, "\"%s\" is a invalid name!", 
            lex.token.str.S());
        return M_SC_ERR;
    }

    //�������λ����Ϣ
    symb->lin = lex.token.lin;
    symb->col = lex.token.col;
    symb->name = lex.token.str;
    
    //�жϷ����Ƿ�����
    EExprElementType type = SymbolLookup();
    if (func == symbols->Get(0).func())
    {
        //ȫ�ַ���
        if (type == etError)
        {
            return M_SC_OK;
        }
    }
    else
    {
        //�����ľֲ�����
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
// ����������������ʱ����
//------------------------------------------------------

//������ֵ��䣬ͨ�ú�����-1��ʾȫ�ֱ���������Ϊ�ֲ������±�
int CSCParser::VarAssign(CSCSymbol *symb, const int value)
{
    //���ʽ����
    int lin = lex.lin;
    int col = lex.col;
    GET_NEXT_TOKEN;
    EXPRESSION(symb->type);

    //����һ����ֵ���ñ�����ָ��
    CSCCode &code = func->codes.AddCode(otAssign);
    code.lin = lin;
    code.col = col;
    code.symb1 = EXPR_RESULT;
    code.result.type = symb->type;
    if (value == -1)
    {
        //ȫ�ֱ���
        code.result.symbol = symb;
    }
    else
    {
        //�ֲ�����
        code.result.value = value;
        code.result.str = symb->name;
    }

    //���ʽĩβ��;
    EXPECT_TOKEN(";");
    
    //�ɹ����ؽ��
    return M_SC_OK;
}

//������ʱ������value����ʱ�����±�lin��ռ�ñ�־
void CSCParser::VarAllocTemp(const ESymbolType type, CSCCodeSymbol &symb)
{
    //�ֲ���������
    CNString name;  //��ʱ������
    int i;          //ͨ��ѭ������
    CSCSymbol *temp;//��ǰ��ʱ����

    //Ϊ��ʱ����������֣�ֻ�Ǳ��ڵ���
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

    //��ʼ����ʱ����
    symb.type = type;
    symb.symbol = NULL;

    //�������ͷ���ʱ������ʧ���������µ�
    for (i = 1; ;i++)
    {
        temp = func->symbols.Lookup(name + i);
        if (temp == NULL)
        {
            //û�������ʱ������������һ���µ�
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

//�ͷ���ʱ����
void CSCParser::VarFreeTemp(const CSCCodeSymbol &symb)
{
    //����жϣ�������ȫ�ֱ�������
    if (symb.symbol != NULL)
    {
        return;
    }

    //�ͷ���ʱ����
    if ((symb.type == stVarInteger) ||
        (symb.type == stVarString) ||
        (symb.type == stVarBoolean))
    {
        func->symbols[symb.value].lin = M_SC_FREE;
    }
}

//�ֲ���������
int CSCParser::VarLocalDefine()
{
    //�﷨���壺
    //�ֲ�������(int|string|boolean) var_name[ = expression];

    //��������һ��symbol�������浱ǰ�����ķ���
    CSCSymbol *locsymb = new CSCSymbol;

    //����������ͣ�(int|string|boolean)
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

    //��ñ�������������(VarName)
    if (SymbolGetName(locsymb) != M_SC_OK)
    {
        delete locsymb;
        return M_SC_ERR;
    }

    //��ӵ��б�
    func->symbols.Add(locsymb);
    
    //�����һ���ַ���(=|;)
    GET_EXPECT_TOKEN2("=", ";");
    if (lex.token.str == "=")
    {
        //��Ĭ��ֵ�ֲ����������Ĭ��ֵ���ʽ��Expression
        if (VarAssign(locsymb, func->symbols.GetCount() - 1) != M_SC_OK)
        {
            return M_SC_ERR;
        }
    }

    //�ɹ����ؽ��
    return M_SC_OK;
}


//------------------------------------------------------
// �������������
//------------------------------------------------------

//���������
int CSCParser::StatementGroup()
{
    //���ζ�ȡ�Ǻţ��ɹ�����M_SC_POK��ʧ�ܷ���M_SC_ERR���ļ���������M_SC_OK
    int result;
    while ((result = lex.GetNextToken()) == M_SC_POK)
    {
        //����ǰ�﷨�飬�����Ǿֲ�������������if/while/for/switch
        switch (Statement())
        {
        case M_SC_ERR: //����
            return M_SC_ERR;
        case M_SC_POK: //����"}"�ɹ�����
            return M_SC_OK;
        }
    }

    //�ж��ǲ��Ǵ����������Ҳ�Ҳ���"}"
    if (result == M_SC_OK)
    {
        lex.PrintError(232, "state need \"}\" for end");
    }

    //�ߵ�����϶��ǳ�����
    return M_SC_ERR;
}

//����һ�����
int CSCParser::Statement()
{
    //������ʼ��
    if (TkIS("}"))
    {
        //����"}"�ɹ�����
        return M_SC_POK;
    }
    else
    if (TkIS("int") || TkIS("string") || TkIS("boolean"))
    {
        //�ֲ���������
        return VarLocalDefine();
    }
    else
    if (TkIS("return"))
    {
        //��������
        return KeyReturn();
    }
    else
    if (TkIS("if"))
    {
        //if�������
        return StatementIf();
    }
    else
    if (TkIS("for"))
    {
        //forѭ�����
        return StatementFor();
    }
    else
    if (TkIS("while"))
    {
        //whileѭ�����
        return StatementWhile();
    }
    else
    if (TkIS("do"))
    {
        //do...whileѭ�����
        return StatementDoWhile();
    }
    else
    if (TkIS("switch"))
    {
        //switch��֧���
        return StatementSwitch();
    }
    else
    if (TkIS("break"))
    {
        //Break����for/while/switch
        return KeyBreak();
    }
    else
    if (TkIS("continue"))
    {
        //Continue����for/while
        return KeyContinue();
    }
    else
    if (TkIS("case"))
    {
        //Case����while
        return KeyCase();
    }
    else
    if (TkIS("default"))
    {
        //Default����while
        return KeyDefault();
    }
    else
    {
        switch (SymbolLookup())
        {
            case etVarName    : //ȫ�ֱ�����
                GET_EXPECT_TOKEN("=")
                return VarAssign(tempsymbol, -1);
            case etFuncVarName: //����������ֲ�������
                GET_EXPECT_TOKEN("=")
                return VarAssign(tempsymbol, 
                    func->symbols.IndexOf(*tempsymbol));
            case etFuncName   : //������
                //��������
                EXPRESSION(stFunction);
                EXPECT_TOKEN(";");
                return M_SC_OK;
        }

        //����ʧ��
        lex.PrintError(230, "unexpect word \"%s\"", lex.token.str.S());
        return M_SC_ERR;
    }
}

//���������䶨��
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

//��������䵽����ջ
CSCState &CSCParser::StatementAdd(const EStatementType type)
{
    //�﷨�ṹ��ջ
    CSCState &state = statestk.AddState(type, lex);

    //����������
    
    //001  goto (003)
    state.breakpos = func->codes.GetCount() + 1;
    CodeAddGoto(state.breakpos + 1);
    
    //002  goto (breakpos:012)
    CodeAddGoto(0);

    //���ؽ��
    return state;
}


//------------------------------------------------------
// ���������䣬Statement��˽�к���
//------------------------------------------------------

//if�������
int CSCParser::StatementIf()
{
    //�﷨���壺
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
    
    //����: if (bool_expression)
    GET_EXPECT_TOKEN("(");
    EXPRESSION(stVarBoolean)
    state.contipos = func->codes.GetCount();
    CodeAddGoNot(0);

    //������ȷ��������֧
    EXPECT_TOKEN("{")
    STATEMENT_GROUP;
    CodeAddGoto(state.breakpos);

    //else��֧
    GET_NEXT_TOKEN
    while (TkIS("else"))
    {
        GET_EXPECT_TOKEN2("if", "{")
        CodeSetGoNot(state.contipos);
        if (TkIS("if"))
        {
            //else if������֧
            GET_EXPECT_TOKEN("(");
            EXPRESSION(stVarBoolean)
            state.contipos = func->codes.GetCount();
            CodeAddGoNot(0);

            //������ȷ��������֧
            EXPECT_TOKEN("{")
            STATEMENT_GROUP;
            CodeAddGoto(state.breakpos);
            GET_NEXT_TOKEN
        }
        else
        {
            //��else������֧
            EXPECT_TOKEN("{")
            STATEMENT_GROUP;
            
            //ɾ���﷨�ṹ���ɹ�����
            CodeAddBreak();
            return M_SC_OK;
        }
    }
    
    //����������϶��Ƕ�ȡ����һ���ṹ��token��ֱ������
    CodeSetGoNot(state.contipos);
    CodeAddBreak();
    return Statement();
}

//forѭ�����
int CSCParser::StatementFor()
{
    //�﷨���壺
    /*
    for ([var_1 = int_expr]; [bool_expr]; [var_2(++|--)])
    {
        statement
        break;
        continue; 
    }
    */
    CSCState &state = StatementAdd(ftFor);

    //����for�ĵ�һ������: ��ֵ[var_1 = int_expr]
    GET_EXPECT_TOKEN("(");
    GET_NEXT_TOKEN
    if (TkNOT(";"))
    {
        int pos = 0;
        switch (SymbolLookup())
        {
            case etVarName    : //ȫ�ֱ�����
                pos = -1;
                break;
            case etFuncVarName: //����������ֲ�������
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

    //�����ۼӣ�ֱ�ӽ�������ж�
    CodeAddGoto(func->codes.GetCount() + 2);
    state.contipos = func->codes.GetCount();
    CSCCode &code = func->codes.AddCode(otPlu, lex);

    //����for�ĵڶ�������: ����ж�[bool_expr]
    GET_NEXT_TOKEN
    if (TkNOT(";"))
    {
        EXPRESSION(stVarBoolean)
        CodeAddGoNot(state.breakpos);
    }
    
    //����for�ĵ���������: �ۼ��ۼ�[var_2(++|--)]
    GET_NEXT_TOKEN
    if (TkNOT(")"))
    {
        //����ѭ������
        switch (SymbolLookup())
        {
            case etVarName    : //ȫ�ֱ�����
                code.symb1.symbol = tempsymbol;
                break;
            case etFuncVarName: //����������ֲ�������
                code.symb1.value = func->symbols.IndexOf(*tempsymbol);
                code.symb1.str = tempsymbol->name;
                break;
            default:
                lex.PrintError(232, "\"for\" para 3 must be \"[varname(++|--)]\"");
                return M_SC_ERR;
        }
        code.symb1.type = tempsymbol->type;
        code.result = code.symb1;

        //�����ۼ�����
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
    
    //�����������
    GET_EXPECT_TOKEN("{")
    STATEMENT_GROUP;
    CodeAddGoto(state.contipos);
    
    //ɾ�����������ṹ��ɹ�����
    CodeAddBreak();
    return M_SC_OK;
}

//whileѭ�����
int CSCParser::StatementWhile()
{
    //�﷨���壺
    /*
    while (bool_expression)
    {
        statement
        break;
        continue; 
    }
    */
    CSCState &state = StatementAdd(ftWhile);

    //����: while (bool_expression)
    GET_EXPECT_TOKEN("(");
    state.contipos = func->codes.GetCount();
    EXPRESSION(stVarBoolean)
    CodeAddGoNot(state.breakpos);

    //�����������
    EXPECT_TOKEN("{")
    STATEMENT_GROUP;
    CodeAddGoto(state.contipos);
    
    //ɾ�����������ṹ��ɹ�����
    CodeAddBreak();
    return M_SC_OK;
}

//do...whileѭ�����
int CSCParser::StatementDoWhile()
{
    //�﷨���壺
    /*
    do
    {
        statement
        break;
        continue; 
    } while (bool_expression);
    */
    CSCState &state = StatementAdd(ftDoWhile);

    //�����������
    GET_EXPECT_TOKEN("{")
    state.contipos = func->codes.GetCount();
    STATEMENT_GROUP;
    
    //����: while (bool_expression);
    GET_EXPECT_TOKEN("while");
    GET_EXPECT_TOKEN("(");
    EXPRESSION(stVarBoolean)
    CodeAddGoNot(func->codes.GetCount() + 2);
    CodeAddGoto(state.contipos);
    
    //ɾ�����������ṹ��ɹ�����
    CodeAddBreak();
    return M_SC_OK;
}

//switch��֧���
int CSCParser::StatementSwitch()
{
    //�﷨���壺
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

    //���һ����֧�Ƚ�ָ��
    CSCCode &code = func->codes.AddCode(otEqu, lex);
    code.symb2.type = stConstInteger;
    VarAllocTemp(stVarBoolean, code.result);
    
    //����ö��ֵswitch (int_var_name)
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

    //�����������
    GET_EXPECT_TOKEN("{")
    STATEMENT_GROUP;
    if (state.contipos == 0)
    {
        lex.PrintError(234, "\"switch\" need one \"case\" at least");
        return M_SC_ERR;
    }
    
    //ɾ�����������ṹ��ɹ�����
    CodeAddBreak();
    VarFreeTemp(code.result);
    return M_SC_OK;
}


//------------------------------------------------------
// ����������Ĺؼ���
//------------------------------------------------------

//�˳�����
int CSCParser::KeyReturn()
{
    //�жϺ�����û�з���ֵ
    int lin = lex.lin;
    int col = lex.col;
    if (func->symbols[0].type == stVoid)
    {
        //û�з���ֵ
        GET_NEXT_TOKEN
        if (TkNOT(";"))
        {
            lex.PrintError(231, "void function can't return value");
            return M_SC_ERR;
        }
    }
    else
    {
        //�з���ֵ
        if (VarAssign(&(func->symbols[0]), 0) != M_SC_OK)
        {
            return M_SC_ERR;
        }
    }

    //�����˳�������ָ��
    CSCCode &code = func->codes.AddCode(otReturn);
    code.lin = lin;
    code.col = col;
    return M_SC_OK;
}

//�˳�������
int CSCParser::KeyBreak()
{
    //����Break�ĸ�ѭ��
    int pos = StatementLookup("break");
    if (pos == -1)
    {
        lex.PrintError(234, "\"break\" without \"for/while/switch\"");
        return M_SC_ERR;
    }

    //���Breakָ��
    CodeAddGoto(statestk[pos].breakpos);
    
    //�ɹ�����
    GET_EXPECT_TOKEN(";")
    return M_SC_OK;
}

//����ѭ��������
int CSCParser::KeyContinue()
{
    //����Continue�ĸ�ѭ��
    int pos = StatementLookup("continue");
    if (pos == -1)
    {
        lex.PrintError(234, "\"continue\" without \"for/while\"");
        return M_SC_ERR;
    }

    //���Continueָ��
    CodeAddGoto(statestk[pos].contipos);
    
    //�ɹ�����
    GET_EXPECT_TOKEN(";")
    return M_SC_OK;
}

//Switch�ķ�֧
int CSCParser::KeyCase()
{
    //����Case�ĸ�Switch
    int pos = StatementLookup("case");
    if (pos == -1)
    {
        lex.PrintError(234, "\"case\" without \"switch\"");
        return M_SC_ERR;
    }

    //��������ж�
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
        
    //���������������ʽ
    GET_NEXT_TOKEN;
    EXPRESSION(stConstInteger);

    //��ӳ����Ƚϱ��ʽ
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

    //������תָ��
    statestk[pos].contipos = func->codes.GetCount();
    code1 = &(func->codes.AddCode(otGoNot, lex));
    code1->symb1 = code->result;
    code1->result.type = stConstInteger;

    //�ɹ�����
    EXPECT_TOKEN(":")
    return M_SC_OK;
}

//Switch��Ĭ�Ϸ�֧
int CSCParser::KeyDefault()
{
    //����Default�ĸ�Switch
    int pos = StatementLookup("default");
    if (pos == -1)
    {
        lex.PrintError(234, "\"default\" without \"switch\"");
        return M_SC_ERR;
    }

    //��������ж�
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

    //������תλ��
    func->codes[statestk[pos].contipos].result.value = func->codes.GetCount();
    statestk[pos].contipos = -1;

    //ֱ�ӷ��ؼ���
    GET_EXPECT_TOKEN(":")
    return M_SC_OK;
}


//------------------------------------------------------
// Ϊ����������м����
//------------------------------------------------------

//��������ת
void CSCParser::CodeAddGoto(const int pos)
{
    CSCCode &code = func->codes.AddCode(otGoto, lex);
    code.result.type = stConstInteger;
    code.result.value = pos;
}

//��������ת
void CSCParser::CodeAddGoNot(const int pos)
{
    CSCCode &code = func->codes.AddCode(otGoNot, lex);
    code.symb1 = EXPR_RESULT;
    code.result.type = stConstInteger;
    code.result.value = pos;
}

//������������תλ��
void CSCParser::CodeSetGoNot(const int pos)
{
    func->codes[pos].result.value = func->codes.GetCount();
}

//�˳�������
void CSCParser::CodeAddBreak()
{
    CodeSetGoNot(statestk.Last().breakpos);
    statestk.DeleteLast();
}

