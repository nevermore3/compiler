//------------------------------------------------------
// SC��̬�ű����Եı����������ű�
//   ���ߣ�����27651
//   ʱ�䣺2008-12-16
//------------------------------------------------------

#ifndef _SC_SYMBOL_TABLE_H
#define _SC_SYMBOL_TABLE_H

//�м�������
#include "SCCodeTable.h"

//���ú�����
struct CSCFunction;


//------------------------------------------------------
// ���ŵĶ���
//------------------------------------------------------
struct CSCSymbol
{
    CNString name;   //���ŵ�����
    ESymbolType type;//��������
    int value;       //���ŵ�ȡֵ
    int lin;         //�Ǻ������кţ���0��ʼ
    int col;         //�Ǻ������кţ���0��ʼ

    //��ʼ��
    void Init(const char *_name, const ESymbolType _type);
    
    //����ڴ�
    void Clear();
    
    //�ַ���ָ��
    CNString * str() {return (CNString *)value;}

    //����ָ��
    CSCFunction * func() {return (CSCFunction *)value;}

    //�޲ι��캯��
    CSCSymbol() {type = stVoid; value = lin = col = 0;}
    
    //��������
    virtual ~CSCSymbol() {Clear();}
    
    //���رȽ�������ţ���ģ����CNList��������
    Bool operator <  (const CSCSymbol &right) const {return name <  right.name;}
    Bool operator == (const CSCSymbol &right) const {return name == right.name;}
    Bool operator >  (const CSCSymbol &right) const {return name >  right.name;}

    //���ڵ���
    void Debug(const Bool showdetail = True);
};


//------------------------------------------------------
// ���ű�
//------------------------------------------------------
struct CSCSymbolTable : public CNList<CSCSymbol>
{
    //���һ������
    CSCSymbol &AddSymb(CSCSymbol &_symb);
    CSCSymbol &AddSymb(const char *name, const ESymbolType type);
    CSCSymbol &AddSymb(const char *name, const ESymbolType type,
        const CSCLexicaler &lex); 

    //�������Ʋ��ҷ��ţ�ʧ�ܷ���0
    CSCSymbol * Lookup(const char *name);

    //���ڵ��ԣ�showsys��ʱ�Ƿ���ʾϵͳ���������
    void Debug(const Bool showsyscall = False);
};


//------------------------------------------------------
// ������
//------------------------------------------------------
struct CSCFunction
{
    CSCSymbolTable symbols; //�����ľֲ�����
    CSCCodeTable codes;     //�������м�����
    CNString name;          //����������
    int paras;              //�����������޲�����ȡֵ0
    Bool syscall;           //�ǲ���ϵͳ�ڲ�����

    //�޲ι��캯��
    CSCFunction() {paras = syscall = 0;}
    
    //���ڵ���
    void Debug();
};

#endif  /* _SC_SYMBOL_TABLE_H */

