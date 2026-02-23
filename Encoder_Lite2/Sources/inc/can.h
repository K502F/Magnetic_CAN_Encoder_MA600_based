//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 在此添加用户文件头说明信息  
// 文件名称: can.h
////////////////////////////////////////
//<<AICUBE_USER_HEADER_REMARK_END>>


#ifndef __CAN_H__
#define __CAN_H__


//<<AICUBE_USER_DEFINE_BEGIN>>
// 在此添加用户宏定义  
//<<AICUBE_USER_DEFINE_END>>


#define CAN1                    1
#define CAN2                    2

typedef struct                          //定义CAN数据收发数据结构
{
    uint8_t     DLC : 4;                //数据长度, bit[3:0]
    uint8_t         : 2;                //空数据, bit[5:4]
    uint8_t     RTR : 1;                //帧类型, bit6
    uint8_t     FF  : 1;                //帧格式, bit7
    uint32_t    ID;                     //CAN ID
    uint8_t     DataBuffer[8];          //数据缓存
} CAN_DataDef;

void CAN1_Init(void);
uint8_t CAN_ReadMsg(uint8_t CANx, CAN_DataDef *pCAN);
void CAN_SendMsg(uint8_t CANx, CAN_DataDef *pCAN);

extern BOOL fCAN1SendBusy;
extern BOOL fCAN1ReadyRead;


//<<AICUBE_USER_EXTERNAL_DECLARE_BEGIN>>
// 在此添加用户外部函数和外部变量声明  
//<<AICUBE_USER_EXTERNAL_DECLARE_END>>


#endif
