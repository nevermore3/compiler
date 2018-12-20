//�ʷ�������
#include "SCLexicaler.h"

//���ش����ļ�
int CSCLexicaler::LoadScript(const char *filename)
{
    //���ش����ļ�
    if (!list.LoadFromFile(filename))
    {
        printf("Read script file (%s) error!\r\n", filename);
        return M_SC_ERR;
    }
    
    //��ʾ�ļ��ڴ棬���ڵ��ԣ�Ĭ��Ϊע�͵�
    //list.Show();

    //��ʼ���кź��к�
    lin = 0;
    col = 0;
    errors = 0;

    //��ʼ���������б�AddPart�Ĺ������Կո�ָ��ַ�����Ȼ��ֱ����operators
    operators.AddPart("{ } ( ) , ; : + - * / % ! && || = < > == != <= >=", " ");

    //��ʼ���ؼ����б�AddPart����������Ϊfalse��ʾ�����֮ǰ��ӵ��ַ���
    keywords.AddPart("boolean string int const true false void", " ");
    keywords.AddPart("return for continue break if", " ", False);
    keywords.AddPart("else switch case default while", " ", False);

    //���ؽ��
    return M_SC_OK;
}

//���ζ�ȡ�Ǻţ��ɹ�����M_SC_POK��ʧ�ܷ���M_SC_ERR���ļ���������M_SC_OK
int CSCLexicaler::GetNextToken()
{
    //���ݵ�ǰ�ַ������﷨�Ƶ�ѭ��
    while (list.GetCount() > lin)
    {
        switch (GetCharType(list[lin][col]))
        {
        case ctSpace:
            //������ֿո��������һ���ַ�
            col++;
            break;
        case ctZero:
            //�������һ�е���β����������һ��
            JumpLine();
            break;
        case ctNumber:
            //����������֣���ȡ����
            return GetInteger();
        case ctString:
            //������֡�"������ȡ�ַ���
            return GetString();
        case ctLetter:
            //���������ĸ����ȡ����
            return GetWord();
        case ctSign:
            //������ַ��ţ������ж��ǲ���ע�͡�//��
            if ((list[lin][col] == '/') && (col < list[lin].GetLength() - 1) && 
                (list[lin][col + 1] == '/'))
            {
                JumpLine();
                break;
            }
            //Ȼ����ȡ������
            return GetOperator();
        default:
            //�������δ֪���ַ�������
            InitToken(ttError);
            PrintError(101, "Unknown char \"%c\"", list[lin][col]);
            col++;
            return M_SC_ERR;
        }
    }

    //�ļ�ȫ�����ˣ�����OK
    return M_SC_OK;
}

//������һ�У���GetNextToken��˽�к���
void CSCLexicaler::JumpLine()
{
    lin++;
    col = 0;
}

//��ʼ��token����GetNextToken��˽�к���
void CSCLexicaler::InitToken(ETokenType type)
{
    token.type = type;
    token.lin = lin;
    token.col = col;
    token.str = "";
}

//����ַ����ͣ���GetNextToken��˽�к���
ECharType CSCLexicaler::GetCharType(char c)
{
    if ((c == ' ') || (c == '\t'))
    {
        return ctSpace;
    }
    else
    if (c == '\0')
    {
        return ctZero;
    }
    else
    if (c == '\"')
    {
        return ctString;
    }
    else
    if ((c >= '0') && (c <= '9'))
    {
        return ctNumber;
    }
    else
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) ||
        (list[lin][col] == '_'))
    {
        return ctLetter;
    }
    else
    if ((c == '{') || (c == '}') || (c == '(') || (c == ')') || (c == '+') || 
        (c == '-') || (c == '*') || (c == '/') || (c == '%') || (c == '=') || 
        (c == '!') || (c == '>') || (c == '<') || (c == '|') || (c == '&') ||
        (c == ',') || (c == ';') || (c == ':'))
    {
        return ctSign;
    }
    else
    {
        return ctError;
    }
}

//�﷨�Ƶ���ʽ�����������GetNextToken��˽�к���
int CSCLexicaler::GetInteger()
{
    //��ʼ��Ϊ����
    InitToken(ttInteger);

    //�����﷨����ΪN����������
    do
    {
        token.str += list[lin][col++];
    } while (GetCharType(list[lin][col]) == ctNumber);

    //���ؽ��
    return M_SC_POK;
}

//�﷨�Ƶ���ʽ����ַ�������GetNextToken��˽�к���
int CSCLexicaler::GetString()
{
    //��ʼ��Ϊ����
    InitToken(ttString);

    /*������ʾͨ���Զ����ķ�ʽ��ȡ�ַ���

      ֻ������״̬��0(�ַ�̬)��1(ת��̬)
      ״̬�л������£�

      state       \0       "         \         rnt      other
      0(�ַ�̬)   err1     return    to1       +        +
      1(ת��̬)   err1     + to0     + to0     +x to0   err2

      �����ĺ��壺
      return����ȷ���ؽ��
      to n��״̬���л���ָ������ֵ
      +��׷�ӵ�ǰ�ַ�������ַ���
      +x��ת��׷���ַ���\r=0x0d, \n=0x0a, \t=0x09
      err1���ַ���������
      err2��δ֪��ת���ַ�
    */
    int state = 0;
    while (list[lin].GetLength() > col++)
    {
        switch (state)
        {
        case 0: //�ַ�̬
            switch (list[lin][col])
            {
            case '\"':
                col++;
                return M_SC_POK;
            case '\\':
                state = 1;
                break;
            case '\0':
                token.col = col;
                PrintError(102, "Incompleteline string \"%s\"", token.str.S());
                return M_SC_ERR;
            default:
                token.str += list[lin][col];
            }
            break;
        case 1: //ת��̬
            switch (list[lin][col])
            {
            case '\"':
            case '\\':
                token.str += list[lin][col];
                state = 0;
                break;
            case 'r':
                token.str += '\r';
                state = 0;
                break;
            case 'n':
                token.str += '\n';
                state = 0;
                break;
            case 't':
                token.str += '\t';
                state = 0;
                break;
            case '\0':
                token.col = col;
                PrintError(102, "Incompleteline string \"%s\"", token.str.S());
                return M_SC_ERR;
            default:
                token.col = col;
                PrintError(103, "Unknown escape char \"%c\"", list[lin][col]);
                return M_SC_ERR;
            }
            break;
        }
    }

    //���ؽ��
    return M_SC_POK;
}

//�﷨�Ƶ���ʽ��õ��ʣ���GetNextToken��˽�к���
int CSCLexicaler::GetWord()
{
    //��ʼ��Ϊ����
    InitToken(ttWord);

    //�����﷨����ΪN�����ֻ���ĸ����
    do
    {
        token.str += list[lin][col++];
    } while ((GetCharType(list[lin][col]) == ctLetter) ||
             (GetCharType(list[lin][col]) == ctNumber));
    
    //�ж��ǲ��ǹؼ���
    if (keywords.IndexOf(token.str) >= 0)
    {
        token.type = ttKeyword;
    }

    //���ؽ��
    return M_SC_POK;
}

//�﷨�Ƶ���ʽ��ò���������GetNextToken��˽�к���
int CSCLexicaler::GetOperator()
{
    //��ʼ��Ϊ������
    InitToken(ttOperator);

    //��õ�һ���ַ�
    token.str += list[lin][col++];

    //�ж��ǲ���˫�ַ�������
    if ((col < list[lin].GetLength()) && (GetCharType(list[lin][col]) == ctSign))
    {
        //����������������ַ���������˫�ַ������
        token.str += list[lin][col];
        if (operators.IndexOf(token.str) >= 0)
        {
            col++;
            return M_SC_POK;
        }
        //����˫�ַ���������ɾ���ڶ����ַ�
        token.str.Left(1);
    }

    //�ж��ǲ��ǵ��ַ�������
    if (operators.IndexOf(token.str) >= 0)
    {
        return M_SC_POK;
    }

    //�Ȳ��ǵ��ַ���Ҳ����˫�ַ�����������
    token.type = ttError;
    PrintError(101, "Unknown char \"%s\"", token.str.S());
    return M_SC_ERR;
}

//��ӡ��Ϣ��Ҳ�����﷨��������������ô˺���
void CSCLexicaler::Printf(const char* format, ...)
{
    //��ʽ����Ϣ
    va_list argptr;
    char str[1000];
    va_start(argptr, format);
    vsprintf(str, format, argptr);
    va_end(argptr);

    //��ӡ��Ϣ
    printf("Line:%d, Col:%d - %s\r\n", token.lin + 1, token.col + 1, str);
}

//��ӡ������Ϣ��Ҳ�����﷨��������������ô˺���
void CSCLexicaler::PrintError(const int ID, const char* format, ...)
{
    //��ӡ������
    printf("\r\n%s\r\n", list[token.lin].S());
    for (int i = 0; i < token.col; i++)
    {
        printf(" ");
    }
    printf("-\r\n");

    //��ʽ����Ϣ
    va_list argptr;
    char str[1000];
    va_start(argptr, format);
    vsprintf(str, format, argptr);
    va_end(argptr);

    //��ӡ��Ϣ
    printf("%d. Line:%d, Col:%d - Error(%d): %s\r\n\r\n", 
        ++errors, token.lin + 1, token.col + 1, ID, str);
}

//�Զ����кŴ�ӡ�����ӡ������Ϣ
void CSCLexicaler::PrintErrorLin(const int lin, const int col, 
    const int ID, const char* format, ...)
{
    //��ӡ������
    printf("\r\n%s\r\n", list[lin].S());
    for (int i = 0; i < col; i++)
    {
        printf(" ");
    }
    printf("-\r\n");

    //��ʽ����Ϣ
    va_list argptr;
    char str[1000];
    va_start(argptr, format);
    vsprintf(str, format, argptr);
    va_end(argptr);

    //��ӡ��Ϣ
    printf("%d. Line:%d, Col:%d - Error(%d): %s\r\n\r\n", 
        ++errors, lin + 1, col + 1, ID, str);
}

//���ڵ��ԣ���ӡ��ǰ�Ǻ���Ϣ
void CSCLexicaler::Debug()
{
    if (token.type == ttString)
    {
        Printf("%s:\"%s\"", ETokenStr[token.type], token.str.S());
    }
    else
    {
        Printf("%s:%s", ETokenStr[token.type], token.str.S());
    }
}
