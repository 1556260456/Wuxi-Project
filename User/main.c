/**
  ******************************************************************************
  * @file    main.c
  * @author  yueran
  * @version V1.0 裸机
  * @date    2020-04-18
  * @brief   无锡固废项目--小爪子
  *****************************************************************************
  * @attention
  *
  * 实验平台: STM32 F407VET6
  *
  ******************************************************************************
  */
	
	
 /*
 *************************************************************************
 *                        包含的头文件
 *************************************************************************  
 */
#include "stm32f4xx.h"
#include "bsp_SysTick.h"
#include "GPIO.h"
#include "bsp_led.h" 
#include "bsp_basic_tim.h"
#include "ADC.h"
#include "relay.h"
#include "exti.h"
#include "alarm.h"
#include "Data_type.h" 
#include "bsp_key.h" 
#include "usart1.h"
#include "usart2.h"
#include "RS485.h"
#include "uart4.h"
#include "Control.h"	
#include "Manual.h"
#include "LCFunc.h"
#include "FullAuto.h"
#include "HalfAuto.h"
 /*
 *************************************************************************
 *                          全局变量
 *************************************************************************  
 */
//等待标志位  上位机数据传来时 WaitFlag = 1；否则WaitFlag = 0。
uint8_t WaitFlag = 0;
//运行模式 1：手动   2：半自动   3：全自动
uint8_t Run_Mode = 0;
//0:不上传，1:上传
uint8_t Up_Data_Flag = 0;

POSITION origin;//起始位置
POSITION target;//目标位置 

 /*
 *************************************************************************
 *                        函数声明
 *************************************************************************  
 */
void BSP_Init(void);
int main(void)
{	
	//uint8_t Choice = 0;
	SysTick_Init();                 //系统时钟初始化
  BSP_Init();                     //相关硬件初始化 
	
	while((0==SmallClawDataCorrect)||(0==SmallCarDataCorrect))
	{
		DataCommunicateManage(SMALL_CLAW,1);//请求数据
		DataCommunicateManage(SMALL_CAR,1);//请求数据
	}
	Up_Data.Status = Up_Data.Status|0x80;	//初始状态设为正常状态，最高位置1
	
	//初始值
//	target.x[0] = 13000;
//	target.y[0] = 4000; 
//	target.z[0] = 1600; 
//	origin.x[0] = 7900; 
//	origin.y[0] = 4000; 
//	origin.z[0] = 1700;  

  //SelfCheckStatus();//开机启动自检程序
	
	while(1) 
  { 
		if(task_tim.time_10ms >= 20)//运行任务每次10ms
		{
			if(1 == WaitFlag)//已收到上位机传来数据
			{				
				//ChoseTask(Choice);
				/*给Run_Mode赋值，指示何种运行模式*/
				//HTaskModeFlag = 0;
				WaitFlag = 2;//已收到指令正在运行
			}
			else if(0 == WaitFlag)/*等待上位机发送命令*/
			{
				/*等待上位机发送命令*/
			}
			else if(2 == WaitFlag)//已收到指令正在运行
			{
				if(1==Run_Mode)//手动
				{
					//根据指令选择要执行的任务
					switch(HTaskModeFlag)
					{
						case 1: //X
							ManualXMoving(target.x[0]);
							break;
						case 2: //Y
							ManualYMoving(target.y[0]);
							break;
						case 3: //从垃圾池上升
							
							break;				
						case 4: //下降到垃圾池
							
						case 5: //抓料
							ManualClose();
							break;	
						case 6:  //放料
							ManualOpen();
							break;							
						default:
							break;
					}			
				}
				else if(2==Run_Mode)//半自动
				{
					SmallCarHalfAutoMode();
				}
				else if(3==Run_Mode)//全自动
				{
					SmallCarFullAutoMode();
				}
			}
		}
		
		if(task_tim.time_100ms >= 200)//100ms发送
		{
			if(1==Up_Data_Flag)
			{
				RS485_Send_Data();//100ms上传一次数据		
				Up_Data_Flag=0;				
			}
			//出错报警
			if(!ErrorSmallCar)
			{
				ALARM_ON;
			}
		}
	}	 
}

void BSP_Init(void)
{
  LED_Init();                     //LED初始化
	Key_Init();                     //按键初始化
	ADC_DMA_Init();                 //ADC_DMA初始化
	Alarm_GPIO_Init();              //报警端口初始化
	Relay_GPIO_Init();              //继电器端口初始化
	EXTI10_Init();                  //外部中断初始化(用于紧急停止按钮 )
	USART1_Init(115200);            //USART1初始化(接收传感器的433)
	USART2_Init(115200);            //USART2初始化(与电脑端通信的433)
	RS485_Init(115200);             //RS485初始化
	//UART4_Init(115200);	            //UART4初始化(用于调试用)
	TIM7_Init(500-1,84-1);          //f=2kHZ,T=0.5ms 
}


