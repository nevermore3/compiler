//�����
#include "SCVirtualMachine.h"


//��ʼ�����ű�LoadScript��˽�к���
int CSCVirtualMachine::InitSymbols()
{
    //��վɵķ��ű�
    symbols.Clear();
    
    //�������һ��ϵͳ�ĳ�ʼ������
    symbols.AddSymb("@init", stFunction);

    //Ȼ�����ϵͳ�ڽ������б�
    CSCParser parser;
    parser.symbols = &symbols;

    //ִ�дʷ�����
    if (parser.Parse("init.sc") != M_SC_OK)
    {
        printf("\r\ncompiler error!\r\n");
        return M_SC_ERR;
    }

    //�ɹ����ؽ��
    return M_SC_OK;
}

//�������������ļ��������ʷ��������﷨������
int CSCVirtualMachine::LoadScript(const char *filename)
{
    //��ʼ�����ű�
    if (InitSymbols() != M_SC_OK)
    {
        printf("\r\ninit symbol table error!\r\n");
        return M_SC_ERR;
    }

    //�﷨��������
    CSCParser parser;
    parser.symbols = &symbols;

    //ִ�дʷ�����
    int result = parser.Parse(filename);

    //���������Ƿ�ʧ��
    if (result != M_SC_OK)
    {
        printf("\r\ncompiler error!\r\n");
        return M_SC_ERR;
    }

    //���ؽ��
    return M_SC_OK;
}

//��ʼ����������ջ��RunScript��˽�к���
int CSCVirtualMachine::InitCallstk()
{
    //��վɵĺ�������ջ
    callstk.Clear();
    
    //���Ȱ�0��(��ں���@init)ѹջ
    callstk.PushCall(symbols[0].func());

    //����main����
    CSCSymbol *fmain = symbols.Lookup("main");
    if (fmain == Null)
    {
        printf("\r\ncan not find main()!\r\n");
        return M_SC_ERR;
    }

    //����ں���@init�е���main����
    CSCCodeTable &codes = symbols[0].func()->codes;
    if ((codes.GetCount() < 2) || (codes.Last().oper != otReturn))
    {
        CSCCode &code = codes.AddCode(otFuncLBra);
        code.symb1.symbol = fmain;
        code.symb1.type = stFunction;
        code.lin = fmain->lin;
        codes.AddCode(otReturn).lin = fmain->lin;
    }

    //�ɹ����ؽ��
    return M_SC_OK;
}

//ִ���м����
int CSCVirtualMachine::RunScript()
{
    //��ʼ����������ջ
    if (InitCallstk() != M_SC_OK)
    {
        printf("\r\ninit call stack error!\r\n");
        return M_SC_ERR;
    }

    //��������ִ�еĴ���
    symbols.Debug(False);

    //��ʼִ���м����
    int ret = callstk.Run();
    if (!IN_2(ret, M_SC_OK, M_SC_QUT))
    {
        printf("\r\nexecute script error!\r\n");
        return M_SC_ERR;
    }
    return M_SC_OK;
}

