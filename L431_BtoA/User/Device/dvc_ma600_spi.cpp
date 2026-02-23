#include "dvc_ma600_spi.h"

HAL_StatusTypeDef Class_Encoder_MA600::Init(SPI_HandleTypeDef *hspi,GPIO_TypeDef *CSPort,uint16_t CSPin){
	if(CSPin<=GPIO_PIN_15){
			hspix=hspi;
			CSPortx=CSPort;
			CSPinx=CSPin;
	    return HAL_OK;
	}
	else{
		return HAL_ERROR;
	}
}
int16_t Class_Encoder_MA600::getMultiturn(void)
{
  uint32_t timeout=10;
  uint8_t txData[4];
  uint8_t rxData[4];
  txData[1]=0;
  txData[0]=0;
	txData[3]=0;
  txData[2]=0;
  int16_t mttSensor;
  HAL_GPIO_WritePin(CSPortx, CSPinx, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(hspix, txData, rxData, 4, timeout);
  HAL_GPIO_WritePin(CSPortx, CSPinx, GPIO_PIN_SET);
  mttSensor=rxData[2]<<8 | rxData[3];
  return mttSensor;
}

uint16_t Class_Encoder_MA600::getAngle(void)
{
  uint32_t timeout=100;
  uint8_t txData[2];
  uint8_t rxData[2];
  txData[1]=0;
  txData[0]=0;
  uint16_t angleSensor;
  HAL_GPIO_WritePin(CSPortx, CSPinx, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(hspix, txData, rxData, 2, timeout);
  HAL_GPIO_WritePin(CSPortx, CSPinx, GPIO_PIN_SET);
	rxData[1]&=0xF0;
  angleSensor=rxData[0]<<8 | rxData[1];
  return angleSensor;
}
uint16_t Class_Encoder_MA600::getAngle16(void)
{
  uint32_t timeout=100;
  uint8_t txData[2];
  uint8_t rxData[2];
  txData[1]=0;
  txData[0]=0;
  uint16_t angleSensor;
  HAL_GPIO_WritePin(CSPortx, CSPinx, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(hspix, txData, rxData, 2, timeout);
  HAL_GPIO_WritePin(CSPortx, CSPinx, GPIO_PIN_SET);
	rxData[1]&=0xFF;
  angleSensor=rxData[0]<<8 | rxData[1];
  return angleSensor;
}
float Class_Encoder_MA600::Get_Single_Angle(){//一圈360度的单圈角度
	return ((float)Class_Encoder_MA600::getAngle()/65536.0f)*360.0f;
}
float Class_Encoder_MA600::Get_Single_Angle16(){//一圈360度的单圈角度
	return ((float)Class_Encoder_MA600::getAngle16()/65536.0f)*360.0f;
}
float Class_Encoder_MA600::Get_Single_Radian(){//一圈360度的单圈弧度
	return ((float)Class_Encoder_MA600::getAngle()/65536.0f)*6.28;
}
float Class_Encoder_MA600::Get_Now_Angle(){//一圈360度的多圈角度
	uint32_t totalAngle=0;
	totalAngle+=Class_Encoder_MA600::getAngle();
	totalAngle+=Class_Encoder_MA600::getMultiturn()*65536;
	float totalAngleFL=totalAngle/65535.0;
	return totalAngleFL*360.0;
}
float Class_Encoder_MA600::Get_Now_Radian(){//一圈360度的多圈弧度
	int32_t totalAngle=0;
	totalAngle+=Class_Encoder_MA600::getAngle();
	totalAngle+=Class_Encoder_MA600::getMultiturn()*65536;
	float totalAngleFL=totalAngle/65535.0;
	return totalAngleFL*6.28;
}











