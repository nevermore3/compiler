//------------------------------------------------------
// SC��̬�ű����Եı��������﷨��������
//   ���ߣ�����27651
//   ʱ�䣺2008-12-09
//------------------------------------------------------

#ifndef _SC_PARSER_H
#define _SC_PARSER_H

//���ʽ������
#include "SCExpression.h"

//�����﷨��������
struct CSCParser;


//------------------------------------------------------
// �﷨��������
//------------------------------------------------------
struct CSCParser
{
    CSCLexicaler lex;        //�ʷ�������
    CSCExpression expr;      //���ʽ
    CSCSymbolTable *symbols; //���ű����������ʼ��
    CSCSymbol *symbol;       //��ǰ�����ķ���
    CSCSymbol *tempsymbol;   //�������ҵķ���
    CSCFunction *func;       //��ǰ������ָ��
    CSCStateStack statestk;  //�����﷨�ṹ����ջ

    //ִ���﷨������CSCParserΪΨһ��ں���
    int Parse(const char *filename);

    //Parse��˽�к����������������Block
    int Block();         //Block�ķ�������
    int Const();         //����ȫ�ֳ���
    int VarOrFunction(); //����ȫ�ֱ�������

    //VarOrFunction��˽�к�����������������
    int FunctionDefine(); //������������
    int FunctionBody();   //����������

    //����(������/������)�ĸ��ִ���
    EExprElementType SymbolLookup();   //�������з���
    int SymbolGetName(CSCSymbol *symb);//��lex��÷�����
    
    //����������������ʱ����
    //������ʱ����: value����ʱ�����±�, lin��ռ�ñ�־
    int VarAssign(CSCSymbol *symb, const int value);//������ֵ
    void VarAllocTemp(const ESymbolType type, CSCCodeSymbol &symb);
    void VarFreeTemp(const CSCCodeSymbol &symb);//�ͷ���ʱ����
    int VarLocalDefine(); //�ֲ���������

    //�������������
    int StatementGroup(); //���������
    int Statement();      //�������
    int StatementLookup(const char *type);//���������䶨��
    CSCState &StatementAdd(const EStatementType type);//���������
    
    //���������䣬Statement��˽�к���
    int StatementIf();
    int StatementFor();
    int StatementWhile();
    int StatementDoWhile();
    int StatementSwitch();
    
    //����������Ĺؼ���
    int KeyReturn();  //�˳�����
    int KeyBreak();   //�˳�������
    int KeyContinue();//����ѭ��������
    int KeyCase();    //Switch�ķ�֧
    int KeyDefault(); //Switch��Ĭ�Ϸ�֧

    //Ϊ����������м����
    void CodeAddGoto(const int pos);  //��������ת
    void CodeAddGoNot(const int pos); //��������ת
    void CodeSetGoNot(const int pos); //������������תλ��
    void CodeAddBreak();              //�˳�������
};


#endif  /* _SC_PARSER_H */

