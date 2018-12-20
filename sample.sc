
//Ö÷º¯Êý
void main()
{
    int i;
    int cnt = 0;
    
    printf("please input a number (1-10000): ");
    string str = scanf();
    int all = str_int(str);
    
    for (i = 1; i <= all; i++)
    {
        cnt = cnt + i;
    }
    printf("\r\n1+2+..+" + str + " = " + int_str(cnt) + "\r\n\r\n");
}

