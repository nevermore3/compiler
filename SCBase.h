//------------------------------------------------------
// SC��̬�ű����Եı����������������Ļ��������⣩
//   ���ߣ�����27651
//   ʱ�䣺2008-12-09
//------------------------------------------------------

#ifndef _SC_BASE_H
#define _SC_BASE_H

//����������
#include "normal/normal.h"

//�����붨��
#define M_SC_OK  0   //ִ�гɹ�(��:ִ�к����ɹ�)
#define M_SC_ERR 1   //ִ��ʧ��
#define M_SC_POK 2   //ִ�н׶��Գɹ�(��:ִ�к�����1��ָ��ɹ�)
#define M_SC_BRK 3   //ִ�б��ϵ��ס
#define M_SC_FUC 4   //ִ�е����ú��������ϼ���������
#define M_SC_QUT 5   //ǿ���˳�����ִ��

//��ʱ�����ĺ�
#define M_SC_MAX_TEMP 100   //������������ĳ����ʱ��������
#define M_SC_FREE     0     //δ��ʹ�õ���ʱ����
#define M_SC_USE      1     //�Ѿ���ʹ�õ���ʱ����

//�Ǻ�����
typedef enum
{
    ttInteger = 0,  //����
    ttString,       //�ַ���
    ttOperator,     //������
    ttKeyword,      //�ؼ���
    ttWord,         //����
    ttError         //����
} ETokenType;
extern const char * ETokenStr[ttError + 1];

//�ַ�����
typedef enum
{
    ctSpace = 0,    //�ո�
    ctZero,         //'\0'
    ctNumber,       //���֣�0-9
    ctLetter,       //��ĸ��a-z, A-Z, _
    ctString,       //˫���ţ�'\"'
    ctSign,         //��������ע�ͣ�-!%&()*,/;{|}+<=>
    ctError         //����
} ECharType;
extern const char * ECharStr[ctError + 1];

//��������
typedef enum
{
    stVarInteger = 0,//��������
    stVarString,     //�ַ�������
    stVarBoolean,    //����ֵ����
    stConstInteger,  //��������
    stConstString,   //�ַ�������
    stConstBoolean,  //����ֵ����
    stFunction,      //����
    stVoid           //δָ��
} ESymbolType;
extern const char * ESymbolStr[stVoid + 1];

//���ʽ��Ԫ������
typedef enum
{
    etLogicNot = 0, //�߼�������!
    etSign,         //������+��-
    etMulDiv,       //�˳���*��/��%
    etPluMin,       //�Ӽ���+��-
    etCompare,      //�ȽϷ���>��<��>=��<=
    etCompareEqu,   //���ڱȽϷ���!=��==
    etLogicAnd,     //�߼�����&&
    etLogicOr,      //�߼�����||
    etLBracket,     //������(
    etRBracket,     //������)
    etComma,        //����,
    etInteger,      //��������
    etString,       //�ַ�������
    etBoolean,      //��������
    etConstName,    //������
    etVarName,      //������
    etFuncName,     //������
    etFuncVarName,  //����������ֲ�������
    etError         //�������ַ�
} EExprElementType;
extern const char * EExprElementStr[etError + 1];

//���ʽ�����������
typedef enum
{
    otLBracket = 0, //������(
    otFuncLBra,     //����������(
    otComma,        //����,
    otSignPlu,      //����+
    otSignMin,      //����-
    otPlu,          //�Ӻ�+
    otMin,          //����-
    otMul,          //�˺�*
    otDiv,          //����/
    otMod,          //ģ��%
    otMore,         //>
    otLess,         //<
    otMoreEqu,      //>=
    otLessEqu,      //<=
    otNotEqu,       //!=
    otEqu,          //==
    otOr,           //||
    otAnd,          //&&
    otNot,          //!
    otAssign,       //��ֵ
    otReturn,       //����
    otGoto,         //��������ת
    otGoNot,        //����Ϊ��ʱ��ת
    otVoid          //�޲���
} EOperatorType;
extern const char * EOperatorStr[otVoid + 1];
extern const char * EOperatorSign[otVoid + 1];

//�������﷨�ṹ����
typedef enum
{
    ftIf = 0,  //If�������
    ftFor,     //Forѭ�����
    ftWhile,   //Whileѭ�����
    ftDoWhile, //Do...Whileѭ�����
    ftSwitch,  //Switch��֧���
    ftError    //������
} EStatementType;
extern const char * EStatementStr[ftError + 1];

#endif  /* _SC_BASE_H */

