#include "exti.h"

void EXTI10_Init(void)
{		
		GPIO_InitTypeDef GPIO_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;

		//IO口初始化
		RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOD, ENABLE); 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOD, &GPIO_InitStructure);	
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource10);//PD10 连接线10
	
		/* 配置EXTI_Line10 */
		EXTI_InitStructure.EXTI_Line = EXTI_Line10;//LINE10
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //触发条件
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE10
		EXTI_Init(&EXTI_InitStructure);
  
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//外部中断10
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//响应优先级0
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断
		NVIC_Init(&NVIC_InitStructure);//?? NVIC		
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line10)!=RESET)//判断某个线上的中断是否发生
	{
		
		EXTI_ClearITPendingBit(EXTI_Line10); //清除LINE10上的中断标志位
	}
}
/*********************************************END OF FILE**********************/
