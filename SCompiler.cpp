//------------------------------------------------------
// SC��̬�ű����Եı�����
//   ���ߣ�����27651
//   ʱ�䣺2008-12-09
//------------------------------------------------------

//�����
#include "SCVirtualMachine.h"

//������
int main(int argc, char* argv[])
{
    //�������
    CSCVirtualMachine vm;

    //�������������ļ��������ʷ��������﷨������
    if (vm.LoadScript("sample.sc") != M_SC_OK)
    {
        return M_SC_ERR;
    }

    //ִ���м����
    if (vm.RunScript() != M_SC_OK)
    {
        return M_SC_ERR;
    }
    
    //����
    return M_SC_OK;
}

