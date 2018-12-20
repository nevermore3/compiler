//------------------------------------------------------
// SC��̬�ű����Եı��������м�����
//   ���ߣ�����27651
//   ʱ�䣺2009-04-09
//------------------------------------------------------

#ifndef _SC_CODE_TABLE_H
#define _SC_CODE_TABLE_H

//��������
#include "SCBase.h"

//�ʷ�������
#include "SCLexicaler.h"


//------------------------------------------------------
// ���ʽ�����
//------------------------------------------------------
struct CSCOperator
{
    EExprElementType type;//�����������
    EOperatorType oper;   //�������������
    int lin;              //�Ǻ������кţ���0��ʼ
    int col;              //�Ǻ������кţ���0��ʼ
    
    //�޲ι��캯��
    CSCOperator() {type = etError; oper = otVoid; lin = col = 0;}
};


//------------------------------------------------------
// ���ʽ�����ջ
//------------------------------------------------------
struct CSCOperatorStack : public CNVector<CSCOperator>
{
    //���һ�������
    CSCOperator &AddOper(const EExprElementType type,
        const CSCLexicaler &lex);
};


struct CSCSymbol;

//------------------------------------------------------
// ���ʽ����
//------------------------------------------------------
struct CSCCodeSymbol
{
    CSCSymbol *symbol; //ָ��ȫ�ֻ����ķ��ű��ָ��
    ESymbolType type;  //��������
    int value;         //���͡������ͳ���
    CNString str;      //�ַ����ͳ���
    
    //�޲ι��캯��
    CSCCodeSymbol() {Clear();}

    //���
    void Clear()
    {
        symbol = NULL; 
        type = stVoid; 
        value = 0; 
        str = "";
    }
    
    //�жϷ�������
    Bool IsInt();         //�ǲ�������
    Bool IsStr();         //�ǲ����ַ���
    Bool IsBool();        //�ǲ��ǲ���ֵ
    Bool IsVar();         //�ǲ��Ǳ���
    ESymbolType VarType();//����Var����

    //���ڵ���
    void Debug() const;
};


//------------------------------------------------------
// ���ʽ����ջ
//------------------------------------------------------
struct CSCSymbolStack : public CNVector<CSCCodeSymbol>
{
    //���һ������
    CSCCodeSymbol &AddSymb();
    
    //���ڵ���
    void Debug() const;
};


//------------------------------------------------------
// �м����(�沨��ʽ)
//------------------------------------------------------
struct CSCCode
{
    EOperatorType oper;   //�����
    CSCCodeSymbol symb1;  //ֵ1
    CSCCodeSymbol symb2;  //ֵ2
    CSCCodeSymbol result; //���
    int lin;              //�Ǻ������кţ���0��ʼ
    int col;              //�Ǻ������кţ���0��ʼ
    
    //�޲ι��캯��
    CSCCode() {oper = otVoid, lin = col = 0;}
    
    //���ڵ���
    void Debug() const;
};


//------------------------------------------------------
// �м�����(�沨��ʽ��)
//------------------------------------------------------
struct CSCCodeTable : public CNVector<CSCCode>
{
    //���һ��ָ��
    CSCCode &AddCode(const EOperatorType oper);
    CSCCode &AddCode(const EOperatorType oper,
        const CSCLexicaler &lex);
    CSCCode &AddCode(const CSCOperator &oper);
    
    //���ڵ���
    void Debug() const;
};


//------------------------------------------------------
// �����﷨�ṹ����
//------------------------------------------------------
struct CSCState
{
    EStatementType type; //�����﷨�ṹ����
    int breakpos;    //�˳���ָ��λ��
    int contipos;    //����ѭ����ָ��λ��
    int lin;         //�Ǻ������кţ���0��ʼ
    int col;         //�Ǻ������кţ���0��ʼ
    
    //�޲ι��캯��
    CSCState() {breakpos = contipos = lin = col = 0;}
};

//------------------------------------------------------
// �����﷨�ṹ����ջ
//------------------------------------------------------
struct CSCStateStack : public CNVector<CSCState>
{
    //���һ�������﷨�ṹ
    CSCState &AddState(const EStatementType type, const CSCLexicaler &lex);
};


#endif  /* _SC_CODE_TABLE_H */

