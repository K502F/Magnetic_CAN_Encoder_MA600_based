#include "dvc_as5600_3.h"
#define ADDR 0x36
#define ANGLE_HIGH 0x0e
#define ANGLE_LOW 0x0f
#define MAGNITUDE_HIGH 0x1b
#define MAGNITUDE_LOW 0x1c
#define STATUS 0x0b
HAL_StatusTypeDef Class_Encoder_5600::Init(I2C_HandleTypeDef *hi2c,GPIO_TypeDef *INVPort,uint16_t INVPin,uint8_t INV){
	if(INVPin<=GPIO_PIN_15){
			hi2cx=hi2c;
			INVPortx=INVPort;
			INVPinx=INVPin;
		if(INV==1){
		HAL_GPIO_WritePin(INVPortx,INVPinx,GPIO_PIN_SET);
		}else{
			HAL_GPIO_WritePin(INVPortx,INVPinx,GPIO_PIN_RESET);
		}
//		HAL_I2C_Mem_Write(hi2cx,ADDR,ANGLE_HIGH,I2C_MEMADD_SIZE_8BIT,&highbyte,1,100);
	    return HAL_OK;
	}
	else{
		return HAL_ERROR;
	}
}

uint16_t Class_Encoder_5600::getAngle(void)
{
 uint8_t highbyte;
 uint8_t lowbyte;
	uint16_t result=0;
	HAL_I2C_Mem_Read(hi2cx,ADDR,ANGLE_LOW,I2C_MEMADD_SIZE_16BIT,&lowbyte,1,100);
	hi2cx->ErrorCode=0;
	for(int i=0;i<=9999;i++){
		__NOP();
	}
	HAL_I2C_Mem_Read(hi2cx,ADDR,ANGLE_HIGH,I2C_MEMADD_SIZE_16BIT,&highbyte,1,100);
	hi2cx->ErrorCode=0;
	result+=lowbyte;
		highbyte=highbyte<<4;
	result+=highbyte<<4;
	return result;
}

float Class_Encoder_5600::Get_Single_Angle(){//一圈360度的单圈角度
	return ((float)Class_Encoder_5600::getAngle()/4096.0f)*360.0f;
}

float Class_Encoder_5600::Get_Single_Radian(){//一圈360度的单圈弧度
	return ((float)Class_Encoder_5600::getAngle()/4096.0f)*6.28;
}
uint16_t Class_Encoder_5600::GetMagnitude(){
	uint8_t highbyte;
 uint8_t lowbyte;
	uint16_t result=0;
	float magnitude=0.0f;
	HAL_I2C_Mem_Read(hi2cx,ADDR,MAGNITUDE_LOW,I2C_MEMADD_SIZE_8BIT,&lowbyte,1,100);
	hi2cx->ErrorCode=0;
	for(int i=0;i<=9999;i++){
		__NOP();
	}
	HAL_I2C_Mem_Read(hi2cx,ADDR,MAGNITUDE_HIGH,I2C_MEMADD_SIZE_8BIT,&highbyte,1,100);
	hi2cx->ErrorCode=0;
	result+=lowbyte;
	highbyte=highbyte<<4;
	result+=highbyte<<4;
return result;
}
uint8_t Class_Encoder_5600::GetStatus(){
	uint8_t status;
	HAL_I2C_Mem_Read(hi2cx,ADDR,STATUS,I2C_MEMADD_SIZE_8BIT,&status,1,100);
	hi2cx->ErrorCode=0;
	status=status&0x38;
  return status;
}








