//语法分析器
#include "SCParser.h"


//------------------------------------------------------
// 语法分析的局部宏
//------------------------------------------------------

//判断是需要的记号的宏
#define TkIS(t) (parser->lex.token.str == t)

//打印调试信息的行号
#define LIN(obj) obj.lin, obj.col

//出栈一个运算符
#define POP_A_OPER \
    if (PopAOper() != M_SC_OK) \
    { \
        return M_SC_ERR; \
    }

//下一个表达式元素跟随错误
#define NEXT_TOKEN_ERR \
    parser->lex.PrintError(209, "\"%s\" can't follow \"%s\"", \
        parser->lex.token.str.S(), EExprElementStr[oldtype]); \
    curtype = etError;

//当前所处的函数
#define FUNC parser->func


//------------------------------------------------------
// 表达式解析类
//------------------------------------------------------
    
//表达式运算，是表达式解析类的唯一入口
int CSCExpression::Calc(const ESymbolType needtype)
{
    //局部变量定义
    int result;

    //初始化表达式解析的数据
    curtype = etError;
    operatorstk.Clear();
    symbolstk.Clear();
    
    //循环的分析表达式的每个元素
    while ((result = GetNextElem()) == M_SC_OK)
    {
        switch (curtype)
        {
        case etLBracket   :  //左括号(
            PushOperator();
            break;
        case etRBracket   :  //右括号)
            if (PopWith(curtype) != M_SC_OK)
            {
                return M_SC_ERR;
            }
            break;
        case etComma      :  //逗号
        case etSign       :  //正负号+、-
        case etPluMin     :  //加减号+、-
        case etMulDiv     :  //乘除号*、/、%
        case etCompare    :  //比较符号>、<、>=、<=
        case etCompareEqu :  //等于比较符号!=、==
        case etLogicAnd   :  //逻辑运算&&
        case etLogicOr    :  //逻辑运算||
        case etLogicNot   :  //逻辑非运算!
            if (PopWith(curtype) != M_SC_OK)
            {
                return M_SC_ERR;
            }
            PushOperator();
            break;
        case etInteger    :  //整数常数
        case etString     :  //字符串常数
        case etBoolean    :  //布尔常数
        case etConstName  :  //常量名
        case etVarName    :  //变量名
        case etFuncName   :  //函数名
        case etFuncVarName:  //函数参数或局部变量名
            PushSymbol();
            break;
        }
    }

    //获取表达式元素出错
    if (result == M_SC_ERR)
    {
        return M_SC_ERR;
    }

    //把堆栈里没有运算的对象全运算出来
    if (PopAll() != M_SC_OK)
    {
        return M_SC_ERR;
    }

    //检查返回值类型
    switch (needtype)
    {
    case stConstInteger: //整数常量
    case stConstString : //字符串常量
    case stConstBoolean: //布尔值常量
        if ((symbolstk.GetCount() != 1) ||
            (needtype != symbolstk[0].type))
        {
            parser->lex.PrintError(210, "expression must return a \"%s\" value!",
                ESymbolStr[needtype]);
            return M_SC_ERR;
        }
        break;
    case stVarInteger  : //整数变量
    case stVarString   : //字符串变量
    case stVarBoolean  : //布尔值变量
        if ((symbolstk.GetCount() != 1) ||
            (needtype != symbolstk[0].VarType()))
        {
            parser->lex.PrintError(210, "expression must return a \"%s\" value!",
                ESymbolStr[needtype]);
            return M_SC_ERR;
        }
        break;
    case stVoid        : //未指定
        if (symbolstk.GetCount() != 0)
        {
            parser->lex.PrintError(211, "this expression can't return value!");
            return M_SC_ERR;
        }
    }
        
    //表达式解析成功
    return M_SC_OK;
}

//当前记号进运算符栈，Calc的私有函数
void CSCExpression::PushOperator()
{
    //添加运算符及其基本属性
    CSCOperator &oper = operatorstk.AddOper(curtype, parser->lex);

    //分析运算符的具体类型
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

//当前记号进符号栈，Calc的私有函数
void CSCExpression::PushSymbol()
{
    //添加符号及其基本属性
    CSCCodeSymbol &symb = symbolstk.AddSymb();

    //分析符号的取值
    switch (curtype)
    {
    case etInteger    :  //整数常数
        symb.type = stConstInteger;
        symb.value = parser->lex.token.str.I();
        break;
    case etString     :  //字符串常数
        symb.type = stConstString;
        symb.str = parser->lex.token.str;
        break;
    case etBoolean    :  //布尔常数
        symb.type = stConstBoolean;
        symb.value = TkIS("true");
        break;
    case etConstName  :  //常量名
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
    case etVarName    :  //全局变量名
    case etFuncName   :  //函数名
        symb.type = parser->tempsymbol->type;
        symb.symbol = parser->tempsymbol;
        break;
    case etFuncVarName:  //函数参数或局部变量名
        symb.type = parser->tempsymbol->type;
        symb.value = parser->func->symbols.IndexOf(*parser->tempsymbol);
        break;
    }
}

//复制符号进栈，Calc的私有函数
void CSCExpression::PushSymbol(const CSCCodeSymbol &symbol)
{
    symbolstk.Add();
    symbolstk.Last() = symbol;
}

//出栈一个单目运算，PopAOper的私有函数
int CSCExpression::PopOne(CSCCode &code, const CSCOperator &oper)
{
    //入口判断
    if (symbolstk.GetCount() < 1)
    {
        parser->lex.PrintErrorLin(LIN(oper), 302, 
            "\"%s\" not one symbol!", EOperatorSign[oper.oper]);
        return M_SC_ERR;
    }

    //保存符号1
    code.symb1 = symbolstk.Last();
    symbolstk.DeleteLast();

    //执行运算
    switch (oper.oper)
    {
    case otSignPlu:  //正号+
    case otSignMin:  //负号-
        switch (code.symb1.type)
        {
        case stConstInteger:
            //整数常量直接运算，不用输出中间代码
            PushSymbol(code.symb1);
            if (oper.oper == otSignMin)
            {
                symbolstk.Last().value = -symbolstk.Last().value;
            }
            FUNC->codes.DeleteLast();
            break;
        case stVarInteger:
            //整数变量需要保存在中间变量里
            parser->VarFreeTemp(code.symb1);
            parser->VarAllocTemp(stVarInteger, code.result);
            PushSymbol(code.result);
            break;
        default:
            //错误！
            parser->lex.PrintErrorLin(LIN(oper), 304, 
                "\"%s\" only for integer!", EOperatorSign[oper.oper]);
            return M_SC_ERR;
        }
        break;
    case otNot:  //!
        switch (code.symb1.type)
        {
        case stConstBoolean:
            //布尔常量直接运算，不用输出中间代码
            PushSymbol(code.symb1);
            symbolstk.Last().value = !symbolstk.Last().value;
            FUNC->codes.DeleteLast();
            break;
        case stVarBoolean:
            //布尔变量需要保存在中间变量里
            parser->VarFreeTemp(code.symb1);
            parser->VarAllocTemp(stVarBoolean, code.result);
            PushSymbol(code.result);
            break;
        default:
            //错误！
            parser->lex.PrintErrorLin(LIN(oper), 305, 
                "\"%s\" only for boolean!", EOperatorSign[oper.oper]);
            return M_SC_ERR;
        }
        break;
    }

    //成功返回
    return M_SC_OK;
}

//出栈一个双目运算，PopAOper的私有函数
int CSCExpression::PopTwo(CSCCode &code, const CSCOperator &oper)
{
    //入口判断
    if (symbolstk.GetCount() < 2)
    {
        parser->lex.PrintErrorLin(LIN(oper), 303, 
            "\"%s\" need two symbols!", EOperatorSign[oper.oper]);
        return M_SC_ERR;
    }

    //保存符号2
    code.symb2 = symbolstk.Last();
    symbolstk.DeleteLast();

    //保存符号1
    code.symb1 = symbolstk.Last();
    symbolstk.DeleteLast();

    //不支持不同数据类型间的隐式转化
    if ((code.symb1.VarType() != code.symb2.VarType()) ||
        (code.symb1.VarType() == stVoid))
    {
        parser->lex.PrintErrorLin(LIN(oper), 305, 
            "\"%s\" can not for different data type!", EOperatorSign[oper.oper]);
        return M_SC_ERR;
    }

    //数据类型效验
    switch (oper.oper)
    {
    case otPlu:  //加号+
    case otMin:  //减号-
    case otMul:  //乘号*
    case otDiv:  //除号/
    case otMod:  //模号%
        //int & int = int
        if (!code.symb1.IsInt())
        {
            //"+"还支持：str & str = str
            if ((oper.oper != otPlu) || !code.symb1.IsStr())
            {
                parser->lex.PrintErrorLin(LIN(oper), 308, 
                    "\"%s\" can not for string/boolean data type!", EOperatorSign[oper.oper]);
                return M_SC_ERR;
            }
        }

        //消除零除问题
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

    //常数运算，不用输出中间代码
    if (!code.symb1.IsVar() && !code.symb2.IsVar())
    {
        PushSymbol(code.symb1);
        switch (oper.oper)
        {
        case otPlu:  //加号+
            if (code.symb1.IsInt())
            {
                symbolstk.Last().value += code.symb2.value;
            }
            else
            {
                symbolstk.Last().str += code.symb2.str;
            }
            break;
        case otMin:  //减号-
            symbolstk.Last().value -= code.symb2.value;
            break;
        case otMul:  //乘号*
            symbolstk.Last().value *= code.symb2.value;
            break;
        case otDiv:  //除号/
            symbolstk.Last().value /= code.symb2.value;
            break;
        case otMod:  //模号%
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
        //变量运算，首先释放旧的临时变量
        parser->VarFreeTemp(code.symb1);
        parser->VarFreeTemp(code.symb2);
        switch (oper.oper)
        {
        case otMore    :  //>，这四个运算符的结果和值类型不同
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

    //成功返回
    return M_SC_OK;

}

//出栈一个函数，PopAOper的私有函数
int CSCExpression::PopFunc(CSCCode &code)
{
    //局部变量定义
    int i, j;    //通用循环变量
    CSCCodeSymbol *symb = NULL;  //当前的函数符号
    CSCFunction *function = NULL;//当前的函数指针

    //查找这个函数
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

    //参数数量
    function = symb->symbol->func();
    if (function->paras != symbolstk.GetCount() - 1 - i)
    {
        parser->lex.PrintErrorLin(LIN(code), 312, 
            "function \"%s\" need %d parameters!",
            function->name.S(), function->paras);
        return M_SC_ERR;        
    }
    
    //参数数据类型效验，通过则入栈
    for (j = 1, i++; j <= function->paras; i++, j++)
    {
        //数据类型效验
        if (symbolstk[i].VarType() != function->symbols[j].type)
        {
            parser->lex.PrintErrorLin(LIN(code), 313, 
                "function \"%s\" parameter %d need %s but give %s!",
                function->name.S(), j, 
                ESymbolStr[function->symbols[j].type], ESymbolStr[symbolstk[i].type]);
            return M_SC_ERR;        
        }

        //添加函数参数压栈指令
        CSCCode &para = FUNC->codes.AddCode(otAssign);
        para.lin = code.lin;
        para.col = code.col;
        para.symb1 = symbolstk[i];
        parser->VarFreeTemp(symbolstk[i]);
        para.result.type = function->symbols[j].type;
        para.result.value = j;
        para.result.str = function->symbols[j].name;
    }

    //函数及其参数出栈
    for (i = 0; i <= function->paras; i++)
    {
        symbolstk.DeleteLast();
    }

    //处理函数的返回值
    if (function->symbols[0].type != stVoid)
    {
        parser->VarAllocTemp(function->symbols[0].type, code.result);
        PushSymbol(code.result);
    }
        
    //成功返回
    return M_SC_OK;

}

//出栈一行中间代码，Calc的私有函数
int CSCExpression::PopAOper()
{
    //入口判断
    if (operatorstk.GetCount() <= 0)
    {
        parser->lex.PrintError(301, 
            "not operator when pop expression!");
        return M_SC_ERR;
    }

    //添加一行中间代码
    CSCOperator &oper = operatorstk.Last();
    CSCCode &code = FUNC->codes.AddCode(oper.oper);
    code.lin = oper.lin;
    code.col = oper.col;
    int result = M_SC_OK;

    //根据操作符栈顶元素判断如何出栈
    switch (oper.oper)
    {
    case otFuncLBra:  //函数左括号(
        //函数
        result = PopFunc(code);
        break;
    case otSignPlu :  //正号+
    case otSignMin :  //负号-
    case otNot     :  //!
        //单目运算
        result = PopOne(code, oper);
        break;
    case otPlu     :  //加号+
    case otMin     :  //减号-
    case otMul     :  //乘号*
    case otDiv     :  //除号/
    case otMod     :  //模号%
    case otMore    :  //>
    case otLess    :  //<
    case otMoreEqu :  //>=
    case otLessEqu :  //<=
    case otNotEqu  :  //!=
    case otEqu     :  //==
    case otOr      :  //||
    case otAnd     :  //&&
        //双目运算
        result = PopTwo(code, oper);
        break;
    default:
        //其他运算只要删除符号栈就可以了，不用输出中间代码
        FUNC->codes.DeleteLast();
        break;
    }

    //删除运算符栈顶元素
    operatorstk.DeleteLast();
    
    //成功返回
    return result;
}

//一直出栈到与oper匹配的运算符，Calc的私有函数
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

    //括号匹配检查
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

    //成功返回
    return M_SC_OK;
}

//所有表达式都出栈，Calc的私有函数
int CSCExpression::PopAll()
{
    //所有的运算符出栈
    while (operatorstk.GetCount() > 0)
    {
        //不能出现"("，因为"("都会被匹配的")"出栈
        CSCOperator &oper = operatorstk.Last();
        if (oper.oper == otLBracket)
        {
            parser->lex.PrintErrorLin(LIN(oper), 321, 
                "\"(\" need matched \")\"");
            return M_SC_ERR;
        }

        //当前栈顶符号出栈
        POP_A_OPER
    }

    //分析解析结果
    if (symbolstk.GetCount() > 1)
    {
        parser->lex.PrintError(322, "unwanted symbol \"%s\"", 
            ESymbolStr[symbolstk[1].type]);
        return M_SC_ERR;
    }
    
    //临时变量释放
    if (symbolstk.GetCount() > 0)
    {
        parser->VarFreeTemp(symbolstk[0]);
    }

    //成功返回
    return M_SC_OK;
}

//获得当前记号的表达式元素类型，Calc的私有函数
//根据语法，表达式末尾符号可能是 ; { : 三种
int CSCExpression::GetNextElem()
{
    //分析表达式元素的类型
    EExprElementType oldtype = curtype;
    curtype = etError;

    //如果不是初始化状态则提取下一个元素
    if (oldtype != etError)
    {
        if (parser->lex.GetNextToken() != M_SC_POK)
        {
            return M_SC_ERR;
        }
    }
    
    //分析每个记号    
    switch (parser->lex.token.type)
    {
    case ttInteger://整数
        curtype = etInteger;
        break;
    case ttString://字符串
        curtype = etString;
        break;
    case ttOperator://操作符
        if (TkIS(";") || TkIS("{") || TkIS(":"))
        {
            //;和{表示表达式完成，用POK表示
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
            //只有这5种表达式元素后面可以跟加减
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
    case ttWord://单词
        //判断是不是全局变量、全局常量或函数名
        curtype = parser->SymbolLookup();

        //没有找到定义
        if (curtype == etError)
        {
            parser->lex.PrintError(207, 
                "unknown const/variable/function \"%s\" in expression!",
                parser->lex.token.str.S());
            return M_SC_ERR;
        }
        
        break;
    case ttKeyword://关键字
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

    //根据上一个元素类型判断新元素是否合法
    switch (oldtype)
    {
    case etLBracket://"("不能跟","、双目运算符
        if ((curtype == etComma) || (curtype == etPluMin) ||
            (curtype == etMulDiv) || (curtype == etCompare) ||
            (curtype == etCompareEqu) || (curtype == etLogicAnd) || 
            (curtype == etLogicOr))
        {
            NEXT_TOKEN_ERR
        }
        break;
    case etRBracket://")"不能跟"("、单目运算符、常变量
        if ((curtype == etLBracket) || (curtype == etSign) ||
            (curtype == etLogicNot) || (curtype == etInteger) ||
            (curtype == etString) || (curtype == etBoolean) ||
            (curtype == etConstName) || (curtype == etVarName) ||
            (curtype == etFuncName) || (curtype == etFuncVarName))
        {
            NEXT_TOKEN_ERR
        }
        break;
    case etComma://","不能跟")"、","、双目运算符
        if ((curtype == etRBracket) || (curtype == etComma) ||
            (curtype == etPluMin) || (curtype == etMulDiv) ||
            (curtype == etCompare) || (curtype == etCompareEqu) || 
            (curtype == etLogicAnd) || (curtype == etLogicOr))
        {
            NEXT_TOKEN_ERR
        }
        break;
    case etSign://加减号不能跟")"、","、运算符、非整数
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
    case etPluMin://双目运算符不能跟")"、","、双目运算符
    case etMulDiv:
    case etCompare:
    case etCompareEqu:
    case etLogicAnd:
    case etLogicOr:
    case etLogicNot://单目运算符"!"的禁止表和etLogic一样
        if ((curtype == etRBracket) || (curtype == etComma) ||
            (curtype == etPluMin) || (curtype == etMulDiv) ||
            (curtype == etCompare) || (curtype == etCompareEqu) || 
            (curtype == etLogicAnd) || (curtype == etLogicOr))
        {
            NEXT_TOKEN_ERR
        }
        break;
    case etInteger://常变量不能跟"("、单目运算符、常变量
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
    case etFuncName://函数只能跟"("
        if (curtype != etLBracket)
        {
            parser->lex.PrintError(210, "function must have \"()\"");
            curtype = etError;
        }
        break;
    }
    
    //返回结果
    return (curtype == etError ? M_SC_ERR : M_SC_OK);
}


