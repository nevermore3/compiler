//------------------------------------------------------
// SC��̬�ű����Եı���������������ջ��
//   ���ߣ�����27651
//   ʱ�䣺2009-05-05
//------------------------------------------------------

#ifndef _SC_CALL_STACK_H
#define _SC_CALL_STACK_H

//���ű�
#include "SCSymbolTable.h"


//------------------------------------------------------
// ��������ջԪ��
//------------------------------------------------------
struct CSCCall
{
    CSCFunction *func;     //���õĺ���ָ��
    CSCSymbolTable symbols;//�����ľֲ�����
    int pos;               //������ǰִ�еĴ�����±�
    
    //�޲ι��캯��
    CSCCall() {func = Null; pos = 0;}
    
    //���ڵ���
    void Debug();
    
    //ϵͳ���õ��ڲ�����
    int SysCall();

    //ִ���м���룬M_SC_OK:�ɹ���ERR:ʧ�ܣ�FUC:ִ�е����ú���
    int Run();

    //ִ��һ��ָ�Run��˽�к���
    int RunCode(CSCCode &code);

    //��������
    int GetInt(CSCCodeSymbol &symb);
    void SetInt(CSCCodeSymbol &symb, int _value);

    //�ַ�������
    CNString GetStr(CSCCodeSymbol &symb);
    void SetStr(CSCCodeSymbol &symb, CNString _str);
};


//------------------------------------------------------
// ��������ջ
//------------------------------------------------------
struct CSCCallStack : public CNVector<CSCCall>
{
    //���һ������
    CSCCall &PushCall(CSCFunction *func);
    
    //ɾ��һ������
    void PopCall();
    
    //���ڵ���
    void Debug();

    //ִ���м���룬M_SC_OK:�ɹ���ERR:ʧ��
    int Run();
};


#endif  /* _SC_CALL_STACK_H */

