/***************************************************************************

所谓半自动，即在爪子抓完料上升到最高点和爪子水平移动到焚料池上方时请求确认

****************************************************************************/
#include "HalfAuto.h"
#include "FullAuto.h"
#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "usart2.h"
#include "relay.h"

 /*
 *************************************************************************
 *                        宏定义
 *************************************************************************  
 */
 //1:在此步完成后需要请求是否执行下一步
 //0:在此步完成后不需要请求，直接执行下一步
 #define RESTEP_1      0// 是否从四楼平台抬升爪子
 #define RESTEP_2      0// 是否水平移动到抓料处正上方
 #define RESTEP_3      0// 是否爪子开始下降去抓料
 #define RESTEP_4      0// 是否执行抓料动作
 #define RESTEP_5      0// 是否执行大爪上升动作
 #define RESTEP_6      0// 是否水平移动至焚烧池上方
 #define RESTEP_7      0// 是否下放大爪子靠近焚烧池
 #define RESTEP_8      0// 是否提升爪子至安全高度
 #define RESTEP_9      0// 是否水平移动到初始位置上方
 #define RESTEP_10     0// 是否将爪子下降到四楼平台
 #define RESTEP_11     0// 是否断开电源,运行结束
 /*
 *************************************************************************
 *                       全局变量
 *************************************************************************  
 */
extern POSITION origin;//起始位置
extern POSITION target;//目标位置 

extern uint8_t WaitFlag;
extern uint8_t Run_Mode;

uint8_t HalfAutoStep = 0;
uint8_t SingleStepOver = 0;//0:代表此步正在执行；1:代表在此步执行结束
int8_t IsExecute = -1;//1:执行下一步,0：不执行下一步
/*
*****************************************************************************************************************
*                                    void SmallCarHalfAutoMode(void)
*
*Description : 半自动任务执行函数
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void SmallCarHalfAutoMode(void)
{
	if(0==HalfAutoStep)//行车上电
 	{
		if(0==SingleStepOver)
		{
			PowerOn();
			if(RelayOnflag==-2)
			{
				#if RESTEP_1==0  //直接进行下一步
					HalfAutoStep++;
				#elif RESTEP_1==1 //表示此步执行结束
					SingleStepOver=1;
				#endif
				RelayOnflag=-1;//标志位复位			
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//经上位机确定执行下一步
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}	
		}
		Up_Data.Status = (Up_Data.Status&0xF0)|(0xF1);
	}
	else if(1==HalfAutoStep)        //确保小行车在初始状态
	{
		if(0==SingleStepOver)
		{
			BackToOriginState();		//确保小行车在初始状态
			if(UP_BIT == 1)         //上升结束
			{
				#if RESTEP_1==0       //直接进行下一步
					HalfAutoStep++; 
				#elif RESTEP_1==1     //表示此步执行结束
					SingleStepOver=1;
				#endif
				UP_BIT=0;//标志位复位
			}			
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//经上位机确定执行下一步
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}	
		}
	}
	else if(2==HalfAutoStep)//水平移动到抓料处正上方
	{
		if(0==SingleStepOver)
		{
			HorizontalMoving(target.x[0],target.y[0]);
			if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
			{
				#if RESTEP_1==0  //直接进行下一步
					HalfAutoStep++;
				  UpOrDown = 0;//0代表上半部，1代表下半部,为下面下降程序做准备
				#elif RESTEP_1==1 //表示此步执行结束
					SingleStepOver=1;
				#endif
				X_MOVE_BIT=0;//标志位复位
				Y_MOVE_BIT=0;//标志位复位				
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//经上位机确定执行下一步
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				UpOrDown = 0;//0代表上半部，1代表下半部,为下面下降程序做准备
				IsExecute=0;
			}			
		}
	}
	else if(3==HalfAutoStep)//爪子开始下降去抓料
	{
		if(0==SingleStepOver)
		{
			DowntoLitterPool(target.z[0]);
			if(1==DOWN_BIT)
			{
				#if RESTEP_1==0  //直接进行下一步
					HalfAutoStep++;
				  CloseFlag=1;//将下面执行的抓料标志位置1
				#elif RESTEP_1==1 //表示此步执行结束
					SingleStepOver=1;
				#endif
				DOWN_BIT = 0;//标志位复位
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//经上位机确定执行下一步
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				CloseFlag=1;//将下面执行的抓料标志位置1
				IsExecute=0;
			}				
		}
	}
	else if(4==HalfAutoStep)//执行抓料动作
	{
		if(0==SingleStepOver)
		{
			HFClosePaw();
			if(2==CloseFlag)
			{
				#if RESTEP_1==0  //直接进行下一步
					HalfAutoStep++;
				  UpOrDown = 1;  //0代表上半部，1代表下半部,将爪子上抬标志位置1
				#elif RESTEP_1==1 //表示此步执行结束
					SingleStepOver=1;
				#endif
				CloseFlag = 0; //标志位复位				
			}			
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//经上位机确定执行下一步
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				UpOrDown = 1;  //0代表上半部，1代表下半部,将爪子上抬标志位置1
				IsExecute=0;
			}		
		}
	}
	else if(5==HalfAutoStep)//执行小爪上升动作
	{
		if(0==SingleStepOver)
		{
			RisePawFromLitterPool();
			if(1==UP_BIT)
			{
				#if RESTEP_1==0  //直接进行下一步
					HalfAutoStep++;
				#elif RESTEP_1==1 //表示此步执行结束
					SingleStepOver=1;
				#endif
				UP_BIT = 0;//标志位复位
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//经上位机确定执行下一步
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}	
		}
	}
	else if(6==HalfAutoStep)//水平移动至投放位置坐标
	{
		if(0==SingleStepOver)
		{
			HorizontalMoving(target.x[1],target.y[1]);
			if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
			{
				#if RESTEP_1==0  //直接进行下一步
					HalfAutoStep++;
				#elif RESTEP_1==1 //表示此步执行结束
					SingleStepOver=1;
				#endif
				X_MOVE_BIT=0;  //标志位复位
				Y_MOVE_BIT=0;  //标志位复位
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//经上位机确定执行下一步
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
	else if(7==HalfAutoStep)//下放小爪子靠近垃圾池
	{
		if(0==SingleStepOver)
		{
			DownPawToLitterPool(target.z[1]);
			if(1==DOWN_BIT)
			{
				#if RESTEP_1==0  //直接进行下一步
					HalfAutoStep++;
				  OpenFlag=1;    //将松开爪子标志位置1
				#elif RESTEP_1==1 //表示此步执行结束
					SingleStepOver=1;
				#endif
				DOWN_BIT=0;    //标志位复位	
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//经上位机确定执行下一步
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				OpenFlag=1;    //将松开爪子标志位置1
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
	else if(7==HalfAutoStep)//松开爪子放料
	{
		if(0==SingleStepOver)
		{
			HFOpenPaw();
			if(2==OpenFlag)
			{
				#if RESTEP_1==0  //直接进行下一步
					HalfAutoStep++;
				#elif RESTEP_1==1 //表示此步执行结束
					SingleStepOver=1;
				#endif
				OpenFlag=0;    //标志位复位	
			}
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//经上位机确定执行下一步
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
	else if(8==HalfAutoStep)//提升爪子至安全高度
	{
		if(0==SingleStepOver)
		{
			UpPawFromLitterPool(ORIGIN_Z);
			if(1==UP_BIT)
			{
				#if RESTEP_1==0  //直接进行下一步
					HalfAutoStep++;
				#elif RESTEP_1==1 //表示此步执行结束
					SingleStepOver=1;
				#endif
				UP_BIT = 0;
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//经上位机确定执行下一步
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
	else if(9==HalfAutoStep)//水平移动到初始位置上方
	{
		if(0==SingleStepOver)
		{
			HorizontalMoving(ORIGIN_X,ORIGIN_Y);
			if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
			{
				#if RESTEP_1==0  //直接进行下一步
					HalfAutoStep++;
				#elif RESTEP_1==1 //表示此步执行结束
					SingleStepOver=1;
				#endif
				X_MOVE_BIT=0;//标志位复位
				Y_MOVE_BIT=0;//标志位复位
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//经上位机确定执行下一步
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
//	else if(10==HalfAutoStep)//将爪子下降到四楼平台
//	{
//		if(0==SingleStepOver)
//		{
//			DownToOrigin(ORIGIN_Z);
//			if(1==DOWN_BIT)
//			{
//				#if RESTEP_1==0  //直接进行下一步
//					HalfAutoStep++;
//				#elif RESTEP_1==1 //表示此步执行结束
//					SingleStepOver=1;
//				#endif
//				DOWN_BIT=0;//标志位复位
//			}		
//		}
//		else if(1==SingleStepOver)
//		{
//			ConfirmNextStep(HalfAutoStep+1);
//			if(1==IsExecute)//经上位机确定执行下一步
//			{
//				Up_Data.HalfStep = 0;
//				HalfAutoStep++;
//				SingleStepOver=0;
//				IsExecute=0;
//			}
//		}
//	}
	else if(10==HalfAutoStep)//断开电源,运行结束
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
			ErrorSmallCar = HalfAutoStep;//记录出错在哪一步
			Up_Data.Status =  Up_Data.Status&0x7F;
			RelayOffflag=-1;
			HalfAutoStep=0;
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
*                                    void ConfirmNextStep(uint8_t step)
*
*Description : 用于在每一步中间请求确认是否执行下一步
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ConfirmNextStep(uint8_t step)
{
	Up_Data.HalfStep = step;
}
