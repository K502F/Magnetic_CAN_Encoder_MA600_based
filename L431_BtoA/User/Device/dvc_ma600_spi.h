
#ifndef _MA600_SPI
#define _MA600_SPI



#include "spi.h"
#include "gpio.h"

extern SPI_HandleTypeDef hspi1;

class Class_Encoder_MA600
	{
		protected:
			SPI_HandleTypeDef* hspix;
		  GPIO_TypeDef* CSPortx;
		  uint16_t CSPinx;
	
	  public:		
				int16_t getMultiturn(void);//65535 16bit多圈
	  uint16_t getAngle(void);//65535 16bit角度
		  uint16_t getAngle16(void);//65535 16bit角度
	  HAL_StatusTypeDef Init(SPI_HandleTypeDef *hspi,GPIO_TypeDef *CSPort,uint16_t CSPin);
		float Get_Now_Angle();//一圈360度的多圈角度
		float Get_Now_Radian();//一圈2PI的多圈弧度
		float Get_Single_Angle();//一圈360度的单圈角度
		float Get_Single_Angle16();//一圈360度的单圈角度
		float Get_Single_Radian();//一圈2PI的单圈弧度
	};
	
#endif
		
	
















