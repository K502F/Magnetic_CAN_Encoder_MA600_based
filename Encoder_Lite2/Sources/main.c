//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 在此添加用户文件头说明信息  
// 文件名称: main.c
// 文件描述: 
// 文件版本: V1.0
// 修改记录:
//   1. (2026-01-10) 创建文件
////////////////////////////////////////
//<<AICUBE_USER_HEADER_REMARK_END>>


#include "config.h"                     //默认已包含stdio.h、intrins.h等头文件


//<<AICUBE_USER_INCLUDE_BEGIN>>
// 在此添加用户头文件包含  
//<<AICUBE_USER_INCLUDE_END>>


//<<AICUBE_USER_GLOBAL_DEFINE_BEGIN>>
// 在此添加用户全局变量定义、用户宏定义以及函数声明  
#define SS P22
#define CAN_id 0xD4
extern uint8_t SPI_ReadByte00(void);
void SSPI_Init(void);
//<<AICUBE_USER_GLOBAL_DEFINE_END>>



////////////////////////////////////////
// 项目主函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void main(void)
{
    //<<AICUBE_USER_MAIN_INITIAL_BEGIN>>
    // 在此添加用户主函数初始化代码  
    //<<AICUBE_USER_MAIN_INITIAL_END>>

    SYS_Init();

    //<<AICUBE_USER_MAIN_CODE_BEGIN>>
    // 在此添加主函数中运行一次的用户代码
       SS=1;
	P20=1;
	SSPI_Init();
    //<<AICUBE_USER_MAIN_CODE_END>>

    while (1)
    {
        //<<AICUBE_USER_MAIN_LOOP_BEGIN>>
        // 在此添加主函数中用户主循环代码  
        //<<AICUBE_USER_MAIN_LOOP_END>>
    }
}

////////////////////////////////////////
// 系统初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void SYS_Init(void)
{
    EnableAccessXFR();                  //使能访问扩展XFR
    AccessCodeFastest();                //设置最快速度访问程序代码
    AccessIXramFastest();               //设置最快速度访问内部XDATA
    IAP_SetTimeBase();                  //设置IAP等待参数,产生1us时基

    //<<AICUBE_USER_PREINITIAL_CODE_BEGIN>>
    // 在此添加用户预初始化代码  
    //<<AICUBE_USER_PREINITIAL_CODE_END>>

    P0M0 = 0x00; P0M1 = 0x00;           //初始化P0口为准双向口模式
    P1M0 = 0x00; P1M1 = 0x00;           //初始化P1口为准双向口模式
    P2M0 = 0x00; P2M1 = 0x00;           //初始化P2口为准双向口模式
    P3M0 = 0x00; P3M1 = 0x00;           //初始化P3口为准双向口模式
    P4M0 = 0x00; P4M1 = 0x00;           //初始化P4口为准双向口模式
    P5M0 = 0x00; P5M1 = 0x00;           //初始化P5口为准双向口模式
    P6M0 = 0x00; P6M1 = 0x00;           //初始化P6口为准双向口模式
    P7M0 = 0x00; P7M1 = 0x00;           //初始化P7口为准双向口模式

    PORT0_Init();                       //P0口初始化
    PORT2_Init();                       //P2口初始化
    CLK_Init();                         //时钟模块初始化
    TIMER0_Init();                      //定时器0初始化
    SSPI_Init();                         //SPI初始化
    CAN1_Init();                        //CAN1初始化
    MATHLIB_Init();                     //MATH库初始化

    //<<AICUBE_USER_INITIAL_CODE_BEGIN>>
    // 在此添加用户初始化代码  
    //<<AICUBE_USER_INITIAL_CODE_END>>

    EnableGlobalInt();                  //使能全局中断
}

////////////////////////////////////////
// 微秒延时函数
// 入口参数: us (设置延时的微秒值)
// 函数返回: 无
////////////////////////////////////////
void delay_us(uint16_t us)
{
    do
    {
        NOP(18);                        //(MAIN_Fosc + 500000) / 1000000 - 6
    } while (--us);
}


////////////////////////////////////////
// 毫秒延时函数
// 入口参数: ms (设置延时的毫秒值)
// 函数返回: 无
////////////////////////////////////////
void delay_ms(uint16_t ms)
{
    uint16_t i;

    do
    {
        i = MAIN_Fosc / 6000;
        while (--i);
    } while (--ms);
}


//<<AICUBE_USER_FUNCTION_IMPLEMENT_BEGIN>>
// 在此添加用户函数实现代码  
sbit CS_PIN = P2^2;    // ????
sbit MOSI_PIN = P2^3;  // ????
sbit MISO_PIN = P2^4;  // ????
sbit SCK_PIN = P2^5;   // ????


CAN_DataDef CAN_Data;
float Single_Rad,Multi_Rad,Last_Multi_Rad;
uint16_t Single_Rad_16,Omega_16;
int16_t Multi_Rad_16;
struct angle_raw_t{
	uint16_t angle;
	int32_t multi_angle;
};
struct angle_raw_t raw_angles;
void SSPI_Init(void)
{
    // ??SPI??
   
    
    // ???????
    CS_PIN = 1;     // ?????,???
    SCK_PIN = 1;    // ??3:?????????
    MOSI_PIN = 1;   // MOSI?????
}

// SPI??????
unsigned short SPI_ReadWord(void)
{
    unsigned short i, dat = 0;
	CS_PIN=0;
    MOSI_PIN=0;
    for(i = 0; i < 16; i++)
    {
        // ???????(??3:??????????)
        SCK_PIN = 0;
        _nop_();
        _nop_();
        
        // ??MISO??
        dat <<= 1;
        if(MISO_PIN)
            dat |= 0x01;
        
        // ???(??3:??????????)
        SCK_PIN = 1;
        _nop_();
        _nop_();
    }
    CS_PIN=1;
    return dat;
}
unsigned short SPI_ReadLong(void)
{
    unsigned long i, dat = 0;
	CS_PIN=0;
    MOSI_PIN=0;
    for(i = 0; i < 32; i++)
    {
        // ???????(??3:??????????)
        SCK_PIN = 0;
        _nop_();
        _nop_();
        
        // ??MISO??
        dat <<= 1;
        if(MISO_PIN)
            dat |= 0x01;
        
        // ???(??3:??????????)
        SCK_PIN = 1;
        _nop_();
        _nop_();
    }
    CS_PIN=1;
    return dat<<16;
}
void CAN_Send(uint8_t CANx,uint32_t canid,uint8_t* datagram){
	int i=0;
	
	CAN_Data.DLC=8;
	CAN_Data.RTR=0;
	CAN_Data.FF=0;
	CAN_Data.ID=canid;
	for(i=0;i<=7;i++){
		CAN_Data.DataBuffer[i]=datagram[i];
	}
	CAN_SendMsg(CANx,&CAN_Data);
	
}
uint16_t SPI_Read_MA600_Angle(){
	uint8_t SPI_Buffer_2[2];
	uint16_t angle_out=0;
	angle_out=SPI_ReadWord();
	return angle_out;
}
int32_t SPI_Read_MA600_Multi_Angle(){
	uint8_t SPI_Buffer_2[2];
	uint32_t multiangle_out=0;
	multiangle_out=SPI_ReadLong()*65536+SPI_Read_MA600_Angle();
	return (int32_t)multiangle_out;
}

int32_t last_composite_angle=0;
int16_t tmr_cycles=0;
void Task_TMR0(){

	int32_t composite_angle_diff=0;
	int32_t angle_rad100=0;
	int32_t composite_angle_rad100=0;
	int32_t omega_rad100=0;
	uint8_t CAN1_TX_Data[8];
	WDT_Clear();
	if(tmr_cycles<=(CAN_id&0x0f)*200){
		if(tmr_cycles%160<=0){
	P20=1;
	}
	else{
		P20=0;
	}
	}else{
		P20=0;
	}
	if(tmr_cycles>1000){
		tmr_cycles=0;
	}
	tmr_cycles++;
	
	//Angle Readings
	raw_angles.angle=SPI_Read_MA600_Angle();
	raw_angles.multi_angle=SPI_Read_MA600_Multi_Angle();
	Single_Rad=(raw_angles.angle/65536.0)*6.28;
	Multi_Rad=(raw_angles.multi_angle/65536.0)*6.28;//(raw_angles.multiturn*6.28)+Single_Rad;
		while(Single_Rad > 3.14){
		Single_Rad -= 6.28;
	}
	Single_Rad_16 = Single_Rad * 100.0f;
	Multi_Rad_16= Multi_Rad  * 100.0f;
	Omega_16 = (Multi_Rad-Last_Multi_Rad) *1000.0f * 100.0f;
	Last_Multi_Rad=Multi_Rad;
	CAN1_TX_Data[0]=0xA5;
	CAN1_TX_Data[7]=0xB5;
	CAN1_TX_Data[1]=Single_Rad_16&0x00ff;
	CAN1_TX_Data[2]=Single_Rad_16/256;
	CAN1_TX_Data[3]=Multi_Rad_16&0x00ff;
	CAN1_TX_Data[4]=Multi_Rad_16/256;
	CAN1_TX_Data[5]=Omega_16&0x00ff;
	CAN1_TX_Data[6]=Omega_16>>8;
	CAN_Send(CAN1,CAN_id,CAN1_TX_Data);
}

//<<AICUBE_USER_FUNCTION_IMPLEMENT_END>>


