#include "Manual.h"
#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "relay.h"

 /*
 *************************************************************************
 *                       全局变量
 *************************************************************************  
 */
extern uint8_t WaitFlag;
extern uint8_t Run_Mode;

//手动状态下的运行步骤 1:停止,2:X,3:Y,4:上,4:下,6:抓,7:松,
uint8_t HTaskModeFlag = 0;

/*
*****************************************************************************************************************
*                                     void ManualXMoxing(float x)
*
*Description : 手动X方向移动
*Arguments   : 目标点x坐标
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualXMoving(float x)
{
//------------------------------------------------------------
	PowerOn();//行车上电
//------------------------------------------------------------	
	if((0!=HTaskModeFlag)||(0==SmallCarDataCorrect))//数据不正常
	{
		DataCommunicateManage(SMALL_CAR,1);//修改		
	}
	else if(1==SmallCarDataCorrect)//数据正常
	{		
		if(0==X_MOVE_BIT)
		{
				XMoving(x);	
		}
		else if(1==X_MOVE_BIT)//运行结束
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				X_MOVE_BIT = 0;//复位
				RequestStop(SMALL_CAR);
			}
		}
		else if(2==X_MOVE_BIT)//运行异常导致结束
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				X_MOVE_BIT = 0;//复位
			}
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ManualXMoxing(float y)
*
*Description : 手动Y方向移动
*Arguments   : 目标点y坐标
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualYMoving(float y)
{
//------------------------------------------------------------
	PowerOn();//行车上电
//------------------------------------------------------------	
	if((0!=HTaskModeFlag)||(0==SmallCarDataCorrect))//数据不正常
	{
		DataCommunicateManage(SMALL_CAR,1);//修改		
	}
	else if(1==SmallCarDataCorrect)//数据正常
	{
		
		if(0==Y_MOVE_BIT)
		{
				YMoving(y);
		}
		else if(1==Y_MOVE_BIT)//运行结束
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Y_MOVE_BIT = 0;//复位
				RequestStop(SMALL_CAR);
			}
		}
		else if(1==Y_MOVE_BIT)//运行结束
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Y_MOVE_BIT = 0;//复位
			}
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ManualClose(void)
*
*Description : 合拢小爪子
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualClose(void)
{
	//------------------------------------------------------------
	PowerOn();//行车上电
  //------------------------------------------------------------	
	if(CloseFlag==0)
	{
		CloseFlag=1;
	}
	else if(CloseFlag==1)
	{
		ClosePaw();
	}
	else if(CloseFlag==2)
	{
		PowerOff();//行车断电
		if(RelayOffflag==-2)
		{
			RelayOnflag = -1;//复位
			RelayOffflag = -1;//复位
			HTaskModeFlag=0;//无任务模式
			WaitFlag = 0;//进入等待状态
			CloseFlag = 0;//复位
		}		
	}
}
/*
*****************************************************************************************************************
*                                     void ManualOpen(void)
*
*Description : 张开小爪子
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualOpen(void)
{
	//------------------------------------------------------------
	PowerOn();//行车上电
  //------------------------------------------------------------
	if(OpenFlag==0)
	{
		OpenFlag=1;
	}
	else if(OpenFlag==1)
	{
		ClosePaw();
	}
	else if(OpenFlag==2)
	{
		PowerOff();//行车断电
		if(RelayOffflag==-2)
		{
			RelayOnflag = -1;//复位
			RelayOffflag = -1;//复位
			HTaskModeFlag=0;//无任务模式
			WaitFlag = 0;//进入等待状态
			OpenFlag = 0;//复位
		}		
	}	
}
/*
*****************************************************************************************************************
*                                     void ManualRaisePawFromLitterPool(float z)
*
*Description : 将爪子从垃圾池抬起  (差一个手动放下的程序)
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualRaisePawFromLitterPool(void)
{
	//------------------------------------------------------------
	PowerOn();//行车上电
  //------------------------------------------------------------
	
	if(1==SmallClawDataCorrect)//数据正常
	{
		if(UP_BIT == 0)//正在运行
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromLitterPool(ORIGIN_Z);		
		}
		else if(UP_BIT == 1)//正常运行结束
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				UP_BIT = 0;//复位
			}
		}
		else if(UP_BIT == 2)//运行过程中出错
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				UP_BIT = 0;//复位
			}	
		}
	}		
}
