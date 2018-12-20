
//-----------------------------------------
// 系统函数
//-----------------------------------------

//打印调试信息
void printf(string str);

//从终端获得一个字符串，敲Enter结束
string scanf();


//-----------------------------------------
// 数据转化
//-----------------------------------------

//大写<->小写
string lowercase(string str);
string uppercase(string str);

//字符串<->数字
int str_int(string str);
string int_str(int val);

//字符串<->布尔值
boolean str_bool(string str);
string bool_str(boolean bool);

//整数<->布尔值
boolean int_bool(int val);
int bool_int(boolean bool);

