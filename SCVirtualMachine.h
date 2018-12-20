//------------------------------------------------------
// SC��̬�ű����Եı��������������
//   ���ߣ�����27651
//   ʱ�䣺2008-12-09
//------------------------------------------------------

#ifndef _SC_VIRTUAL_MACHINE_H
#define _SC_VIRTUAL_MACHINE_H

//�﷨������
#include "SCParser.h"

//��������ջ
#include "SCCallStack.h"


//------------------------------------------------------
// �������
//------------------------------------------------------
struct CSCVirtualMachine
{
    CSCSymbolTable symbols; //���ű�
    CSCCallStack callstk;   //��������ջ

    //��ʼ�����ű�LoadScript��˽�к���
    int InitSymbols();
    
    //��ʼ����������ջ��RunScript��˽�к���
    int InitCallstk();

    //�������������ļ��������ʷ��������﷨������
    int LoadScript(const char *filename);

    //ִ���м����
    int RunScript();
};

#endif  /* _SC_VIRTUAL_MACHINE_H */

