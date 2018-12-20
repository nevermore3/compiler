//------------------------------------------------------
// SC��̬�ű����Եı��������ʷ���������
//   ���ߣ�����27651
//   ʱ�䣺2008-12-09
//------------------------------------------------------

#ifndef _SC_LEXICALER_H
#define _SC_LEXICALER_H

//����������
#include "SCBase.h"


//------------------------------------------------------
// �Ǻ���
//------------------------------------------------------
struct CSCToken
{
    ETokenType type;//�Ǻ�����
    CNString str;   //�Ǻŵ�ֵ
    int lin;        //�Ǻ������кţ���0��ʼ
    int col;        //�Ǻ������кţ���0��ʼ
};


//------------------------------------------------------
// �ʷ���������
//------------------------------------------------------
struct CSCLexicaler
{
    CNStringList list;     //�����ļ�
    CNStringList operators;//�������б�
    CNStringList keywords; //�ؼ����б�
    int lin;               //��ǰ�кţ���0��ʼ
    int col;               //��ǰ�кţ���0��ʼ
    CSCToken token;        //��ǰ�Ǻ�
    int errors;            //��ӡ�Ĵ�������

    //���ش����ļ�
    int LoadScript(const char *filename);

    //���ζ�ȡ�Ǻţ��ɹ�����M_SC_POK��ʧ�ܷ���M_SC_ERR���ļ���������M_SC_OK
    int GetNextToken();

    //������һ�У���GetNextToken��˽�к���
    void JumpLine();

    //��ʼ��token����GetNextToken��˽�к���
    void InitToken(ETokenType type);

    //����ַ����ͣ���GetNextToken��˽�к���
    ECharType GetCharType(char c);

    //�﷨�Ƶ���ʽ�����������GetNextToken��˽�к���
    int GetInteger();

    //�﷨�Ƶ���ʽ����ַ�������GetNextToken��˽�к���
    int GetString();

    //�﷨�Ƶ���ʽ��õ��ʣ���GetNextToken��˽�к���
    int GetWord();

    //�﷨�Ƶ���ʽ��ò���������GetNextToken��˽�к���
    int GetOperator();

    //��ӡ��Ϣ��Ҳ�����﷨��������������ô˺���
    void Printf(const char* format, ...);

    //��ӡ������Ϣ��Ҳ�����﷨��������������ô˺���
    void PrintError(const int ID, const char* format, ...);

    //�Զ����кŴ�ӡ�����ӡ������Ϣ
    void PrintErrorLin(const int lin, const int col, 
        const int ID, const char* format, ...);

    //���ڵ��ԣ���ӡ��ǰ�Ǻ���Ϣ
    void Debug();
};


#endif  /* _SC_LEXICALER_H */

