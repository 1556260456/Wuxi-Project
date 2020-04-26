#include "Control.h"
#include "bsp_led.h"
#include "Data_type.h"   
#include "uart4.h"
#include "usart2.h"
#include "beep.h"
#include "task.h"
#include "LCFunc.h"
#include "relay.h"

 /*
 *************************************************************************
 *                          全局变量
 *************************************************************************  
 */
extern uint8_t start_down ;
extern uint8_t down_time ;
extern uint8_t start_up ;
extern uint8_t up_time ;

Task_loop_t task_tim;

void ChoseTask(uint8_t choice)
{
  //TaskModeFlag = choice;
}



void control_task(void)
{

	task_tim.time_1ms++;
	task_tim.time_2ms++;	
	task_tim.time_5ms++;
	task_tim.time_10ms++;	
	task_tim.time_25ms++;
	task_tim.time_100ms++;	
	task_tim.time_1000ms++;	
	


	if(task_tim.time_5ms >= 10)
	{
	  task_tim.time_5ms -= 10;

	}
	else if(task_tim.time_10ms >= 20)
	{
	  task_tim.time_10ms -= 20;
		if(start_down == 1)
		{
			down_time++;
		}
		if(start_up == 1)
		{
			up_time++;
		}

	}
	else if(task_tim.time_25ms >= 50)
	{
	  task_tim.time_25ms -= 50;
	}
	else if(task_tim.time_100ms >= 200)
	{
		
	  task_tim.time_100ms -= 200;
		if((ReverseStop == 1)||(ReverseStop == 2))
		{
			ReverseTime++;
		}
		if((PointMove == 1)||(PointMove == 2))
		{
			PointMoveTime++;
		}
		
		if(CloseFlag == 1)
		{
			CloseDelay++;
		}
		if(OpenFlag == 1)
		{
			OpenDelay++;
		}		
		
		if(RelayOnflag>=0)
		{
			RelayOnflag++;
		}
		if(RelayOffflag>=0)
		{
			RelayOffflag++;
		}
	}
//	else if(task_tim.time_1000ms >= 2000)
//	{
//	  task_tim.time_1000ms -= 2000;
//    if (1==Big_Claw_Up_Delay_Flag)
//    {
//        Big_Claw_Up_Delay = Big_Claw_Up_Delay+1;
//    }
//    if (1==Big_Claw_Up_Delay_Pool_Flag)
//    {
//        Big_Claw_Up_Delay_Pool = Big_Claw_Up_Delay_Pool+1;
//    }   
//	}	
}



