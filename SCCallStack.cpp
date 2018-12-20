//函数调用栈
#include "SCCallStack.h"


//------------------------------------------------------
// 函数调用栈元素
//------------------------------------------------------

void CSCCall::Debug()
{
    printf("-> %s() - line:", func->name.S());
    if ((pos >= 0) && (pos < func->codes.GetCount()))
    {
        printf("%d", func->codes[pos].lin + 1);
    }
}

//系统调用的内部库函数
int CSCCall::SysCall()
{
    //内部库函数
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

//执行中间代码，M_SC_OK:成功，ERR:失败，FUC:执行到调用函数
int CSCCall::Run()
{
    //系统调用函数
    if (func->syscall)
    {
        return SysCall();
    }

    //有中间代码的函数
    while ((pos >= 0) && (pos < func->codes.GetCount()))
    {
        int ret = RunCode(func->codes[pos]);
        if (ret != M_SC_POK)
        {
            return ret;
        }
    }

    //pos跳到了错误位置
    return M_SC_ERR;
}

//普通指令: 执行后指令需要++，返回M_SC_POK
#define M_RET_POK pos++; return M_SC_POK;

//执行一条指令，Run的私有函数
int CSCCall::RunCode(CSCCode &code)
{
    switch (code.oper)
    {
    case otFuncLBra: //函数左括号(
        return M_SC_FUC;
    case otSignPlu : //正号+
        SetInt(code.result, GetInt(code.symb1));
        M_RET_POK;
    case otSignMin : //负号-
        SetInt(code.result, -GetInt(code.symb1));
        M_RET_POK;
    case otPlu     : //加号+
        if (code.result.IsStr())
        {
            SetStr(code.result, GetStr(code.symb1) + GetStr(code.symb2));
        }
        else
        {
            SetInt(code.result, GetInt(code.symb1) + GetInt(code.symb2));
        }
        M_RET_POK;
    case otMin     : //减号-
        SetInt(code.result, GetInt(code.symb1) - GetInt(code.symb2));
        M_RET_POK;
    case otMul     : //乘号*
        SetInt(code.result, GetInt(code.symb1) * GetInt(code.symb2));
        M_RET_POK;
    case otDiv     : //除号/
        if (GetInt(code.symb2) == 0)
        {
            printf("\r\n[%d-%d] / div 0 error!\r\n", code.lin + 1, code.col + 1);
            return M_SC_ERR;
        }
        SetInt(code.result, GetInt(code.symb1) / GetInt(code.symb2));
        M_RET_POK;
    case otMod     : //模号%
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
    case otAssign  : //赋值
        if (code.result.IsStr())
        {
            SetStr(code.result, GetStr(code.symb1));
        }
        else
        {
            SetInt(code.result, GetInt(code.symb1));
        }
        M_RET_POK;
    case otReturn  : //返回
        return M_SC_OK;
    case otGoto    : //无条件跳转
        pos = GetInt(code.result);
        return M_SC_POK;
    case otGoNot   : //条件为否时跳转
        if (!GetInt(code.symb1))
        {
            pos = GetInt(code.result);
            return M_SC_POK;
        }
        M_RET_POK;
    }
    return M_SC_ERR;
}

//整数/布尔运算
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

//字符串运算
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
// 函数调用栈
//------------------------------------------------------

//添加一个函数
CSCCall &CSCCallStack::PushCall(CSCFunction *func)
{
    Add();
    CSCCall &call = Last();
    call.func = func;
    call.pos = 0;

    //复制符号表
    for (int i = 0; i < func->symbols.GetCount(); i++)
    {
        call.symbols.AddSymb(func->symbols[i]);
    }

    //返回结果
    return call;
}

//删除一个函数
void CSCCallStack::PopCall()
{
    DeleteLast();
}

//用于调试
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

//执行中间代码，M_SC_OK:成功，ERR:失败
int CSCCallStack::Run()
{
    //总是执行最后压栈的函数
    CSCCall &call = Last();
    int i, ret;
    
    //死循环执行中间指令
    while (True)
    {
        //执行函数
        if ((ret = call.Run()) != M_SC_FUC) return ret;

        //子函数进栈，并传递参数
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

        //执行子函数并判断运行结果
        if ((ret = Run()) != M_SC_OK) return ret;
        
        //获得子函数的结果，退栈
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

