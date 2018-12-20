//词法分析器
#include "SCLexicaler.h"

//加载代码文件
int CSCLexicaler::LoadScript(const char *filename)
{
    //加载代码文件
    if (!list.LoadFromFile(filename))
    {
        printf("Read script file (%s) error!\r\n", filename);
        return M_SC_ERR;
    }
    
    //显示文件内存，用于调试，默认为注释掉
    //list.Show();

    //初始化行号和列号
    lin = 0;
    col = 0;
    errors = 0;

    //初始化操作符列表，AddPart的功能是以空格分割字符串，然后分别插入operators
    operators.AddPart("{ } ( ) , ; : + - * / % ! && || = < > == != <= >=", " ");

    //初始化关键字列表，AddPart第三个参数为false表示不清空之前添加的字符串
    keywords.AddPart("boolean string int const true false void", " ");
    keywords.AddPart("return for continue break if", " ", False);
    keywords.AddPart("else switch case default while", " ", False);

    //返回结果
    return M_SC_OK;
}

//依次读取记号，成功返回M_SC_POK，失败返回M_SC_ERR，文件结束返回M_SC_OK
int CSCLexicaler::GetNextToken()
{
    //根据当前字符启动语法制导循环
    while (list.GetCount() > lin)
    {
        switch (GetCharType(list[lin][col]))
        {
        case ctSpace:
            //如果发现空格，则分析下一个字符
            col++;
            break;
        case ctZero:
            //如果到了一行的行尾，则跳到下一行
            JumpLine();
            break;
        case ctNumber:
            //如果发现数字，提取整数
            return GetInteger();
        case ctString:
            //如果发现“"”，提取字符串
            return GetString();
        case ctLetter:
            //如果发现字母，提取单词
            return GetWord();
        case ctSign:
            //如果发现符号，首先判断是不是注释“//”
            if ((list[lin][col] == '/') && (col < list[lin].GetLength() - 1) && 
                (list[lin][col + 1] == '/'))
            {
                JumpLine();
                break;
            }
            //然后提取操作符
            return GetOperator();
        default:
            //如果发现未知的字符，报错
            InitToken(ttError);
            PrintError(101, "Unknown char \"%c\"", list[lin][col]);
            col++;
            return M_SC_ERR;
        }
    }

    //文件全读完了，返回OK
    return M_SC_OK;
}

//跳到下一行，是GetNextToken的私有函数
void CSCLexicaler::JumpLine()
{
    lin++;
    col = 0;
}

//初始化token，是GetNextToken的私有函数
void CSCLexicaler::InitToken(ETokenType type)
{
    token.type = type;
    token.lin = lin;
    token.col = col;
    token.str = "";
}

//获得字符类型，是GetNextToken的私有函数
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

//语法制导方式获得整数，是GetNextToken的私有函数
int CSCLexicaler::GetInteger()
{
    //初始化为整数
    InitToken(ttInteger);

    //按照语法整数为N个数字连接
    do
    {
        token.str += list[lin][col++];
    } while (GetCharType(list[lin][col]) == ctNumber);

    //返回结果
    return M_SC_POK;
}

//语法制导方式获得字符串，是GetNextToken的私有函数
int CSCLexicaler::GetString()
{
    //初始化为单词
    InitToken(ttString);

    /*这里演示通过自动机的方式提取字符串

      只有两种状态：0(字符态)、1(转意态)
      状态切换表如下：

      state       \0       "         \         rnt      other
      0(字符态)   err1     return    to1       +        +
      1(转意态)   err1     + to0     + to0     +x to0   err2

      动作的含义：
      return：正确返回结果
      to n：状态机切换到指定的数值
      +：追加当前字符到结果字符串
      +x：转意追加字符，\r=0x0d, \n=0x0a, \t=0x09
      err1：字符串不完整
      err2：未知的转意字符
    */
    int state = 0;
    while (list[lin].GetLength() > col++)
    {
        switch (state)
        {
        case 0: //字符态
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
        case 1: //转意态
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

    //返回结果
    return M_SC_POK;
}

//语法制导方式获得单词，是GetNextToken的私有函数
int CSCLexicaler::GetWord()
{
    //初始化为单词
    InitToken(ttWord);

    //按照语法单词为N个数字或字母连接
    do
    {
        token.str += list[lin][col++];
    } while ((GetCharType(list[lin][col]) == ctLetter) ||
             (GetCharType(list[lin][col]) == ctNumber));
    
    //判断是不是关键字
    if (keywords.IndexOf(token.str) >= 0)
    {
        token.type = ttKeyword;
    }

    //返回结果
    return M_SC_POK;
}

//语法制导方式获得操作符，是GetNextToken的私有函数
int CSCLexicaler::GetOperator()
{
    //初始化为操作符
    InitToken(ttOperator);

    //获得第一个字符
    token.str += list[lin][col++];

    //判断是不是双字符操作符
    if ((col < list[lin].GetLength()) && (GetCharType(list[lin][col]) == ctSign))
    {
        //如果是连续的两个字符，可能是双字符运算符
        token.str += list[lin][col];
        if (operators.IndexOf(token.str) >= 0)
        {
            col++;
            return M_SC_POK;
        }
        //不是双字符操作符，删除第二个字符
        token.str.Left(1);
    }

    //判断是不是单字符操作符
    if (operators.IndexOf(token.str) >= 0)
    {
        return M_SC_POK;
    }

    //既不是单字符，也不是双字符，就是乱码
    token.type = ttError;
    PrintError(101, "Unknown char \"%s\"", token.str.S());
    return M_SC_ERR;
}

//打印信息，也允许语法分析和虚拟机调用此函数
void CSCLexicaler::Printf(const char* format, ...)
{
    //格式化信息
    va_list argptr;
    char str[1000];
    va_start(argptr, format);
    vsprintf(str, format, argptr);
    va_end(argptr);

    //打印信息
    printf("Line:%d, Col:%d - %s\r\n", token.lin + 1, token.col + 1, str);
}

//打印错误信息，也允许语法分析和虚拟机调用此函数
void CSCLexicaler::PrintError(const int ID, const char* format, ...)
{
    //打印错误行
    printf("\r\n%s\r\n", list[token.lin].S());
    for (int i = 0; i < token.col; i++)
    {
        printf(" ");
    }
    printf("-\r\n");

    //格式化信息
    va_list argptr;
    char str[1000];
    va_start(argptr, format);
    vsprintf(str, format, argptr);
    va_end(argptr);

    //打印信息
    printf("%d. Line:%d, Col:%d - Error(%d): %s\r\n\r\n", 
        ++errors, token.lin + 1, token.col + 1, ID, str);
}

//自定义行号打印错误打印错误信息
void CSCLexicaler::PrintErrorLin(const int lin, const int col, 
    const int ID, const char* format, ...)
{
    //打印错误行
    printf("\r\n%s\r\n", list[lin].S());
    for (int i = 0; i < col; i++)
    {
        printf(" ");
    }
    printf("-\r\n");

    //格式化信息
    va_list argptr;
    char str[1000];
    va_start(argptr, format);
    vsprintf(str, format, argptr);
    va_end(argptr);

    //打印信息
    printf("%d. Line:%d, Col:%d - Error(%d): %s\r\n\r\n", 
        ++errors, lin + 1, col + 1, ID, str);
}

//用于调试，打印当前记号信息
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
