#include "relay.h"

int  RelayOnflag = -1;
int  RelayOffflag = -1;

void Relay_GPIO_Init(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE); 

													   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOD, &GPIO_InitStructure);	

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOE, &GPIO_InitStructure);	  
		
		CAR_EAST(OFF);
		CAR_WEST(OFF);
		CAR_SOUTH(OFF);
		CAR_NORTH(OFF);
		PAW_UP(OFF);
		PAW_DOWN(OFF);
		PAW_CLOSE(OFF);
		PAW_RELEASE(OFF);
		CAR_START(OFF);
		CAR_STOP(OFF);
		GPIO_SetBits(GPIOD,GPIO_Pin_10);
		
}
//打开遥控器
void RelayOn(void)
{
	if(RelayOnflag>=0 && RelayOnflag <=25)
	{
		CAR_START(ON);
	}
	else if(RelayOnflag>=25 && RelayOnflag <=50)
	{		
		CAR_START(OFF); 
	}	
	else
	{
		RelayOnflag = -2;
	}	
	 
}
//关闭遥控器
void RelayOff(void)
{
	if(RelayOffflag>=0 && RelayOffflag <=25)
	{
		CAR_STOP(ON);
	}
	else if(RelayOffflag>=25 && RelayOffflag <=50)
	{		
		CAR_STOP(OFF);
	}	 
	else
	{
		RelayOffflag = -2;
	}	  
}
//上电
void PowerOn(void)
{
	if(RelayOnflag==-1)//开遥控器
	{
		RelayOnflag = 0;
	}
	if(RelayOnflag>=0)
	{
		RelayOn();//打开遥控器
	}
}
//断电
void PowerOff(void)
{
		if(RelayOffflag==-1)//关遥控器
		{
			RelayOffflag = 0;
		}
		if(RelayOffflag>=0)
		{
			CAR_EAST(OFF);
			CAR_WEST(OFF);
			CAR_NORTH(OFF);
			CAR_SOUTH(OFF);
			PAW_UP(OFF);
			PAW_DOWN(OFF);
			PAW_CLOSE(OFF);
			PAW_RELEASE(OFF);
			RelayOff();//关闭遥控器
		}
}
/*********************************************END OF FILE**********************/
