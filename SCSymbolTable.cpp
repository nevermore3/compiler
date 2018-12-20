//���ű����﷨�����ĳ�Ա
#include "SCSymbolTable.h"


//------------------------------------------------------
// ���ŵĶ���
//------------------------------------------------------

//��ʼ��
void CSCSymbol::Init(const char *_name, const ESymbolType _type)
{
    //�������ԭ���ڴ�
    Clear();
    
    //Ȼ��ֵ
    name = _name;
    type = _type;
    
    //Ϊ�ַ��������ڴ�
    if ((type == stVarString) || (type == stConstString))
    {
        value = (int) new CNString;
    }
    
    //Ϊ���������ڴ�
    if (type == stFunction)
    {
        value = (int) new CSCFunction;
        func()->symbols.AddSymb("@retrun", stVoid);
        func()->symbols[0].lin = lin;
        func()->symbols[0].col = col;
        func()->name = name;
    }
}

//����ڴ�
void CSCSymbol::Clear()
{
    //Ϊ�ַ����ͷ��ڴ�
    if ((value != 0) && ((type == stVarString) || (type == stConstString)))
    {
        delete str();
        value = 0;
    }
    
    //Ϊ�����ͷ��ڴ�
    if ((value != 0) && (type == stFunction))
    {
        delete func();
        value = 0;
    }
}

//���ڵ���
void CSCSymbol::Debug(const Bool showdetail)
{
    if (showdetail)
    {
        //��ϸ��ӡ
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
        //�򵥴�ӡ
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

//���ڵ���
void CSCFunction::Debug()
{
    //��ӡ����ֵ
    printf("\r\n       return : ");
    symbols[0].Debug();
    printf("\r\n");

    //��ӡ����
    for (int i = 1; i <= paras; i++)
    {
        printf("       para %d : ", i);
        symbols[i].Debug();
        printf("\r\n");
    }

    //��ӡ�ֲ�����
    for (; i < symbols.GetCount(); i++)
    {
        printf("    loc_var %d : ", i);
        symbols[i].Debug();
        printf("\r\n");
    }

    //��ӡ�м����
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
// ���ű�
//------------------------------------------------------

//���һ������
CSCSymbol &CSCSymbolTable::AddSymb(CSCSymbol &_symb)
{
    Add();
    CSCSymbol &symb = Last();
    symb.Init(_symb.name, _symb.type);
    symb.lin = _symb.lin;
    symb.col = _symb.col;
    return symb;
}

//���һ������
CSCSymbol &CSCSymbolTable::AddSymb(const char *name, const ESymbolType type)
{
    Add();
    CSCSymbol &symb = Last();
    symb.Init(name, type);
    symb.lin = 0;
    symb.col = 0;
    return symb;
}

//���һ������
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

//�������Ʋ��ҷ��ţ�ʧ�ܷ���0
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

//���ڵ��ԣ�showsys��ʱ�Ƿ���ʾϵͳ���������
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

