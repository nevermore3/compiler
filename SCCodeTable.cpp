//���ű�
#include "SCSymbolTable.h"


//------------------------------------------------------
// ���ʽ�����ջ
//------------------------------------------------------

//���һ�������
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
// ���ʽ����
//------------------------------------------------------

//�ǲ�������
Bool CSCCodeSymbol::IsInt()
{
    return ((type == stVarInteger) || (type == stConstInteger));
}

//�ǲ����ַ���
Bool CSCCodeSymbol::IsStr()
{
    return ((type == stVarString) || (type == stConstString));
}

//�ǲ��ǲ���ֵ
Bool CSCCodeSymbol::IsBool()
{
    return ((type == stVarBoolean) || (type == stConstBoolean));
}

//�ǲ��Ǳ���
Bool CSCCodeSymbol::IsVar()
{
    return ((type == stVarInteger) || (type == stVarString) || 
            (type == stVarBoolean));
}

//����Var����
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

//���ڵ��Ա��ʽ����
void CSCCodeSymbol::Debug() const
{
    if (symbol == NULL)
    {
        //ȫ�ֳ�������
        switch (type)
        {
        case stConstInteger : //��������
            printf("%d", value);
            break;
        case stConstString  : //�ַ�������
            printf("\"%s\"", str.S());
            break;
        case stConstBoolean : //����ֵ����
            printf("%s", (value ? "true" : "false"));
            break;
        case stVarInteger   : //������ʱ����
        case stVarString    : //�ַ���ʱ������
        case stVarBoolean   : //������ʱֵ����
            printf("L%d:%s:%s", value, ESymbolStr[type], str.S());
            break;
        case stVoid         : //δָ��
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
// ���ʽ����ջ
//------------------------------------------------------

//���һ������
CSCCodeSymbol &CSCSymbolStack::AddSymb()
{
    Add();
    return Last();
}


//------------------------------------------------------
// �м����(�沨��ʽ)
//------------------------------------------------------

//���ڵ����м����
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
// �м�����(�沨��ʽ��)
//------------------------------------------------------

//���һ��ָ��
CSCCode &CSCCodeTable::AddCode(const EOperatorType oper)
{
    Add();
    CSCCode &code = Last();
    code.oper = oper;
    return code;
}

//���һ��ָ��
CSCCode &CSCCodeTable::AddCode(const EOperatorType oper,
    const CSCLexicaler &lex)
{
    CSCCode &code = AddCode(oper);
    code.lin = lex.lin;
    code.col = lex.col;
    return code;
}
    
//���һ��ָ��
CSCCode &CSCCodeTable::AddCode(const CSCOperator &oper)
{
    CSCCode &code = AddCode(oper.oper);
    code.lin = oper.lin;
    code.col = oper.col;
    return code;
}

//���ڵ����м�����
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
// �����﷨�ṹ����ջ
//------------------------------------------------------

//���һ�������﷨�ṹ
CSCState &CSCStateStack::AddState(const EStatementType type, const CSCLexicaler &lex)
{
    Add();
    CSCState &state = Last();
    state.type = type;
    state.lin = lex.lin;
    state.col = lex.col;
    return state;
}
