#include "task.h"
#include "drv_tim.h"
#include "drv_can.h"
#include "drv_dwt.h"
#include "drv_crclite.h"
#include "dvc_djimotor.h"

#include "dvc_ma600_spi.h"
#include "stm32l4xx_hal_tim.h"
#include "stm32l4xx_hal.h"
#include "string.h"
#include "alg_filter.h"
#include <stdlib.h>
#include <math.h>
#define HW_VERSION 431 
#define MAGNET_SURFACE_GAUSS 4000 
Class_Encoder_MA600 ma600;
Class_crclite crclite;
#if HW_VERSION == 11
Class_Encoder_5600 as5600;
#define HAVE_5600_NO_24C02
#endif

uint16_t CAN_ID=0XD1;
uint16_t MagThres=0x0F10;
	
extern TIM_HandleTypeDef htim16;
extern LPTIM_HandleTypeDef hlptim1;
	
extern SPI_HandleTypeDef hspi1;
extern CAN_HandleTypeDef hcan1;

extern WWDG_HandleTypeDef hwwdg;
	
void Task1ms_TIM3_Callback()
{

}

float single_Angle=0.0f;
float single_Angle_Rad=0.0f;
float last_multi_Angle=0.0f;
float speed_1ms=0.0f;
uint16_t angle=0;
float multi_Angle=0.0f;
double hysteretic_deg=0;
double hysteretic_sin=0;
double hysteretic_deg_m=0;
double hysteretic_sin_m=0;
double delta,actual_delta,single_Angle_calc,last_single_Angle;
double delta_m,actual_delta_m,multi_Angle_calc;
double single_Rad_IIR;
double single_Angle_IIR;
float last_speed=0;
float last_delta=1;
float last_delta_m=1;
float speed_calc=0;
int firstflag=1;
uint16_t angle16=0;
int16_t mtt16=0;
float angle_deg=0;





Class_Filter_Kalman Radian_Filter;
float Filter_multi_Angle, Filter_Single_AngleR, Filter_Single_Angle,Last_multi_Angle;
float single_Angle_RAD;
int cntcyc=0;

int16_t Multi_Rad_16;
	int16_t Omega_16;
	int16_t Single_Rad_16;
float as5600_Angle;
uint8_t as5600_Status;
uint16_t as5600_Magnitude;
void Task1ms_TIM2_Callback()
{

	#ifdef HAVE_5600_NO_24C02
	as5600_Angle=as5600.Get_Single_Radian();
	as5600_Magnitude=as5600.GetMagnitude();
	as5600_Status=as5600.GetStatus();
	#endif
	angle16=ma600.getAngle();
	mtt16=ma600.getMultiturn();
	single_Angle_Rad=ma600.Get_Single_Radian();
	angle_deg=single_Angle=ma600.Get_Single_Angle();
	
	multi_Angle=ma600.Get_Now_Radian();
	speed_1ms=(multi_Angle-last_multi_Angle)*1000.0f;//单位：弧度每秒
	
	#if MAGNET_SURFACE_GAUSS < 3000
	Radian_Filter.Set_Now(multi_Angle);
	Radian_Filter.Recv_Adjust_PeriodElapsedCallback();
	Filter_multi_Angle = Radian_Filter.Get_Out();
	#endif
	Filter_Single_AngleR = Filter_multi_Angle;
	while(Filter_Single_AngleR > PI){
		Filter_Single_AngleR -= 2 * PI;
	}
	while(Filter_Single_AngleR < -PI){
		Filter_Single_AngleR += 2 * PI;
	}
	Filter_Single_Angle = Filter_Single_AngleR;

	if(firstflag==1){
		last_speed=speed_1ms;
		firstflag=0;
	}
	if(last_single_Angle==0){
		last_single_Angle=0.5;
	}
	while(single_Angle_Rad > PI){
		single_Angle_Rad -= 2 * PI;
	}

	Single_Rad_16 = single_Angle_Rad * 100.0f;
	if(HW_VERSION==1.1&&as5600_Magnitude>=MagThres){//对于V1.1的硬件版本，其具有测量场强功能，场强太弱时启动卡尔曼滤波
	Multi_Rad_16= Filter_multi_Angle  * 100.0f;
	Omega_16 = (Filter_multi_Angle-Last_multi_Angle) *1000.0f * 100.0f;
		Last_multi_Angle=Filter_multi_Angle;
	}
	else{//V1.0,V1.2无场强测量功能，依据宏定义里的场强判定
		if(MAGNET_SURFACE_GAUSS>3000){
		Multi_Rad_16 = multi_Angle  * 100.0f;
	Omega_16 = (multi_Angle-Last_multi_Angle) *1000.0f * 100.0f;
		Last_multi_Angle=multi_Angle;
		}else{
			Multi_Rad_16= Filter_multi_Angle  * 100.0f;
	Omega_16 = (Filter_multi_Angle-Last_multi_Angle) *1000.0f * 100.0f;
		Last_multi_Angle=Filter_multi_Angle;
		}
	}
	//发送数据 A5 单圈*100[15:0] 多圈*100[15:0] 圈速*100[15:0] B5   小端在前
		CAN1_0x200_Tx_Data[0]=0xA5;
		CAN1_0x200_Tx_Data[7]=0xB5;
	memcpy(CAN1_0x200_Tx_Data+1,&Single_Rad_16,sizeof(Single_Rad_16));
	memcpy(CAN1_0x200_Tx_Data+3,&Multi_Rad_16,sizeof(Multi_Rad_16));
	memcpy(CAN1_0x200_Tx_Data+5,&Omega_16,sizeof(Omega_16));
	//crclite.Append_CRC16_Check_Sum(CAN1_0x200_Tx_Data,8);
    /****************************** 驱动层回调函数 1ms *****************************************/ 
        //统一打包发送
	HAL_WWDG_Refresh(&hwwdg);//喂狗
    TIM_CAN_PeriodElapsedCallback();
}

void callback(Struct_CAN_Rx_Buffer *CAN_RxMessage)//CAN1接收数据回调，此处不用
{
    switch (CAN_RxMessage->Header.StdId)
    {
    case (0x4A):   
    {
        
    }
	break;
	}
}
void CAN_ID_Init(){
	#if HW_VERSION == 12
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
	HAL_I2C_Mem_Read(&hi2c1,0xA0,0x04,I2C_MEMADD_SIZE_16BIT,(uint8_t*)&CAN_ID,1,100);//从EEPROM读取初始ID
	#endif
	#if HW_VERSION == 11
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
	#endif
}
	int pressed=0;
		int pressed_flag=0;
int pressed_times=0;
uint32_t tick=0;
uint32_t pl=0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
//	if(GPIO_Pin==GPIO_PIN_2){
/*		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
		//进入设置ID模式
		HAL_NVIC_DisableIRQ(TIM2_IRQn);
		HAL_NVIC_DisableIRQ(TIM3_IRQn);
		HAL_NVIC_DisableIRQ(CEC_CAN_IRQn);
	//__disable_irq();
//	htim3.Instance->CCR3=0;
//	htim3.Instance->PSC=47;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==0){
		//	HAL_Delay(1);
			pressed++;
			HAL_IWDG_Refresh(&hiwdg);
			htim3.Instance->CCR3=pressed/1024;
			if(pressed>=0X1FFFFF){//粗略计时 大概3.5秒
				pressed_flag=1;
				htim3.Instance->CCR3=4095;
				pressed=0;
				break;
		}
	}
	
			pressed=0;
		htim3.Instance->CCR3=0;
	  htim3.Instance->PSC=4700;
		if(pressed_flag==1){
			pressed_times=0;
			tick=htim16.Instance->CNT;
			for(pl=tick;pl<=tick+4000;pl=htim16.Instance->CNT){
				HAL_IWDG_Refresh(&hiwdg);
				if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==0){
					pressed_times++;
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);
					while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==0){
						HAL_IWDG_Refresh(&hiwdg);
						HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
					}
					int tick2=htim16.Instance->CNT;
						for(pl=tick2;pl<=tick2+1;pl=htim16.Instance->CNT){
							HAL_IWDG_Refresh(&hiwdg);}
						
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
				}
			}
			CAN_ID=0xD0+pressed_times-1;
			
		//	HAL_I2C_Mem_Write(&hi2c1,0xA0,0x04,I2C_MEMADD_SIZE_16BIT,(uint8_t*)&CAN_ID,1,100);
			pressed_flag=0;
		}
			
			
			
				HAL_NVIC_EnableIRQ(TIM2_IRQn);
		HAL_NVIC_EnableIRQ(TIM6_IRQn);
	//	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_3);
			//__enable_irq();
		//}*/
	}
extern "C" void Task_Init()
{  

    //DWT_Init(168);
   CAN_ID_Init();
    /********************************** 驱动层初始化 **********************************/
    CAN_Init(&hcan1,callback);
    //定时器循环任务
    TIM_Init(&htim2, Task1ms_TIM2_Callback);
    TIM_Init(&htim6, Task1ms_TIM3_Callback);

    /********************************* 设备层初始化 *********************************/

     //设备层集成在交互层初始化中，没有显视地初始化

    /********************************* 交互层初始化 *********************************/

    // imu_test.Init();
    // yaw_test.Init(&hcan1,DJI_Motor_ID_0x202);
    // yaw_test.PID_Angle.Init(2,0,0,0,5,180,0);
    // yaw_test.PID_Omega.Init(2,0,0,0,5,8000,0);
  ma600.Init(&hspi1,GPIOA,GPIO_PIN_4);
	HAL_WWDG_Init(&hwwdg);
	#ifdef HAVE_5600_NO_24C02
	 as5600.Init(&hi2c1,GPIOA,GPIO_PIN_3,1);
	#endif
	Radian_Filter.Init(0.5, 0.02, 0.0);
    /********************************* 使能调度时钟 *********************************/
HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start_IT(&htim6);
		HAL_TIM_Base_Start(&htim16);
		HAL_LPTIM_TimeOut_Start_IT(&hlptim1,999,999);
		//HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
  //  HAL_TIM_Base_Start_IT(&htim16);
		

}