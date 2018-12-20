//虚拟机
#include "SCVirtualMachine.h"


//初始化符号表，LoadScript的私有函数
int CSCVirtualMachine::InitSymbols()
{
    //清空旧的符号表
    symbols.Clear();
    
    //必须添加一个系统的初始化函数
    symbols.AddSymb("@init", stFunction);

    //然后添加系统内建函数列表
    CSCParser parser;
    parser.symbols = &symbols;

    //执行词法分析
    if (parser.Parse("init.sc") != M_SC_OK)
    {
        printf("\r\ncompiler error!\r\n");
        return M_SC_ERR;
    }

    //成功返回结果
    return M_SC_OK;
}

//加载与编译代码文件（包括词法分析、语法分析）
int CSCVirtualMachine::LoadScript(const char *filename)
{
    //初始化符号表
    if (InitSymbols() != M_SC_OK)
    {
        printf("\r\ninit symbol table error!\r\n");
        return M_SC_ERR;
    }

    //语法分析器类
    CSCParser parser;
    parser.symbols = &symbols;

    //执行词法分析
    int result = parser.Parse(filename);

    //分析编译是否失败
    if (result != M_SC_OK)
    {
        printf("\r\ncompiler error!\r\n");
        return M_SC_ERR;
    }

    //返回结果
    return M_SC_OK;
}

//初始化函数调用栈，RunScript的私有函数
int CSCVirtualMachine::InitCallstk()
{
    //清空旧的函数调用栈
    callstk.Clear();
    
    //首先把0号(入口函数@init)压栈
    callstk.PushCall(symbols[0].func());

    //查找main函数
    CSCSymbol *fmain = symbols.Lookup("main");
    if (fmain == Null)
    {
        printf("\r\ncan not find main()!\r\n");
        return M_SC_ERR;
    }

    //在入口函数@init中调用main函数
    CSCCodeTable &codes = symbols[0].func()->codes;
    if ((codes.GetCount() < 2) || (codes.Last().oper != otReturn))
    {
        CSCCode &code = codes.AddCode(otFuncLBra);
        code.symb1.symbol = fmain;
        code.symb1.type = stFunction;
        code.lin = fmain->lin;
        codes.AddCode(otReturn).lin = fmain->lin;
    }

    //成功返回结果
    return M_SC_OK;
}

//执行中间代码
int CSCVirtualMachine::RunScript()
{
    //初始化函数调用栈
    if (InitCallstk() != M_SC_OK)
    {
        printf("\r\ninit call stack error!\r\n");
        return M_SC_ERR;
    }

    //看看即将执行的代码
    symbols.Debug(False);

    //开始执行中间代码
    int ret = callstk.Run();
    if (!IN_2(ret, M_SC_OK, M_SC_QUT))
    {
        printf("\r\nexecute script error!\r\n");
        return M_SC_ERR;
    }
    return M_SC_OK;
}

