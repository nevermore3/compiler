//��������ջ
#include "SCCallStack.h"


//------------------------------------------------------
// ��������ջԪ��
//------------------------------------------------------

void CSCCall::Debug()
{
    printf("-> %s() - line:", func->name.S());
    if ((pos >= 0) && (pos < func->codes.GetCount()))
    {
        printf("%d", func->codes[pos].lin + 1);
    }
}

//ϵͳ���õ��ڲ��⺯��
int CSCCall::SysCall()
{
    //�ڲ��⺯��
    if (func->name == "printf")
    {
        printf(symbols[1].str()->S());
    }
    else if (func->name == "scanf")
    {
        char str[1024];
        scanf("%[^\n]", str);
        fflush(stdin);
        *(symbols[0].str()) = str;
    }
    else if (func->name == "lowercase")
    {
        *(symbols[0].str()) = symbols[1].str()->ToLower();
    }
    else if (func->name == "uppercase")
    {
        *(symbols[0].str()) = symbols[1].str()->ToUpper();
    }
    else if (func->name == "str_int")
    {
        symbols[0].value = symbols[1].str()->I();
    }
    else if (func->name == "int_str")
    {
        *(symbols[0].str()) = symbols[1].value;
    }
    else if (func->name == "str_bool")
    {
        symbols[0].value = symbols[1].str()->NoCaseCompare("true");
    }
    else if (func->name == "bool_str")
    {
        *(symbols[0].str()) = symbols[1].value ? "true" : "false";
    }
    else if (func->name == "int_bool")
    {
        symbols[0].value = symbols[1].value;
    }
    else if (func->name == "bool_int")
    {
        symbols[0].value = symbols[1].value;
    }
    else
    {
        printf("\r\ncan not find system function \"%s\"!\r\n", func->name.S());
    }
    return M_SC_OK;
}

//ִ���м���룬M_SC_OK:�ɹ���ERR:ʧ�ܣ�FUC:ִ�е����ú���
int CSCCall::Run()
{
    //ϵͳ���ú���
    if (func->syscall)
    {
        return SysCall();
    }

    //���м����ĺ���
    while ((pos >= 0) && (pos < func->codes.GetCount()))
    {
        int ret = RunCode(func->codes[pos]);
        if (ret != M_SC_POK)
        {
            return ret;
        }
    }

    //pos�����˴���λ��
    return M_SC_ERR;
}

//��ָͨ��: ִ�к�ָ����Ҫ++������M_SC_POK
#define M_RET_POK pos++; return M_SC_POK;

//ִ��һ��ָ�Run��˽�к���
int CSCCall::RunCode(CSCCode &code)
{
    switch (code.oper)
    {
    case otFuncLBra: //����������(
        return M_SC_FUC;
    case otSignPlu : //����+
        SetInt(code.result, GetInt(code.symb1));
        M_RET_POK;
    case otSignMin : //����-
        SetInt(code.result, -GetInt(code.symb1));
        M_RET_POK;
    case otPlu     : //�Ӻ�+
        if (code.result.IsStr())
        {
            SetStr(code.result, GetStr(code.symb1) + GetStr(code.symb2));
        }
        else
        {
            SetInt(code.result, GetInt(code.symb1) + GetInt(code.symb2));
        }
        M_RET_POK;
    case otMin     : //����-
        SetInt(code.result, GetInt(code.symb1) - GetInt(code.symb2));
        M_RET_POK;
    case otMul     : //�˺�*
        SetInt(code.result, GetInt(code.symb1) * GetInt(code.symb2));
        M_RET_POK;
    case otDiv     : //����/
        if (GetInt(code.symb2) == 0)
        {
            printf("\r\n[%d-%d] / div 0 error!\r\n", code.lin + 1, code.col + 1);
            return M_SC_ERR;
        }
        SetInt(code.result, GetInt(code.symb1) / GetInt(code.symb2));
        M_RET_POK;
    case otMod     : //ģ��%
        if (GetInt(code.symb2) == 0)
        {
            printf("\r\n[%d-%d] % div 0 error!\r\n", code.lin + 1, code.col + 1);
            return M_SC_ERR;
        }
        SetInt(code.result, GetInt(code.symb1) % GetInt(code.symb2));
        M_RET_POK;
    case otMore    : //>
        SetInt(code.result, GetInt(code.symb1) > GetInt(code.symb2));
        M_RET_POK;
    case otLess    : //<
        SetInt(code.result, GetInt(code.symb1) < GetInt(code.symb2));
        M_RET_POK;
    case otMoreEqu : //>=
        SetInt(code.result, GetInt(code.symb1) >= GetInt(code.symb2));
        M_RET_POK;
    case otLessEqu : //<=
        SetInt(code.result, GetInt(code.symb1) <= GetInt(code.symb2));
        M_RET_POK;
    case otNotEqu  : //!=
        SetInt(code.result, GetInt(code.symb1) != GetInt(code.symb2));
        M_RET_POK;
    case otEqu     : //==
        SetInt(code.result, GetInt(code.symb1) == GetInt(code.symb2));
        M_RET_POK;
    case otOr      : //||
        SetInt(code.result, GetInt(code.symb1) || GetInt(code.symb2));
        M_RET_POK;
    case otAnd     : //&&
        SetInt(code.result, GetInt(code.symb1) && GetInt(code.symb2));
        M_RET_POK;
    case otNot     : //!
        SetInt(code.result, !GetInt(code.symb1));
        M_RET_POK;
    case otAssign  : //��ֵ
        if (code.result.IsStr())
        {
            SetStr(code.result, GetStr(code.symb1));
        }
        else
        {
            SetInt(code.result, GetInt(code.symb1));
        }
        M_RET_POK;
    case otReturn  : //����
        return M_SC_OK;
    case otGoto    : //��������ת
        pos = GetInt(code.result);
        return M_SC_POK;
    case otGoNot   : //����Ϊ��ʱ��ת
        if (!GetInt(code.symb1))
        {
            pos = GetInt(code.result);
            return M_SC_POK;
        }
        M_RET_POK;
    }
    return M_SC_ERR;
}

//����/��������
int CSCCall::GetInt(CSCCodeSymbol &symb)
{
    if (symb.symbol == NULL)
    {
        if (symb.IsVar())
        {
            return symbols[symb.value].value;
        }
        else
        {
            return symb.value;
        }
    }
    else
    {
        return symb.symbol->value;
    }
}
void CSCCall::SetInt(CSCCodeSymbol &symb, int _value)
{
    if (symb.symbol == NULL)
    {
        symbols[symb.value].value = _value;
    }
    else
    {
        symb.symbol->value = _value;
    }
}

//�ַ�������
CNString CSCCall::GetStr(CSCCodeSymbol &symb)
{
    if (symb.symbol == NULL)
    {
        if (symb.IsVar())
        {
            return *(symbols[symb.value].str());
        }
        else
        {
            return symb.str;
        }
    }
    else
    {
        return *(symb.symbol->str());
    }
}
void CSCCall::SetStr(CSCCodeSymbol &symb, CNString _str)
{
    if (symb.symbol == NULL)
    {
        *(symbols[symb.value].str()) = _str;
    }
    else
    {
        *(symb.symbol->str()) = _str;
    }
}


//------------------------------------------------------
// ��������ջ
//------------------------------------------------------

//���һ������
CSCCall &CSCCallStack::PushCall(CSCFunction *func)
{
    Add();
    CSCCall &call = Last();
    call.func = func;
    call.pos = 0;

    //���Ʒ��ű�
    for (int i = 0; i < func->symbols.GetCount(); i++)
    {
        call.symbols.AddSymb(func->symbols[i]);
    }

    //���ؽ��
    return call;
}

//ɾ��һ������
void CSCCallStack::PopCall()
{
    DeleteLast();
}

//���ڵ���
void CSCCallStack::Debug()
{
    int i, j;
    printf("Call Stack : %d\r\n", GetCount());
    for (i = 0; i < GetCount(); i++)
    {
        for (j = 0; j <= i; j++)
        {
            printf("  ");
        }
        Get(i).Debug();
        printf("\r\n");
    }
    printf("\r\n");
}

//ִ���м���룬M_SC_OK:�ɹ���ERR:ʧ��
int CSCCallStack::Run()
{
    //����ִ�����ѹջ�ĺ���
    CSCCall &call = Last();
    int i, ret;
    
    //��ѭ��ִ���м�ָ��
    while (True)
    {
        //ִ�к���
        if ((ret = call.Run()) != M_SC_FUC) return ret;

        //�Ӻ�����ջ�������ݲ���
        CSCCode &fcode = call.func->codes[call.pos++];
        CSCCall &subcall = PushCall(fcode.symb1.symbol->func());
        for (i = 1; i <= subcall.func->paras; i++)
        {
            CSCCode &pcode = call.func->codes[call.pos++];
            if (subcall.symbols[i].type == stVarString)
            {
                *(subcall.symbols[i].str()) = call.GetStr(pcode.symb1);
            }
            else
            {
                subcall.symbols[i].value = call.GetInt(pcode.symb1);
            }
        }

        //ִ���Ӻ������ж����н��
        if ((ret = Run()) != M_SC_OK) return ret;
        
        //����Ӻ����Ľ������ջ
        if (fcode.result.IsVar())
        {
            if (fcode.result.IsStr())
            {
                call.SetStr(fcode.result, *(subcall.symbols[0].str()));
            }
            else
            {
                call.SetInt(fcode.result, subcall.symbols[0].value);
            }
        }
        PopCall();
    }
}

