//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 在此添加用户文件头说明信息  
// 文件名称: clock.c
// 文件描述: 
// 文件版本: V1.0
// 修改记录:
//   1. (2026-01-10) 创建文件
////////////////////////////////////////
//<<AICUBE_USER_HEADER_REMARK_END>>


#include "config.h"


//<<AICUBE_USER_INCLUDE_BEGIN>>
// 在此添加用户头文件包含  
//<<AICUBE_USER_INCLUDE_END>>


//<<AICUBE_USER_GLOBAL_DEFINE_BEGIN>>
// 在此添加用户全局变量定义、用户宏定义以及函数声明  
//<<AICUBE_USER_GLOBAL_DEFINE_END>>



////////////////////////////////////////
// 时钟初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void CLK_Init(void)
{
    CLK_HSIOCK_Divider(1);              //设置高速外设时钟预分频系数

    CLK_SYSCLK_Divider(10);             //切换主时钟前先将系统时钟降频

    HIRC_24M();                         //选择内部预置的频率

    CLK_MCLK_HIRC();                    //选择内部高精度HIRC作为主时钟
    CLK_MCLK2_BYPASS();                 //旁路MCLK2,直接使用MCLK选择

    CLK_SYSCLK_Divider(1);              //设置系统时钟分频系数

    CLK_HSIOCK_MCLK();                  //选择MCLK主时钟作为高速外设时钟源

    //<<AICUBE_USER_CLOCK_INITIAL_BEGIN>>
    // 在此添加用户初始化代码  
    //<<AICUBE_USER_CLOCK_INITIAL_END>>
}



//<<AICUBE_USER_FUNCTION_IMPLEMENT_BEGIN>>
// 在此添加用户函数实现代码  
//<<AICUBE_USER_FUNCTION_IMPLEMENT_END>>


