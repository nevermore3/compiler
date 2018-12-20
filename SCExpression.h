//------------------------------------------------------
// SC��̬�ű����Եı����������ʽ������
//   ���ߣ�����27651
//   ʱ�䣺2009-04-03
//------------------------------------------------------

#ifndef _SC_EXPRESSION_H
#define _SC_EXPRESSION_H

//�ʷ�������
#include "SCLexicaler.h"

//���ű�
#include "SCSymbolTable.h"

//�����﷨��������
struct CSCParser;

    
//------------------------------------------------------
// ���ʽ������
//------------------------------------------------------
struct CSCExpression
{
    CSCParser *parser;            //�﷨����������CSCParser��ʼ��
    EExprElementType curtype;     //��ǰ���ʽԪ�ص�����
    CSCOperatorStack operatorstk; //���ʽ�������ջ
    CSCSymbolStack symbolstk;     //���ʽ�ķ���ջ
        
    //���ʽ���㣬�Ǳ��ʽ�������Ψһ���
    //needtpye: stConst...Ҫ�󷵻س�����
    // stVar...Ҫ�󷵻س�������stVoid�����أ�����������
    int Calc(const ESymbolType needtype);

    //��õ�ǰ�Ǻŵı��ʽԪ�����ͣ�Calc��˽�к���
    int GetNextElem();

    //��ǰ�ǺŽ������ջ��Calc��˽�к���
    void PushOperator();
    
    //��ǰ�ǺŽ�����ջ��Calc��˽�к���
    void PushSymbol();
    void PushSymbol(const CSCCodeSymbol &symbol);
    
    //��ջһ���沨��ʽ��Calc��˽�к���
    int PopAOper();

    //һֱ��ջ����operƥ����������Calc��˽�к���
    int PopWith(const EExprElementType type);
    
    //���б��ʽ����ջ��Calc��˽�к���
    int PopAll();

    //��ջһ����Ŀ���㣬PopAOper��˽�к���
    int PopOne(CSCCode &code, const CSCOperator &oper);
    
    //��ջһ��˫Ŀ���㣬PopAOper��˽�к���
    int PopTwo(CSCCode &code, const CSCOperator &oper);
    
    //��ջһ��������PopAOper��˽�к���
    int PopFunc(CSCCode &code);
};


#endif  /* _SC_EXPRESSION_H */

