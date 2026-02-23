//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 在此添加用户文件头说明信息  
// 文件名称: can.c
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


BOOL fCAN1SendBusy;                     //发送FIFO中的CAN数据包发送忙标志
BOOL fCAN1ReadyRead;                    //接收FIFO中有待读取的CAN数据包标志

////////////////////////////////////////
// CAN1初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void CAN1_Init(void)
{
#define CAN1_BAUDRATE           (1000)
#define CAN1_SJW                (0)
#define CAN1_BRP                (0)
#define CAN1_TSG1               (8)
#define CAN1_TSG2               (1)
#define CAN1_ACR                (0x00000000)
#define CAN1_AMR                (0x00000000)

    CAN1_Enable();                      //使能CAN1功能

    CAN1_SwitchP0001();                 //选择CAN1数据口: CANRX (P0.0), CANTX (P0.1)

    CAN1_SEL();                         //选择CAN1模块
    CAN_EnterResetMode();               //进入复位模式，开始对CAN模块进行初始化

    CAN_NormalMode();                   //CAN模块为普通工作模式
    CAN_DoubleFilter();                 //CAN模块使用双过滤设置

    CAN_SetSJW(CAN1_SJW);               //设置重新同步跳跃宽度
    CAN_SetBRP(CAN1_BRP);               //设置CAN波特率分频系数
    CAN_SetTSG1(CAN1_TSG1);             //设置同步采样段1
    CAN_SetTSG2(CAN1_TSG2);             //设置同步采样段2
    CAN_SampleOnce();                   //总线电平采样1次

    CAN_SetACR0(BYTE3(CAN1_ACR));       //设置验收代码寄存器
    CAN_SetACR1(BYTE2(CAN1_ACR));       //设置验收代码寄存器
    CAN_SetACR2(BYTE1(CAN1_ACR));       //设置验收代码寄存器
    CAN_SetACR3(BYTE0(CAN1_ACR));       //设置验收代码寄存器
    CAN_SetAMR0(BYTE3(CAN1_AMR));       //设置验收屏蔽寄存器
    CAN_SetAMR1(BYTE2(CAN1_AMR));       //设置验收屏蔽寄存器
    CAN_SetAMR2(BYTE1(CAN1_AMR));       //设置验收屏蔽寄存器
    CAN_SetAMR3(BYTE0(CAN1_AMR));       //设置验收屏蔽寄存器

    CAN_ClearFlags(0xff);               //清除全部中断标志位
    CAN_EnableArbitrateLostInt();       //使能仲裁丢失中断
    CAN_EnableErrorWarningInt();        //使能错误警告中断
    CAN_EnableErrorPassiveInt();        //使能被动错误中断
    CAN_EnableReceiveInt();             //使能接收中断
    CAN_EnableTransmitInt();            //使能发送中断
    CAN_EnableBusErrorInt();            //使能总线错误中断
    CAN_EnableDataOverrunInt();         //使能接收溢出中断
    CAN1_SetIntPriority(2);             //设置中断为较高优先级
    CAN1_EnableInt();                   //使能CAN1中断
    fCAN1SendBusy = 0;                  //清除CAN数据包发送忙标志
    fCAN1ReadyRead = 0;                 //清除有待读取的CAN数据包标志

    CAN_ExitResetMode();                //退出复位模式，

    //<<AICUBE_USER_CAN1_INITIAL_BEGIN>>
    // 在此添加用户初始化代码  
    //<<AICUBE_USER_CAN1_INITIAL_END>>
}

////////////////////////////////////////
// 读取CAN缓冲区数据
// 入口参数: *pCAN: (存放CAN总线读取数据结构参数)
// 函数返回: 无
////////////////////////////////////////
void CAN_ReadFIFO(CAN_DataDef *pCAN)
{
    uint8_t index, i;
    uint32_t id;

    *(uint8_t *)pCAN = CAN_ReadBUF0();  //读取帧格式和数据长度信息
    if (pCAN->FF)                       //判断是标准帧还是扩展帧
    {
        id = CAN_ReadBUF1();            //扩展帧ID占4个字节
        id = (id << 8) | CAN_ReadBUF2();
        id = (id << 8) | CAN_ReadBUF3();
        id = (id << 8) | CAN_ReadBUF0();
        pCAN->ID = id >> 3;             //格式化后得到帧的ID
        index = 1;
    }
    else
    {
        id = CAN_ReadBUF1();            //标准帧ID占2个字节
        id = (id << 8) | CAN_ReadBUF2();
        pCAN->ID = id >> 5;             //格式化后得到帧的ID
        index = 3;
    }

    for (i = 0; i < pCAN->DLC; i++)
    {
        if (i >= 8) break;              //数据最多不超过8字节
        pCAN->DataBuffer[i] = CAN_ReadBUF(index++); //读取有效数据
    }

    while (index & 3)                   //保证读取FIFO的数据长度为4的整倍数
        i = CAN_ReadBUF(index++);       //读取冗余数据
}

////////////////////////////////////////
// 接收CAN消息
// 入口参数: *pCAN: (接收CAN消息缓冲区)
// 函数返回: 帧个数
////////////////////////////////////////
uint8_t CAN_ReadMsg(uint8_t CANx, CAN_DataDef *pCAN)
{
    uint8_t n = 0;

    if (CANx == CAN1)
        CAN1_SEL();                     //选择CAN1模块
    else if (CANx == CAN2)
        CAN2_SEL();                     //选择CAN2模块
    else
        return (uint8_t)(-1);           //参数错误,直接退出

    do
    {
        CAN_ReadFIFO(pCAN++);           //读取接收缓冲区数据
        n++;                            //帧个数+1
    } while (CAN_CheckRBufferStatus()); //判断接收缓冲区里是否还有数据，有的话继续读取

    return n;                           //返回帧个数
}

////////////////////////////////////////
// CAN发送消息
// 入口参数: *pCAN: (发送CAN消息缓冲区)
// 函数返回: 无
////////////////////////////////////////
void CAN_SendMsg(uint8_t CANx, CAN_DataDef *pCAN)
{
    uint8_t index, i;
    uint32_t id;


    i = 200;
    if (CANx == CAN1)
        CAN1_SEL();                     //选择CAN1模块
    else if (CANx == CAN2)
        CAN2_SEL();                     //选择CAN2模块
    else
        return;                         //参数错误,直接退出

    i = 200;
    if (CANx == CAN1)                   //判断当前的CAN模块
        while ((--i) && (fCAN1SendBusy)); //等待CAN1上次发送完成

    if (pCAN->FF)                       //判断是否扩展帧
    {
        id = pCAN->ID << 3;
        CAN_WriteBUF0(*(uint8_t*)pCAN); //将帧格式和数据长度信息写入CAN FIFO
        CAN_WriteBUF1(BYTE3(id));
        CAN_WriteBUF2(BYTE2(id));
        CAN_WriteBUF3(BYTE1(id));
        CAN_WriteBUF0(BYTE0(id));

        index = 1;
    }
    else                                //发送标准帧
    {
        id = pCAN->ID << 5;
        CAN_WriteBUF0(*(uint8_t*)pCAN); //将帧格式和数据长度信息写入CAN FIFO
        CAN_WriteBUF1(BYTE1(id));
        CAN_WriteBUF2(BYTE0(id));

        index = 3;
    }
    for (i = 0; i < pCAN->DLC; i++)
    {
        if (i >= 8) break;              //数据最多不超过8字节
        CAN_WriteBUF(index++, pCAN->DataBuffer[i]); //写数据到CAN FIFO
    }
    while (index & 3)                   //判断已读数据长度是否4的整数倍
        CAN_WriteBUF(index++, 0);       //写冗余数据到CAN FIFO

    CAN_TriggerTransmit();              //发起一次帧传输,将CAN FIFO中的数据发送到CAN总线上

    if (CANx == CAN1)                   //判断当前的CAN模块
        fCAN1SendBusy = 1;              //设置CAN1发送忙标志

}


////////////////////////////////////////
// CAN1中断服务程序
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void CAN1_ISR(void) interrupt CAN1_VECTOR
{
    //<<AICUBE_USER_CAN1_ISR_CODE1_BEGIN>>
    // 在此添加中断函数用户代码  
    uint8_t isr;
    uint8_t aux;
    uint8_t ar;

    ar = CANAR;                         //LINAR现场保护
    aux = AUXR2;                        //AUXR2现场保护

    CAN1_SEL();                         //选择CAN1模块
    isr = CAN_ReadIntStatus();          //读取中断标志位
    CAN_ClearFlags(isr);                //写1清除中断标志位

    if (CAN_CheckReceive(isr))          //判断接收中断标志位
    {
        fCAN1ReadyRead = 1;             //设置CAN消息待读取标志
    }
    if (CAN_CheckBusError(isr))         //判断总线错误中断标志位
    {
    }

    AUXR2 = aux;                        //AUXR2现场恢复
    CANAR = ar;                         //CANAR现场恢复
    //<<AICUBE_USER_CAN1_ISR_CODE1_END>>
}


//<<AICUBE_USER_FUNCTION_IMPLEMENT_BEGIN>>
// 在此添加用户函数实现代码  
//<<AICUBE_USER_FUNCTION_IMPLEMENT_END>>


