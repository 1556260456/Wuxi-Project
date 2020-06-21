#include "FullAuto.h"
#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "usart2.h"
#include "relay.h" 

/*
*************************************************************************
*                       全局变量
*************************************************************************
*/

extern uint8_t WaitFlag;
extern uint8_t Run_Mode;
extern int test_distance;

extern POSITION origin;//起始位置
extern POSITION target;//目标位置 
uint8_t FullAutoStep = 0;
/*
*****************************************************************************************************************
*                                    void SmallCarFullAutoMode(void)
*
*Description : 全自动任务执行函数
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void SmallCarFullAutoMode(void)
{
	if(0==FullAutoStep)//行车上电
 	{
		PowerOn();
		if(RelayOnflag==-2)
		{
			FullAutoStep++;//进行下一步
			RelayOnflag=-1;//标志位复位
			mpu.dis = test_distance;// 仅供全自动测试使用
		}
		//Up_Data.Status = (Up_Data.Status&0xF0)|(0xF1);	
	}
	else if(1==FullAutoStep)// 检测小行车是否在初始位置、初始状态
	{
		BackToOriginState();
		if(UP_BIT == 1)//上升结束
		{
			FullAutoStep++;//进行下一步
			UP_BIT=0;//标志位复位
		}
	}
	else if(2==FullAutoStep)//水平移动到抓料处正上方
	{
		HorizontalMoving(target.x[0],target.y[0]);
		if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
		{
			FullAutoStep++;//进行下一步
			X_MOVE_BIT=0;//标志位复位
			Y_MOVE_BIT=0;//标志位复位
<<<<<<< HEAD
			mpu.dis = test_distance;// 仅供全自动测试使用
=======
>>>>>>> f02ed4fcf33be44e9cc1f4e11d06fb0a95e09135
		}
	}
	else if(3==FullAutoStep)//爪子开始下降去抓料
	{
		DowntoLitterPool(target.z[0]);
		if(1==DOWN_BIT)
		{
			FullAutoStep++;//进行下一步
			DOWN_BIT = 0;//标志位复位
			CloseFlag=1;//将下面执行的抓料标志位置1
		}
	}
	else if(4==FullAutoStep)//执行抓料动作
	{
		HFClosePaw();
		if(-2==CloseFlag)
		{
			FullAutoStep++;//进行下一步
			CloseFlag = 0; //标志位复位
<<<<<<< HEAD
			mpu.dis = test_distance;// 仅供全自动测试使用
=======
>>>>>>> f02ed4fcf33be44e9cc1f4e11d06fb0a95e09135
		}
	}
	else if(5==FullAutoStep)//执行小爪上升动作
	{
		RisePawFromLitterPool();
		if(1==UP_BIT)
		{
			FullAutoStep++;//进行下一步
			UP_BIT = 0;//标志位复位
		}
	}
	else if(6==FullAutoStep)//水平移动至放料位置
	{
		HorizontalMoving(target.x[1],target.y[1]);
		if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
		{
			FullAutoStep++;//进行下一步
			X_MOVE_BIT=0;  //标志位复位
			Y_MOVE_BIT=0;  //标志位复位
			mpu.dis = test_distance;// 仅供全自动测试使用
		}
	}
	else if(7==FullAutoStep)//下放小爪子靠近垃圾池
	{
		DowntoLitterPool(target.z[1]);
		if(1==DOWN_BIT)
		{
			FullAutoStep++;//进行下一步
			DOWN_BIT=0;    //标志位复位	
			OpenFlag=1;    //将松开爪子标志位置1
		}
	}
	else if(8==FullAutoStep)//松开爪子放料
	{
		HFOpenPaw();
		if(-2==OpenFlag)
		{
			FullAutoStep++;//进行下一步
			OpenFlag=0;    //标志位复位	
			mpu.dis = test_distance;// 仅供全自动测试使用
		}
	}
	else if(9==FullAutoStep)//提升爪子至安全高度
	{
		RisePawFromLitterPool();
		if(1==UP_BIT)
		{
			FullAutoStep++;//进行下一步
			UP_BIT = 0;
		}
	}
	else if(10==FullAutoStep)//水平移动到初始位置上方
	{
		HorizontalMoving(ORIGIN_X,ORIGIN_Y);
		if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
		{
			FullAutoStep++;//进行下一步
			X_MOVE_BIT=0;//标志位复位
			Y_MOVE_BIT=0;//标志位复位
		}
	}
	else if(11==FullAutoStep)//断开电源,运行结束
	{
		PowerOff();
		if(RelayOffflag==-2)
		{
			Up_Data.Status = (Up_Data.Status&0x87)|0x40;//此时的状态值 完成
			RelayOffflag=-1;
			WaitFlag=0;      
			Run_Mode=0;
		}
	}
	if((2==X_MOVE_BIT)||(2==Y_MOVE_BIT)||(2==DOWN_BIT)||(2==UP_BIT))//运行出错
	{
		PowerOff();
		if(RelayOffflag==-2)
		{
			ErrorSmallCar = FullAutoStep;//记录出错在哪一步
			Up_Data.Status = Up_Data.Status&0xF0;	
			RelayOffflag=-1;
			FullAutoStep=0;
			X_MOVE_BIT=0;//标志位复位
			Y_MOVE_BIT=0;//标志位复位
			DOWN_BIT=0;
			UP_BIT=0;
			WaitFlag=0;      
			Run_Mode=0;
		}	
	}
}

/*
*****************************************************************************************************************
*                                    void HorizontalMoving(float x,float y)
*
*Description : 水平移动
*Arguments   : （x,y）:目的地坐标
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void HorizontalMoving(float x,float y)
{
	if(1==SmallCarDataCorrect)//数据正常
	{
		if(X_MOVE_BIT == 0)
		{
			//小行车X方向移动
			XMoving(x);			
		}
		else if(Y_MOVE_BIT == 0)
		{
			//小行车Y方向移动
			YMoving(y);		
		}
		else
		{
			/*水平移动结束*/
		}
	}
}

/*
*****************************************************************************************************************
*                                   void DowntoLitterPool(float z)
*
*Description : 竖直下降到料池底部抓料
*Arguments   : z:爪子下降到距垃圾的高度
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void DowntoLitterPool(float z)
{
	if(1==SmallClawDataCorrect)//数据正常
	{
		if(DOWN_BIT == 0)
		{
		  DownPawToLitterPool(z);		
		}
		else if(DOWN_BIT == 1)
		{
			/*下降结束*/
		}
	}
}
/*
*****************************************************************************************************************
*                                     void HClosePaw(void)
*
*Description : 抓料函数
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void HFClosePaw(void)
{
	ClosePaw();
}
/*
*****************************************************************************************************************
*                                void RisePawFromLitterPool(void)
*
*Description : 从垃圾池抬升爪子
*Arguments   : none
*Returns     : none
*Notes       : 刚开始用往下射的激光，运行到上半部分用往上射的激光
*****************************************************************************************************************
*/
void RisePawFromLitterPool(void)
{
	if(1==SmallClawDataCorrect)//数据正常
	{
		if(UP_BIT == 0)
		{
		  UpPawFromLitterPool(ORIGIN_Z);		
		}
		else if(UP_BIT == 1)
		{
			/*上升结束*/
		}
	}	
}

/*
*****************************************************************************************************************
*                                    void MoveBackToOriginState(float x,float y)
*
*Description : 先提升爪子，再水平移动至初始位置
*Arguments   : origin（x,y,z）:目的地坐标
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void BackToOriginState(void)
{
	if((1==SmallClawDataCorrect) && (1==SmallCarDataCorrect))//数据正常
	{
		if( (mpu.dis>ORIGIN_Z) && (UP_BIT==0))
		{
			UpPawFromLitterPool(ORIGIN_Z);
		}
//		if((X_MOVE_BIT == 0) && (Y_MOVE_BIT == 0))
//		{
//			if((laser.dis2<1300) || (laser.dis2>1700))
//			{
//				HorizontalMoving(ORIGIN_X,ORIGIN_Y);
//			}
//		}
	}
}

/*
*****************************************************************************************************************
*                                     void HFOpenPaw(void)
*
*Description : 放料函数
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void HFOpenPaw(void)
{
	ReleasePaw();
}
