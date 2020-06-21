/**
  ******************************************************************************
  * @file    main.c
  * @author  yueran
  * @version V1.0 ���
  * @date    2020-04-18
  * @brief   �����̷���Ŀ--Сצ��
  *****************************************************************************
  * @attention
  *
  * ʵ��ƽ̨: STM32 F407VET6
  *
  ******************************************************************************
  */
	
	
 /*
 *************************************************************************
 *                        ������ͷ�ļ�
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
 *                          ȫ�ֱ���
 *************************************************************************  
 */
//�ȴ���־λ  ��λ�����ݴ���ʱ WaitFlag = 1������WaitFlag = 0��
uint8_t WaitFlag = 0;
//����ģʽ 1���ֶ�   2�����Զ�   3��ȫ�Զ�
uint8_t Run_Mode = 0;
//0:���ϴ���1:�ϴ�
uint8_t Up_Data_Flag = 0;
int test_distance = 0; //�������Զ�����ʹ��
POSITION origin;//��ʼλ��
POSITION target;//Ŀ��λ�� 

 /*
 *************************************************************************
 *                        ��������
 *************************************************************************  
 */
void BSP_Init(void);
int main(void)
{	
	//uint8_t Choice = 0;
 	SysTick_Init();                 //ϵͳʱ�ӳ�ʼ��
  BSP_Init();  	//���Ӳ����ʼ�� 
	laser.last_dis1 = laser.dis1;
	laser.last_dis2 = laser.dis2;
	laser.last_dis3 = laser.dis3; 
	while(0==SmallCarDataCorrect)
	{
		DataCommunicateManage(SMALL_CAR,1);//��������
		delay_ms(1000);
	
	}
//	while((0==SmallClawDataCorrect))
//	{ 
//		DataCommunicateManage(SMALL_CLAW,1);//��������
//		delay_ms(1000);
//	}
	SmallClawDataCorrect = 1;//���Զ�����
	Up_Data.Status = Up_Data.Status|0xF0;	//��ʼ״̬��Ϊ����״̬�����λ��1
	
	//��ʼֵ
//	target.x[0] = 3000;
//	target.y[0] = 4000; 
//	target.z[0] = 1000; 
//	target.x[1] = 5000;
//	target.y[1] = 6000; 
//	target.z[1] = 7000; 
//	
//	origin.x[0] = 3500; 
//	origin.y[0] = 4000; 
//	origin.z[0] = 7000;  
	


  //SelfCheckStatus();//���������Լ����
//	WaitFlag = 2;
//	Run_Mode = 1;
//	HTaskModeFlag = 3;
	
	TIM_Cmd(TIM7,ENABLE); 
	test_distance  = 4000;
	while(1) 
  { 
		if(task_tim.time_10ms >= 20)//��������ÿ��10ms
		{
			task_tim.time_10ms -= 20;
			if(1 == WaitFlag)//���յ���λ����������
			{				
				//ChoseTask(Choice);
				/*��Run_Mode��ֵ��ָʾ��������ģʽ*/
				//HTaskModeFlag = 0;
				WaitFlag = 2;//���յ�ָ����������
			}
			else if(0 == WaitFlag)/*�ȴ���λ����������*/
			{
				/*�ȴ���λ����������*/
			}
			else if(2 == WaitFlag)//���յ�ָ����������
			{
				if(1==Run_Mode)//�ֶ�
				{
					//����ָ��ѡ��Ҫִ�е�����
					switch(HTaskModeFlag)
					{
						case 1: //X
							ManualXMoving(target.x[1]);
							break;
						case 2: //Y
							ManualYMoving(target.y[1]);
							break;
						case 3: //������������
							ManualRaisePawFromLitterPool(target.z[1]);
							break;				
						case 6: //�½���������
							ManualDowntoLitterPool(target.z[1]);
							break;
						case 9: //ץ��
							ManualClose();
							break;	
						case 10:  //����
							ManualOpen();
							break;	
						case 7: //��ң����
							ManualPowerOn();
							break;
						case 8:   //�ر�ң����
							ManualPowerOff();
							break;
						default:
							break;
					}			
				}
				else if(2==Run_Mode)//���Զ�
				{
					SmallCarHalfAutoMode();
				}
				else if(3==Run_Mode)//ȫ�Զ�
				{
					SmallCarFullAutoMode();
				}
			}
		}
		
		if(task_tim.time_200ms >= 400)//100ms����
		{

			if(1==Up_Data_Flag)
			{
				RS485_Send_Data();//100ms�ϴ�һ������		
				Up_Data_Flag=0;				
			}
			//��������
			if((ErrorSmallCar)||(ManualError==1))
			{
 				ALARM_ON;
			}
		}
	}
}

void BSP_Init(void)
{
  LED_Init();                     //LED��ʼ��
	Key_Init();                     //������ʼ��
	ADC_DMA_Init();                 //ADC_DMA��ʼ��
	Alarm_GPIO_Init();              //�����˿ڳ�ʼ��
	Relay_GPIO_Init();              //�̵����˿ڳ�ʼ��
	EXTIX_Init();                  //�ⲿ�жϳ�ʼ��(���ڽ���ֹͣ��ť )
	USART1_Init(115200);            //USART1��ʼ��(���մ�������433)
	USART2_Init(115200);            //USART2��ʼ��(����Զ�ͨ�ŵ�433)
	RS485_Init(115200);             //RS485��ʼ��
	UART4_Init(115200);	            //UART4��ʼ��(���ڵ�����)
	TIM7_Init(500-1,84-1);          //f=2kHZ,T=0.5ms 
}
