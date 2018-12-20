//------------------------------------------------------
// SC动态脚本语言的编译器
//   作者：张毅27651
//   时间：2008-12-09
//------------------------------------------------------

//虚拟机
#include "SCVirtualMachine.h"

//主函数
int main(int argc, char* argv[])
{
    //虚拟机类
    CSCVirtualMachine vm;

    //加载与编译代码文件（包括词法分析、语法分析）
    if (vm.LoadScript("sample.sc") != M_SC_OK)
    {
        return M_SC_ERR;
    }

    //执行中间代码
    if (vm.RunScript() != M_SC_OK)
    {
        return M_SC_ERR;
    }
    
    //返回
    return M_SC_OK;
}

