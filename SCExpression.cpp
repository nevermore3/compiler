//�﷨������
#include "SCParser.h"


//------------------------------------------------------
// �﷨�����ľֲ���
//------------------------------------------------------

//�ж�����Ҫ�ļǺŵĺ�
#define TkIS(t) (parser->lex.token.str == t)

//��ӡ������Ϣ���к�
#define LIN(obj) obj.lin, obj.col

//��ջһ�������
#define POP_A_OPER \
    if (PopAOper() != M_SC_OK) \
    { \
        return M_SC_ERR; \
    }

//��һ�����ʽԪ�ظ������
#define NEXT_TOKEN_ERR \
    parser->lex.PrintError(209, "\"%s\" can't follow \"%s\"", \
        parser->lex.token.str.S(), EExprElementStr[oldtype]); \
    curtype = etError;

//��ǰ�����ĺ���
#define FUNC parser->func


//------------------------------------------------------
// ���ʽ������
//------------------------------------------------------
    
//���ʽ���㣬�Ǳ��ʽ�������Ψһ���
int CSCExpression::Calc(const ESymbolType needtype)
{
    //�ֲ���������
    int result;

    //��ʼ�����ʽ����������
    curtype = etError;
    operatorstk.Clear();
    symbolstk.Clear();
    
    //ѭ���ķ������ʽ��ÿ��Ԫ��
    while ((result = GetNextElem()) == M_SC_OK)
    {
        switch (curtype)
        {
        case etLBracket   :  //������(
            PushOperator();
            break;
        case etRBracket   :  //������)
            if (PopWith(curtype) != M_SC_OK)
            {
                return M_SC_ERR;
            }
            break;
        case etComma      :  //����
        case etSign       :  //������+��-
        case etPluMin     :  //�Ӽ���+��-
        case etMulDiv     :  //�˳���*��/��%
        case etCompare    :  //�ȽϷ���>��<��>=��<=
        case etCompareEqu :  //���ڱȽϷ���!=��==
        case etLogicAnd   :  //�߼�����&&
        case etLogicOr    :  //�߼�����||
        case etLogicNot   :  //�߼�������!
            if (PopWith(curtype) != M_SC_OK)
            {
                return M_SC_ERR;
            }
            PushOperator();
            break;
        case etInteger    :  //��������
        case etString     :  //�ַ�������
        case etBoolean    :  //��������
        case etConstName  :  //������
        case etVarName    :  //������
        case etFuncName   :  //������
        case etFuncVarName:  //����������ֲ�������
            PushSymbol();
            break;
        }
    }

    //��ȡ���ʽԪ�س���
    if (result == M_SC_ERR)
    {
        return M_SC_ERR;
    }

    //�Ѷ�ջ��û������Ķ���ȫ�������
    if (PopAll() != M_SC_OK)
    {
        return M_SC_ERR;
    }

    //��鷵��ֵ����
    switch (needtype)
    {
    case stConstInteger: //��������
    case stConstString : //�ַ�������
    case stConstBoolean: //����ֵ����
        if ((symbolstk.GetCount() != 1) ||
            (needtype != symbolstk[0].type))
        {
            parser->lex.PrintError(210, "expression must return a \"%s\" value!",
                ESymbolStr[needtype]);
            return M_SC_ERR;
        }
        break;
    case stVarInteger  : //��������
    case stVarString   : //�ַ�������
    case stVarBoolean  : //����ֵ����
        if ((symbolstk.GetCount() != 1) ||
            (needtype != symbolstk[0].VarType()))
        {
            parser->lex.PrintError(210, "expression must return a \"%s\" value!",
                ESymbolStr[needtype]);
            return M_SC_ERR;
        }
        break;
    case stVoid        : //δָ��
        if (symbolstk.GetCount() != 0)
        {
            parser->lex.PrintError(211, "this expression can't return value!");
            return M_SC_ERR;
        }
    }
        
    //���ʽ�����ɹ�
    return M_SC_OK;
}

//��ǰ�ǺŽ������ջ��Calc��˽�к���
void CSCExpression::PushOperator()
{
    //�������������������
    CSCOperator &oper = operatorstk.AddOper(curtype, parser->lex);

    //����������ľ�������
    if (TkIS("("))
    {
        if ((symbolstk.GetCount() > 0) && (symbolstk.Last().type == stFunction))
        {
            oper.oper = otFuncLBra;
        }
        else
        {
            oper.oper = otLBracket;
        }
    }
    else
    if (TkIS(","))
    {
        oper.oper = otComma;
    }
    else
    if (TkIS("+"))
    {
        oper.oper = (curtype == etSign ? otSignPlu : otPlu);
    }
    else
    if (TkIS("-"))
    {
        oper.oper = (curtype == etSign ? otSignMin : otMin);
    }
    else
    if (TkIS("*"))
    {
        oper.oper = otMul;
    }
    else
    if (TkIS("/"))
    {
        oper.oper = otDiv;
    }
    else
    if (TkIS("%"))
    {
        oper.oper = otMod;
    }
    else
    if (TkIS(">"))
    {
        oper.oper = otMore;
    }
    else
    if (TkIS("<"))
    {
        oper.oper = otLess;
    }
    else
    if (TkIS(">="))
    {
        oper.oper = otMoreEqu;
    }
    else
    if (TkIS("<="))
    {
        oper.oper = otLessEqu;
    }
    else
    if (TkIS("!="))
    {
        oper.oper = otNotEqu;
    }
    else
    if (TkIS("=="))
    {
        oper.oper = otEqu;
    }
    else
    if (TkIS("||"))
    {
        oper.oper = otOr;
    }
    else
    if (TkIS("&&"))
    {
        oper.oper = otAnd;
    }
    else
    if (TkIS("!"))
    {
        oper.oper = otNot;
    }
    else
    {
        oper.oper = otVoid;
    }
}

//��ǰ�ǺŽ�����ջ��Calc��˽�к���
void CSCExpression::PushSymbol()
{
    //��ӷ��ż����������
    CSCCodeSymbol &symb = symbolstk.AddSymb();

    //�������ŵ�ȡֵ
    switch (curtype)
    {
    case etInteger    :  //��������
        symb.type = stConstInteger;
        symb.value = parser->lex.token.str.I();
        break;
    case etString     :  //�ַ�������
        symb.type = stConstString;
        symb.str = parser->lex.token.str;
        break;
    case etBoolean    :  //��������
        symb.type = stConstBoolean;
        symb.value = TkIS("true");
        break;
    case etConstName  :  //������
        symb.type = parser->tempsymbol->type;
        switch (symb.type)
        {
        case stConstInteger:
        case stConstBoolean:
            symb.value = parser->tempsymbol->value;
            break;
        case stConstString:
            symb.str = *(parser->tempsymbol->str());
            break;
        }
        break;
    case etVarName    :  //ȫ�ֱ�����
    case etFuncName   :  //������
        symb.type = parser->tempsymbol->type;
        symb.symbol = parser->tempsymbol;
        break;
    case etFuncVarName:  //����������ֲ�������
        symb.type = parser->tempsymbol->type;
        symb.value = parser->func->symbols.IndexOf(*parser->tempsymbol);
        break;
    }
}

//���Ʒ��Ž�ջ��Calc��˽�к���
void CSCExpression::PushSymbol(const CSCCodeSymbol &symbol)
{
    symbolstk.Add();
    symbolstk.Last() = symbol;
}

//��ջһ����Ŀ���㣬PopAOper��˽�к���
int CSCExpression::PopOne(CSCCode &code, const CSCOperator &oper)
{
    //����ж�
    if (symbolstk.GetCount() < 1)
    {
        parser->lex.PrintErrorLin(LIN(oper), 302, 
            "\"%s\" not one symbol!", EOperatorSign[oper.oper]);
        return M_SC_ERR;
    }

    //�������1
    code.symb1 = symbolstk.Last();
    symbolstk.DeleteLast();

    //ִ������
    switch (oper.oper)
    {
    case otSignPlu:  //����+
    case otSignMin:  //����-
        switch (code.symb1.type)
        {
        case stConstInteger:
            //��������ֱ�����㣬��������м����
            PushSymbol(code.symb1);
            if (oper.oper == otSignMin)
            {
                symbolstk.Last().value = -symbolstk.Last().value;
            }
            FUNC->codes.DeleteLast();
            break;
        case stVarInteger:
            //����������Ҫ�������м������
            parser->VarFreeTemp(code.symb1);
            parser->VarAllocTemp(stVarInteger, code.result);
            PushSymbol(code.result);
            break;
        default:
            //����
            parser->lex.PrintErrorLin(LIN(oper), 304, 
                "\"%s\" only for integer!", EOperatorSign[oper.oper]);
            return M_SC_ERR;
        }
        break;
    case otNot:  //!
        switch (code.symb1.type)
        {
        case stConstBoolean:
            //��������ֱ�����㣬��������м����
            PushSymbol(code.symb1);
            symbolstk.Last().value = !symbolstk.Last().value;
            FUNC->codes.DeleteLast();
            break;
        case stVarBoolean:
            //����������Ҫ�������м������
            parser->VarFreeTemp(code.symb1);
            parser->VarAllocTemp(stVarBoolean, code.result);
            PushSymbol(code.result);
            break;
        default:
            //����
            parser->lex.PrintErrorLin(LIN(oper), 305, 
                "\"%s\" only for boolean!", EOperatorSign[oper.oper]);
            return M_SC_ERR;
        }
        break;
    }

    //�ɹ�����
    return M_SC_OK;
}

//��ջһ��˫Ŀ���㣬PopAOper��˽�к���
int CSCExpression::PopTwo(CSCCode &code, const CSCOperator &oper)
{
    //����ж�
    if (symbolstk.GetCount() < 2)
    {
        parser->lex.PrintErrorLin(LIN(oper), 303, 
            "\"%s\" need two symbols!", EOperatorSign[oper.oper]);
        return M_SC_ERR;
    }

    //�������2
    code.symb2 = symbolstk.Last();
    symbolstk.DeleteLast();

    //�������1
    code.symb1 = symbolstk.Last();
    symbolstk.DeleteLast();

    //��֧�ֲ�ͬ�������ͼ����ʽת��
    if ((code.symb1.VarType() != code.symb2.VarType()) ||
        (code.symb1.VarType() == stVoid))
    {
        parser->lex.PrintErrorLin(LIN(oper), 305, 
            "\"%s\" can not for different data type!", EOperatorSign[oper.oper]);
        return M_SC_ERR;
    }

    //��������Ч��
    switch (oper.oper)
    {
    case otPlu:  //�Ӻ�+
    case otMin:  //����-
    case otMul:  //�˺�*
    case otDiv:  //����/
    case otMod:  //ģ��%
        //int & int = int
        if (!code.symb1.IsInt())
        {
            //"+"��֧�֣�str & str = str
            if ((oper.oper != otPlu) || !code.symb1.IsStr())
            {
                parser->lex.PrintErrorLin(LIN(oper), 308, 
                    "\"%s\" can not for string/boolean data type!", EOperatorSign[oper.oper]);
                return M_SC_ERR;
            }
        }

        //�����������
        if ((oper.oper == otDiv) || (oper.oper == otMod))
        {
            if (!code.symb2.IsVar() && (code.symb2.value == 0))
            {
                parser->lex.PrintErrorLin(LIN(oper), 310, 
                    "\"%s\" div 0 error!", EOperatorSign[oper.oper]);
                return M_SC_ERR;
            }
        }
        break;

    case otMore    :  //>
    case otLess    :  //<
    case otMoreEqu :  //>=
    case otLessEqu :  //<=
        //int & int = bool
        if (!code.symb1.IsInt())
        {
            parser->lex.PrintErrorLin(LIN(oper), 308, 
                "\"%s\" can not for string/boolean data type!", EOperatorSign[oper.oper]);
            return M_SC_ERR;
        }
        break;

    case otOr :  //||
    case otAnd:  //&&
        //bool & bool = bool
        if (!code.symb1.IsBool())
        {
            parser->lex.PrintErrorLin(LIN(oper), 309, 
                "\"%s\" can not for integer/string data type!", EOperatorSign[oper.oper]);
            return M_SC_ERR;
        }
        break;
    }

    //�������㣬��������м����
    if (!code.symb1.IsVar() && !code.symb2.IsVar())
    {
        PushSymbol(code.symb1);
        switch (oper.oper)
        {
        case otPlu:  //�Ӻ�+
            if (code.symb1.IsInt())
            {
                symbolstk.Last().value += code.symb2.value;
            }
            else
            {
                symbolstk.Last().str += code.symb2.str;
            }
            break;
        case otMin:  //����-
            symbolstk.Last().value -= code.symb2.value;
            break;
        case otMul:  //�˺�*
            symbolstk.Last().value *= code.symb2.value;
            break;
        case otDiv:  //����/
            symbolstk.Last().value /= code.symb2.value;
            break;
        case otMod:  //ģ��%
            symbolstk.Last().value %= code.symb2.value;
            break;
        case otMore:  //>
            symbolstk.Last().type = stConstBoolean;
            symbolstk.Last().value = (code.symb1.value > code.symb2.value);
            break;
        case otLess:  //<
            symbolstk.Last().type = stConstBoolean;
            symbolstk.Last().value = (code.symb1.value < code.symb2.value);
            break;
        case otMoreEqu:  //>=
            symbolstk.Last().type = stConstBoolean;
            symbolstk.Last().value = (code.symb1.value >= code.symb2.value);
            break;
        case otLessEqu:  //<=
            symbolstk.Last().type = stConstBoolean;
            symbolstk.Last().value = (code.symb1.value <= code.symb2.value);
            break;
        case otNotEqu:  //!=
            symbolstk.Last().type = stConstBoolean;
            switch (code.symb1.type)
            {
            case stConstInteger:
                symbolstk.Last().value = (code.symb1.value != code.symb2.value);
            case stConstString:
                symbolstk.Last().value = (code.symb1.str != code.symb2.str);
            case stConstBoolean:
                symbolstk.Last().value = ((!code.symb1.value &&  code.symb2.value) ||
                                        ( code.symb1.value && !code.symb2.value));
            }
            break;
        case otEqu:  //==
            symbolstk.Last().type = stConstBoolean;
            switch (code.symb1.type)
            {
            case stConstInteger:
                symbolstk.Last().value = (code.symb1.value == code.symb2.value);
            case stConstString:
                symbolstk.Last().value = (code.symb1.str == code.symb2.str);
            case stConstBoolean:
                symbolstk.Last().value = (( code.symb1.value &&  code.symb2.value) ||
                                        (!code.symb1.value && !code.symb2.value));
            }
            break;
        case otOr://||
            symbolstk.Last().value = (code.symb1.value || code.symb2.value);
            break;
        case otAnd://&&
            symbolstk.Last().value = (code.symb1.value && code.symb2.value);
            break;
        }
        FUNC->codes.DeleteLast();
    }
    else
    {
        //�������㣬�����ͷžɵ���ʱ����
        parser->VarFreeTemp(code.symb1);
        parser->VarFreeTemp(code.symb2);
        switch (oper.oper)
        {
        case otMore    :  //>�����ĸ�������Ľ����ֵ���Ͳ�ͬ
        case otLess    :  //<
        case otMoreEqu :  //>=
        case otLessEqu :  //<=
            parser->VarAllocTemp(stVarBoolean, code.result);
            break;
        default:
            parser->VarAllocTemp(code.symb1.VarType(), code.result);
        }
        PushSymbol(code.result);
    }

    //�ɹ�����
    return M_SC_OK;

}

//��ջһ��������PopAOper��˽�к���
int CSCExpression::PopFunc(CSCCode &code)
{
    //�ֲ���������
    int i, j;    //ͨ��ѭ������
    CSCCodeSymbol *symb = NULL;  //��ǰ�ĺ�������
    CSCFunction *function = NULL;//��ǰ�ĺ���ָ��

    //�����������
    for (i = symbolstk.GetCount() - 1; i >= 0; i--)
    {
        if (symbolstk[i].type == stFunction)
        {
            symb = &(symbolstk[i]);
            break;
        }
    }
    if (symb == NULL)
    {
        parser->lex.PrintErrorLin(LIN(code), 311, 
            "() without function!");
        return M_SC_ERR;        
    }
    code.symb1 = *symb;

    //��������
    function = symb->symbol->func();
    if (function->paras != symbolstk.GetCount() - 1 - i)
    {
        parser->lex.PrintErrorLin(LIN(code), 312, 
            "function \"%s\" need %d parameters!",
            function->name.S(), function->paras);
        return M_SC_ERR;        
    }
    
    //������������Ч�飬ͨ������ջ
    for (j = 1, i++; j <= function->paras; i++, j++)
    {
        //��������Ч��
        if (symbolstk[i].VarType() != function->symbols[j].type)
        {
            parser->lex.PrintErrorLin(LIN(code), 313, 
                "function \"%s\" parameter %d need %s but give %s!",
                function->name.S(), j, 
                ESymbolStr[function->symbols[j].type], ESymbolStr[symbolstk[i].type]);
            return M_SC_ERR;        
        }

        //��Ӻ�������ѹջָ��
        CSCCode &para = FUNC->codes.AddCode(otAssign);
        para.lin = code.lin;
        para.col = code.col;
        para.symb1 = symbolstk[i];
        parser->VarFreeTemp(symbolstk[i]);
        para.result.type = function->symbols[j].type;
        para.result.value = j;
        para.result.str = function->symbols[j].name;
    }

    //�������������ջ
    for (i = 0; i <= function->paras; i++)
    {
        symbolstk.DeleteLast();
    }

    //�������ķ���ֵ
    if (function->symbols[0].type != stVoid)
    {
        parser->VarAllocTemp(function->symbols[0].type, code.result);
        PushSymbol(code.result);
    }
        
    //�ɹ�����
    return M_SC_OK;

}

//��ջһ���м���룬Calc��˽�к���
int CSCExpression::PopAOper()
{
    //����ж�
    if (operatorstk.GetCount() <= 0)
    {
        parser->lex.PrintError(301, 
            "not operator when pop expression!");
        return M_SC_ERR;
    }

    //���һ���м����
    CSCOperator &oper = operatorstk.Last();
    CSCCode &code = FUNC->codes.AddCode(oper.oper);
    code.lin = oper.lin;
    code.col = oper.col;
    int result = M_SC_OK;

    //���ݲ�����ջ��Ԫ���ж���γ�ջ
    switch (oper.oper)
    {
    case otFuncLBra:  //����������(
        //����
        result = PopFunc(code);
        break;
    case otSignPlu :  //����+
    case otSignMin :  //����-
    case otNot     :  //!
        //��Ŀ����
        result = PopOne(code, oper);
        break;
    case otPlu     :  //�Ӻ�+
    case otMin     :  //����-
    case otMul     :  //�˺�*
    case otDiv     :  //����/
    case otMod     :  //ģ��%
    case otMore    :  //>
    case otLess    :  //<
    case otMoreEqu :  //>=
    case otLessEqu :  //<=
    case otNotEqu  :  //!=
    case otEqu     :  //==
    case otOr      :  //||
    case otAnd     :  //&&
        //˫Ŀ����
        result = PopTwo(code, oper);
        break;
    default:
        //��������ֻҪɾ������ջ�Ϳ����ˣ���������м����
        FUNC->codes.DeleteLast();
        break;
    }

    //ɾ�������ջ��Ԫ��
    operatorstk.DeleteLast();
    
    //�ɹ�����
    return result;
}

//һֱ��ջ����operƥ����������Calc��˽�к���
int CSCExpression::PopWith(const EExprElementType type)
{
    while (operatorstk.GetCount() > 0)
    {
        if ((type == etRBracket) || (type == etComma))
        {
            if (operatorstk.Last().type != etLBracket)
            {
                POP_A_OPER
            }
            else
            {
                break;
            }
        }
        else
        {
            if (operatorstk.Last().type <= type)
            {
                POP_A_OPER
            }
            else
            {
                break;
            }
        }
    }

    //����ƥ����
    if ((type == etRBracket) || (type == etComma))
    {
        if ((operatorstk.GetCount() == 0) || 
            (operatorstk.Last().type != etLBracket))
        {
            parser->lex.PrintError(320, "\"%s\" need matched \"(\"", 
                (type == etRBracket ? ")" : ","));
            return M_SC_ERR;
        }
        else
        if (type == etRBracket)
        {
            POP_A_OPER
        }
    }

    //�ɹ�����
    return M_SC_OK;
}

//���б��ʽ����ջ��Calc��˽�к���
int CSCExpression::PopAll()
{
    //���е��������ջ
    while (operatorstk.GetCount() > 0)
    {
        //���ܳ���"("����Ϊ"("���ᱻƥ���")"��ջ
        CSCOperator &oper = operatorstk.Last();
        if (oper.oper == otLBracket)
        {
            parser->lex.PrintErrorLin(LIN(oper), 321, 
                "\"(\" need matched \")\"");
            return M_SC_ERR;
        }

        //��ǰջ�����ų�ջ
        POP_A_OPER
    }

    //�����������
    if (symbolstk.GetCount() > 1)
    {
        parser->lex.PrintError(322, "unwanted symbol \"%s\"", 
            ESymbolStr[symbolstk[1].type]);
        return M_SC_ERR;
    }
    
    //��ʱ�����ͷ�
    if (symbolstk.GetCount() > 0)
    {
        parser->VarFreeTemp(symbolstk[0]);
    }

    //�ɹ�����
    return M_SC_OK;
}

//��õ�ǰ�Ǻŵı��ʽԪ�����ͣ�Calc��˽�к���
//�����﷨�����ʽĩβ���ſ����� ; { : ����
int CSCExpression::GetNextElem()
{
    //�������ʽԪ�ص�����
    EExprElementType oldtype = curtype;
    curtype = etError;

    //������ǳ�ʼ��״̬����ȡ��һ��Ԫ��
    if (oldtype != etError)
    {
        if (parser->lex.GetNextToken() != M_SC_POK)
        {
            return M_SC_ERR;
        }
    }
    
    //����ÿ���Ǻ�    
    switch (parser->lex.token.type)
    {
    case ttInteger://����
        curtype = etInteger;
        break;
    case ttString://�ַ���
        curtype = etString;
        break;
    case ttOperator://������
        if (TkIS(";") || TkIS("{") || TkIS(":"))
        {
            //;��{��ʾ���ʽ��ɣ���POK��ʾ
            return M_SC_POK;
        }
        else
        if (TkIS("("))
        {
            curtype = etLBracket;
        }
        else
        if (TkIS(")"))
        {
            curtype = etRBracket;
        }
        else
        if (TkIS(","))
        {
            curtype = etComma;
        }
        else
        if (TkIS("+") || TkIS("-"))
        {
            //ֻ����5�ֱ��ʽԪ�غ�����Ը��Ӽ�
            if ((oldtype == etRBracket) || (oldtype == etInteger) ||
                (oldtype == etString)   || (oldtype == etConstName) ||
                (oldtype == etVarName)  || (oldtype == etFuncVarName))
            {
                curtype = etPluMin;
            }
            else
            {
                curtype = etSign;
            }
        }
        else
        if (TkIS("*") || TkIS("/") || TkIS("%"))
        {
            curtype = etMulDiv;
        }
        else
        if (TkIS(">")  || TkIS("<") ||
            TkIS(">=") || TkIS("<="))
        {
            curtype = etCompare;
        }
        else
        if (TkIS("==") || TkIS("!="))
        {
            curtype = etCompareEqu;
        }
        else
        if (TkIS("&&"))
        {
            curtype = etLogicAnd;
        }
        else
        if (TkIS("||"))
        {
            curtype = etLogicOr;
        }
        else
        if (TkIS("!"))
        {
            curtype = etLogicNot;
        }
        else
        {
            parser->lex.PrintError(206, 
                "unexpect operator \"%s\" in expression!",
                parser->lex.token.str.S());
            return M_SC_ERR;
        }
        break;
    case ttWord://����
        //�ж��ǲ���ȫ�ֱ�����ȫ�ֳ���������
        curtype = parser->SymbolLookup();

        //û���ҵ�����
        if (curtype == etError)
        {
            parser->lex.PrintError(207, 
                "unknown const/variable/function \"%s\" in expression!",
                parser->lex.token.str.S());
            return M_SC_ERR;
        }
        
        break;
    case ttKeyword://�ؼ���
        if (TkIS("true") || TkIS("false"))
        {
            curtype = etBoolean;
        }
        else
        {
            parser->lex.PrintError(208, 
                "unexpect key word \"%s\" in expression!",
                parser->lex.token.str.S());
            return M_SC_ERR;
        }
        break;
    }

    //������һ��Ԫ�������ж���Ԫ���Ƿ�Ϸ�
    switch (oldtype)
    {
    case etLBracket://"("���ܸ�","��˫Ŀ�����
        if ((curtype == etComma) || (curtype == etPluMin) ||
            (curtype == etMulDiv) || (curtype == etCompare) ||
            (curtype == etCompareEqu) || (curtype == etLogicAnd) || 
            (curtype == etLogicOr))
        {
            NEXT_TOKEN_ERR
        }
        break;
    case etRBracket://")"���ܸ�"("����Ŀ�������������
        if ((curtype == etLBracket) || (curtype == etSign) ||
            (curtype == etLogicNot) || (curtype == etInteger) ||
            (curtype == etString) || (curtype == etBoolean) ||
            (curtype == etConstName) || (curtype == etVarName) ||
            (curtype == etFuncName) || (curtype == etFuncVarName))
        {
            NEXT_TOKEN_ERR
        }
        break;
    case etComma://","���ܸ�")"��","��˫Ŀ�����
        if ((curtype == etRBracket) || (curtype == etComma) ||
            (curtype == etPluMin) || (curtype == etMulDiv) ||
            (curtype == etCompare) || (curtype == etCompareEqu) || 
            (curtype == etLogicAnd) || (curtype == etLogicOr))
        {
            NEXT_TOKEN_ERR
        }
        break;
    case etSign://�Ӽ��Ų��ܸ�")"��","���������������
        if ((curtype == etRBracket) || (curtype == etComma) ||
            (curtype == etSign) || (curtype == etPluMin) ||
            (curtype == etMulDiv) || (curtype == etCompare) || 
            (curtype == etCompareEqu) || (curtype == etLogicAnd) || 
            (curtype == etLogicOr) || (curtype == etLogicNot) || 
            (curtype == etString) || (curtype == etBoolean))
        {
            NEXT_TOKEN_ERR
        }
        break;
    case etPluMin://˫Ŀ��������ܸ�")"��","��˫Ŀ�����
    case etMulDiv:
    case etCompare:
    case etCompareEqu:
    case etLogicAnd:
    case etLogicOr:
    case etLogicNot://��Ŀ�����"!"�Ľ�ֹ���etLogicһ��
        if ((curtype == etRBracket) || (curtype == etComma) ||
            (curtype == etPluMin) || (curtype == etMulDiv) ||
            (curtype == etCompare) || (curtype == etCompareEqu) || 
            (curtype == etLogicAnd) || (curtype == etLogicOr))
        {
            NEXT_TOKEN_ERR
        }
        break;
    case etInteger://���������ܸ�"("����Ŀ�������������
    case etString:
    case etBoolean:
    case etConstName:
    case etVarName:
    case etFuncVarName:
        if ((curtype == etLBracket) || (curtype == etSign) ||
            (curtype == etLogicNot) || (curtype == etInteger) ||
            (curtype == etString) || (curtype == etBoolean) ||
            (curtype == etConstName) || (curtype == etVarName) || 
            (curtype == etFuncName) || (curtype == etFuncVarName))
        {
            NEXT_TOKEN_ERR
        }
        break;
    case etFuncName://����ֻ�ܸ�"("
        if (curtype != etLBracket)
        {
            parser->lex.PrintError(210, "function must have \"()\"");
            curtype = etError;
        }
        break;
    }
    
    //���ؽ��
    return (curtype == etError ? M_SC_ERR : M_SC_OK);
}


