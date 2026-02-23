
#ifndef _AS5600_
#define _AS5600_

#include "i2c.h"
#include "gpio.h"

extern I2C_HandleTypeDef hi2c1;

class Class_Encoder_5600
	{
		protected:
			I2C_HandleTypeDef* hi2cx;
		  GPIO_TypeDef* INVPortx;
		  uint16_t INVPinx;
	
	  public:		
	  uint16_t getAngle(void);//65535 16bit角度
	  HAL_StatusTypeDef Init(I2C_HandleTypeDef* hi2c,GPIO_TypeDef *INVPort,uint16_t INVPin,uint8_t INV);
		float Get_Single_Angle();//一圈360度的单圈角度
		float Get_Single_Radian();//一圈2PI的单圈弧度
		uint16_t GetMagnitude();//获取场强
		uint8_t GetStatus();//获取状态
	};
	
#endif
		
	
















