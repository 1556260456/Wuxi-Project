 /*
 *************************************************************************
 * LCFunc.c  
 * 主要关于行车最底层的每一个环节的直接控制，如果要进行
 * 组合控制，只需要将这里的函数进行组合
 *************************************************************************  
 */
 
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

 //0:没有运行，1：正常运行结束，2：数据出的故障不得已结束
uint8_t X_MOVE_BIT=0;
uint8_t Y_MOVE_BIT=0;
uint8_t DOWN_BIT=0;
uint8_t UP_BIT=0;

//小爪上升固定时间与距离
uint8_t down_time = 0;
uint8_t down_distance = 0;
uint8_t up_time = 0;
uint8_t up_distance = 0;
//下降与上升计时开始标志位
uint8_t start_up = 0;
uint8_t start_down = 0;


//0:数据不正常，1:数据正常
uint8_t SmallCarDataCorrect = 0; 
uint8_t SmallClawDataCorrect = 0;

//小行车反向停止标志位
uint8_t ReverseStop = 0;
//反向时间
int ReverseTime = 0; 
//反向操作次数
uint8_t ReverseCount = 0;

//小行车点动标志位
uint8_t PointMove = 0;
//点动时间
int PointMoveTime = 0; 

//小爪子在料坑上下时用到
uint8_t UpOrDown = 0;//0代表不准上升，1代表可以上升

//1：表示合拢
uint8_t CloseFlag = 0;
//合拢延时
int     CloseDelay = 0;
//1:表示打开
uint8_t OpenFlag = 0;
//打开延时
int     OpenDelay = 0;
//出错状态
uint8_t ErrorSmallCar = 0;

int64_t Small_Claw_Up_Delay = 0;//延时时间 S
uint8_t Small_Claw_Up_Delay_Flag = 0;//打开定时器标志位

int64_t Small_Claw_Up_Delay_Pool = 0;
uint8_t Small_Claw_Up_Delay_Pool_Flag = 0;

Provar BCVar;
/*
*****************************************************************************************************************
*                                     void ClosePaw(void) 
*
*Description : 合紧爪子
*Arguments   : none
*Returns     : none
*Notes       : 使用此函数前CloseFlag置1，此功能结束后会将CloseFlag和CloseDelay置零
*****************************************************************************************************************
*/
void ClosePaw(void)
{
	Up_Data.Status = (Up_Data.Status&0x87)|0x30;//此时的状态值 close
	if(1 == CloseFlag)
	{
		if(CloseDelay<100)
		{
			PAW_CLOSE(ON);
		}
		else
		{
			PAW_CLOSE(OFF);	
			CloseFlag = 2;
			CloseDelay = 0;
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ReleasePaw(void)
*
*Description : 松开爪子
*Arguments   : none
*Returns     : none
*Notes       : 使用此函数前OpenFlag置1，此功能结束后会将OpenFlag和OpenDelay置零
*****************************************************************************************************************
*/
void ReleasePaw(void)
{
	Up_Data.Status = (Up_Data.Status&0x87)|0x38;//此时的状态值  release
	if(1 == OpenFlag)
	{
		if(OpenDelay<100)
		{
			PAW_RELEASE(ON);
		}
		else
		{
			PAW_RELEASE(OFF);	
			OpenFlag = 2;
			OpenDelay = 0;
		}
	}
}
/*
*****************************************************************************************************************
*                                     void XMoving(float x)
*
*Description : X方向运动
*Arguments   : float x：目标位置x坐标
*Returns     : none
*Notes       : X方向移动使用激光laser.dis2
*****************************************************************************************************************
*/
void XMoving(float x)
{
	float err_x = 0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis2<0)//滤除偶尔出现的错误值
	{
		if(same_dis_count<10)
		{
			same_dis_count++;
			laser.dis2=laser.last_dis2;
		}
		else//一直出现负值，停止运动出问题了
		{
			CAR_NORTH(OFF);
			CAR_SOUTH(OFF);
			same_dis_count=0;
			X_MOVE_BIT = 2;			
		}
	}		
	else
	{
		same_dis_count=0;
	}
	err_x = laser.dis2-x;
	
	if(abs(err_x)>2000)//偏差大于一定范围时，要移动	
	{
		
		err_x = laser.dis2 - x;    //24米
		//Now is moving to the X destination
		if(err_x<0)//小行车向南移动
		{
			Up_Data.Status = (Up_Data.Status&0x87)|0x08;//此时的状态值 x-
			//小行车开始向南移动
			CAR_SOUTH(ON);
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);  		
		}
		else if(err_x>0)//小行车开始向北运动
		{
			Up_Data.Status = (Up_Data.Status&0x87)|0x00;//此时的状态值 x+
			//小行车开始向北运动
			CAR_NORTH(ON);		
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);	
		}		
	}	
	//小行车X方向开始点动
	else if(abs(err_x)<2000)//小行车X方向点动模式
	{		
		
		if(err_x<0)//小行车向南点动
		{
			//小行车反向停止
			if((err_x<(-200))&&(ReverseCount<2))
			{
				RevStop(err_x);			
			}
			else if((err_x<(-200))&&(ReverseCount>=2))//小行车向南点动
			{
				HorizontalDotMove(err_x,0.0f);
			}
		  else/*X方向移动结束*/
			{
				CAR_SOUTH(OFF);
				
				ReverseCount = 0;
				PointMove = 0;
				PointMoveTime = 0;
				X_MOVE_BIT = 1;
			}
		}
		else if(err_x>=0)//小行车向北点动
		{
			//小行车反向制动			
			if((err_x>200)&&(ReverseCount<2))
			{
				RevStop(err_x);
			}			
			else if((err_x>200)&&((ReverseCount)>=2))//小行车向北点动
			{
				HorizontalDotMove(err_x,0.0f);
			}
			else/*X方向移动结束*/
			{
				CAR_NORTH(OFF);					
				
				ReverseCount = 0;
				PointMove = 0;
				PointMoveTime = 0;
				X_MOVE_BIT = 1;
			}
			
		}
	}
}
/*
*****************************************************************************************************************
*                                     void YMoving(float y)
*
*Description : Y方向运动
*Arguments   : float y：目标位置y坐标
*Returns     : none
*Notes       : Y方向移动使用激光laser.dis3
*****************************************************************************************************************
*/
void YMoving(float y)
{
	float err_y = 0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis3<0)//滤除偶尔出现的错误值
	{
		if(same_dis_count<10)
		{
			same_dis_count++;
			laser.dis3=laser.last_dis3;
		}
		else//一直出现负值，停止运动出问题了
		{
			CAR_WEST(OFF);
			CAR_EAST(OFF);
			same_dis_count=0;
			Y_MOVE_BIT = 2;			
		}
	}		
	else
	{
		same_dis_count=0;
	}
	err_y = laser.dis3-y;
	laser.last_dis3 = laser.dis3;   	
	
	
	//小行车Y方向移动
	if(abs(err_y)>2000)//小行车Y方向移动
	{	
		//Now is moving to the Y destnation
		if ((err_y>0))//小行车向东运动
		{
			Up_Data.Status = (Up_Data.Status&0x87)|0x10;//此时的状态值 y+
			CAR_EAST(ON); 
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
		}
		else if ((err_y<0))//小行车向西运动
		{
			Up_Data.Status = (Up_Data.Status&0x87)|0x18;//此时的状态值 y-
			CAR_WEST(ON);
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
		}	
	}
	//小行车Y方向点动
	else if ((abs(err_y)<2000))//小行车Y方向点动
	{	
		if(err_y<0)//小行车向西点动
		{
			if(err_y<(-200))
			{
				HorizontalDotMove(0,err_y);
			}
			else//Y方向移动结束
			{
				CAR_WEST(OFF);
				
				PointMove = 0;
				PointMoveTime = 0;	
				Y_MOVE_BIT = 1;				
			}
		}
		else if(err_y>0)//小行车向东点动
		{
			if(err_y>200)
			{
				HorizontalDotMove(0,err_y);
			}
			else//Y方向移动结束
			{
				CAR_EAST(OFF);	
				
				PointMove = 0;
				PointMoveTime = 0;	
				Y_MOVE_BIT = 1;							
			}
		}
	}	
}

/*
*****************************************************************************************************************
*                                     void UpPawFromLitterPool(float z)
*
*Description : 爪子上升程序（从料坑池上升）  （尚需修改）
*Arguments   : float z：激光距离垃圾表面的高度(?)
*Returns     : none
*Notes       : 往下射的激光laser.dis1,依据down_time,down_distance 来确定上升高度
*****************************************************************************************************************
*/
void UpPawFromLitterPool(float z)
{
	float paw_err=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;

	if (laser.dis1<0)//滤除偶尔出现的错误值
		{
			laser.dis1=laser.last_dis1;
		}
	paw_err = laser.dis1 - z;   
	laser.last_dis1 = laser.dis1;   
	
	Up_Data.Status = (Up_Data.Status&0x87)|0x20;//此时的状态值 up
	if(laser.dis1>0 && laser.dis1<10000)
	{
		paw_err = abs(laser.dis1 - z);
		if(paw_err>=100)
		{
			PAW_DOWN(ON);
			start_up = 1;
		}		
	}
	if(up_time>=down_time)   //当小爪上升时间与下降时间成某一比例时，停止
	{
		PAW_DOWN(OFF);
		start_up = 0;
		down_time =0;
		up_time = 0;
		UP_BIT =1;
	}
	up_distance = laser.dis1;
	if(up_distance>=down_distance)  //当小爪上升时高度与上一次下降前高度相等，停止
	{
		PAW_DOWN(OFF);
		start_up = 0;
		down_time =0;
		up_time = 0;
		UP_BIT = 1;
	}
	else
	{
		same_dis_count=0;
	}	
}

/*
*****************************************************************************************************************
*                                     void DownPawToLitterPool(float z)
*
*Description : 爪子下降程序（下降到料坑抓料）
*Arguments   : float z：激光距地面垃圾的高度
*Returns     : none
*Notes       : 一直使用小爪向下的激光，记录下降前的高度值以及下降时间
*****************************************************************************************************************
*/
void DownPawToLitterPool(float z)
{
	/*
		此处使用小爪子向下激光
	*/
	float paw_err=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis1<0)//滤除偶尔出现的错误值
	{
		laser.dis1=laser.last_dis1;
	}				
	paw_err = laser.dis1 - z;   
	laser.last_dis1 = laser.dis1;   
		
	//小爪下降
	if(abs(paw_err)>1000)//小爪下降
	{
		if(paw_err>=0)//小爪下降
		{
			PAW_DOWN(ON);	
			down_distance = laser.dis1;
			start_down = 1;
		}
	}
	//小爪下降到指定高度，准备点动
	else if(abs(paw_err)<1000)//大爪下降到指定高度，准备点动  (是否下面两种情况应该在点动程序段内)
	{
		/*情况1：正常情况运行*/
		if(paw_err>200)//开始点动
		{
			VerticalDotMove(paw_err);
		}
		else//点动结束
		{
			PAW_DOWN(OFF);	
			start_down = 0;
			PointMove = 0;
			PointMoveTime = 0;	
			DOWN_BIT = 1;
		}
		/*情况2：在点动时爪子倾斜过大*/
		if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>10.0f))//爪子倾斜超过一定角度，停止下降，跳出循环
		{
			PAW_DOWN(OFF);
			start_down = 0;
			PointMove = 0;
			PointMoveTime = 0;	
			DOWN_BIT = 1;
		} 		
		/*情况3：爪子无法下降，但是绳索仍然下降*/
		if (abs(paw_err-paw_err_last)<100)//处理已经下降到底部，但还在下降的情况
		{
			same_dis_count = same_dis_count+1;
			
			if (same_dis_count>5)
			{
				PAW_DOWN(OFF);
				start_down = 0;
				same_dis_count=0;
				
				PointMove = 0;
				PointMoveTime = 0;
				same_dis_count=0;				
				DOWN_BIT = 1;
			}	
		}
		else
		{
			same_dis_count=0;
		}
		paw_err_last=paw_err;				
	}		
}

//小行车自检状态
void SelfCheckStatus(void)
{
//	uint8_t count1=0;
//	uint8_t count2=0;
//	
//  RelayOn();//打开遥控器
//	RequestStart(BIG_CAR);//请求大车433发送数据
//	
//	while ((laser.dis6<=0)||(laser.dis7<=0))//判断大行车数据是否正常
//	{
//		RequestStart(BIG_CAR); //请求大车433发送数据	
//		
//		count1++;
//		if(count1>10)//请求次数大于10次还没有得到正确数据，可能是数据传输链路有问题，需要检查
//		{
//			break;
//		}
//	}
//	if(count1<=10)//数据正常
//	{
//		/*
//			数据正常，上传数据，此段函数需要补充
//		*/
//		
//		//判断行车位置
//		if((abs(laser.dis6-ORIGIN_X)<200)&&(abs(laser.dis7-ORIGIN_Y)<200))
//		{
//			//(x,y)坐标在初始位置，继续判断z轴数据是否在初始位置附近
//			RequestStop(BIG_CAR); //请求大车433停止发送数据	
//			RequestStart(BIG_CLAW); //请求大爪433发送数据	
//			
//			while (laser.dis1<=0)//判断大爪上传数据是否正常
//			{
//				count2++;
//				if(count2>10)//请求次数大于10次还没有得到正确数据，可能是数据传输链路有问题，需要检查
//				{
//					break;
//				}
//				for (uint8_t i = 0; i < 5; i++)
//				{
//					RequestStop(BIG_CAR); //请求大车433停止发送数据	
//					RequestStart(BIG_CLAW); //请求大抓433发送数据	
//				}			 
//			}		
//			if(abs(laser.dis1-ORIGIN_Z)<200)
//			{
//				/*不需要动*/
//				RequestStop(BIG_CLAW); //请求大爪433停止发送数据	
//			}
//			else
//			{
//				VerticalMoveOrigin(ORIGIN_Z);//移动到初始位置
//			}
//		}
//		else
//		{
//			//继续判断大爪子在在垃圾池上方还是平台上方
//			if(laser.dis6<ADD_X)//在平台上方
//			{		
//				RisePawFromBurnPool();
//				HorizontalMoving(ORIGIN_X,ORIGIN_Y);
//				VerticalMoveOrigin(ORIGIN_Z);
//			}
//			else//在垃圾池上方
//			{
//				RisePawFromLitterPool();
//				HorizontalMoving(ORIGIN_X,ORIGIN_Y);
//				VerticalMoveOrigin(ORIGIN_Z);				
//			}			
//		}
//	}
//	ResetFlagBit();
//	RelayOff();//关闭遥控器 
//	
}
/*
*****************************************************************************************************************
*                                     DataCommunicateManage(uint8_t task_mode,uint8_t OnorOff)
*
*Description : 数据通讯管理，用于打开某一路上传激光数据
*Arguments   : uint8_t task_mode：用于选择不同设备上的激光 OnorOff:1代表on,0代表off
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
//数据通讯管理
void DataCommunicateManage(uint8_t task_mode,uint8_t OnorOff)
{
	if(SMALL_CAR==task_mode)//请求小行车433
	{	
		if(1==OnorOff)
		{
			if ((laser.dis2<=0)||(laser.dis3<=0))//判断小行车数据是否正常
			{
				SmallCarDataCorrect = 0;
				RequestStart(SMALL_CAR);  //请求大车433发送数据	
			}	
			else
				SmallCarDataCorrect = 1;	
		}
		else if(0==OnorOff)
		{
			RequestStop(SMALL_CAR);
		}
	}
	else if(SMALL_CLAW==task_mode)//请求大爪433
	{
		if(1==OnorOff)
		{
			if (laser.dis1<=0)//判断小爪子数据是否正常
			{
				SmallClawDataCorrect = 0;
				RequestStart(SMALL_CLAW);  //请求小爪433发送数据	
			}	
			else
				SmallClawDataCorrect = 1;			
		}
		else if(0==OnorOff)
		{
			RequestStop(SMALL_CLAW);
		}
	}
}
/*
*****************************************************************************************************************
*                                    void RevStop(float err)
*
*Description : 反向停止
*Arguments   : float err：实际位置距目标位置偏差
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void RevStop(float err)
{
	if(err>0)
	{
		//小行车向南反向制动	
		if(ReverseStop == 0)
		{
			CAR_NORTH(OFF);
			ReverseStop=1;	       				
		}
		else if(ReverseStop==1)
		{
			if(ReverseTime<=10)//1s开
			{				
				CAR_SOUTH(ON);				
			}
			else
			{
				ReverseStop++;
				ReverseTime = 0;
			}	
		}
		else if(ReverseStop==2)
		{
			if(ReverseTime<=2)//0.2s停
			{				
				CAR_SOUTH(OFF);				
			}
			else
			{
				ReverseStop--;
				ReverseTime = 0;
				ReverseCount++;
			}					
		}					
	}
	//小行车向北反向制动
	else
	{
		//小行车反向停止
		if(ReverseStop == 0)
		{
			CAR_SOUTH(OFF);
			ReverseStop=1;	       				
		}
		else if(ReverseStop==1)
		{
			if(ReverseTime<=10)//1s开
			{				
				CAR_NORTH(ON);				
			}
			else
			{
				ReverseStop++;
				ReverseTime = 0;
			}	
		}
		else if(ReverseStop==2)
		{
			if(ReverseTime<=2)//0.2s停
			{				
				CAR_NORTH(OFF);				
			}
			else
			{
				ReverseStop--;
				ReverseTime = 0;
				ReverseCount++;
			}					
		}			
	}
}
/*
*****************************************************************************************************************
*                                    void HorizontalDotMove(float err_x,float err_y)
*
*Description : 小行车点动
*Arguments   : float err_x,float err_y：传入位置偏差，不用的那一个轴传0
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void HorizontalDotMove(float err_x,float err_y)
{
	if(PointMove == 0)
	{
		ReverseStop = 0;
		ReverseTime = 0;
		PointMove = 1;
	}
	else if(PointMove == 1)
	{
		if(PointMoveTime<10)
		{
			if(err_x>0)	CAR_NORTH(ON);
			else if(err_x<0) CAR_SOUTH(ON);
			else if(err_y>0) CAR_EAST(ON);
			else if(err_y<0) {CAR_WEST(ON);}				
		}
		else
		{
			PointMove++;
			PointMoveTime=0;
		}
	}
	else if(PointMove == 2)
	{
		if(PointMoveTime<10)
		{
			if(err_x>0)	{CAR_NORTH(OFF);}
			else if(err_x<0) {CAR_SOUTH(OFF);}
			else if(err_y>0) {CAR_EAST(OFF);}
			else if(err_y<0) {CAR_WEST(OFF);}
		}
		else
		{
			PointMove--;
			PointMoveTime=0;
		}				
	}	
}

/*
*****************************************************************************************************************
*                                    void VerticalDotMove(float err_z)
*
*Description : 小爪子点动
*Arguments   : float err_z：传入位置偏差
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void VerticalDotMove(float err_z)
{
	if(PointMove == 0)
	{
		PointMove = 1;
	}
	else if(PointMove == 1)
	{
		if(PointMoveTime<10)
		{
			if(err_z>0)	
			{
				PAW_DOWN(ON);		
			}	
		}
		else
		{
			PointMove++;
			PointMoveTime=0;
		}
	}
	else if(PointMove == 2)
	{
		if(PointMoveTime<10)
		{
			if(err_z>0)	
			{
				PAW_DOWN(OFF);
			}
		}
		else
		{
			PointMove--;
			PointMoveTime=0;
		}				
	}	
}//复位标志位
/*
*****************************************************************************************************************
*                                    void ResetFlagBit(void)
*
*Description : 复位标志位
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ResetFlagBit(void)
{
	 X_MOVE_BIT=0;
	 Y_MOVE_BIT=0;
	 DOWN_BIT=0;
	 UP_BIT=0;	
}
/*
*****************************************************************************************************************
*                                    void RequestStop(uint8_t dev)
*
*Description : 请求小行车/小爪子停止发送指令
*Arguments   : dev:设备号
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void RequestStop(uint8_t dev)
 {
	switch(dev)
	{
		case SMALL_CAR:
					RequestStopToSmallCar();
					break;
		case SMALL_CLAW:
					RequestStopToSmallClaw();
					break;		
		default:
				break;
	}
}
/*
*****************************************************************************************************************
*                                    void RequestStart(uint8_t dev)
*
*Description : 请求大行车/大爪子开始发送指令
*Arguments   : dev:设备号
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void RequestStart(uint8_t dev)
{
	switch(dev)
	{
		case SMALL_CAR:
					RequestStartToSmallCar();
					break;
		case SMALL_CLAW:
				 	RequestStartToSmallClaw();	
					break;		
		default:
				  break;
	}
}
//暂停
void TaskSuspend(void)
{
	Up_Data.Status = (Up_Data.Status&0x87)|0x50;//此时的状态值 暂停
	
	BCVar._CloseFlag = CloseFlag;
	BCVar._OpenFlag = OpenFlag;
	BCVar._PointMove = PointMove;
	BCVar._ReverseStop = ReverseStop;
	BCVar._Run_Mode = Run_Mode;
	BCVar._WaitFlag = WaitFlag;
	
	PowerOff();//给遥控器断电
	
	CloseFlag = 0;
	OpenFlag = 0;
	PointMove = 0;
	ReverseStop = 0;
	Run_Mode = 0;
	WaitFlag = 0;
	//Up_Data.Status   //暂停
}
//恢复运行
void ConExecute(void)
{

	Up_Data.Status = (Up_Data.Status&0x87)|0x58;//此时的状态值 恢复
	CloseFlag = BCVar._CloseFlag;
	OpenFlag = BCVar._OpenFlag;
	PointMove = BCVar._PointMove;
	ReverseStop = BCVar._ReverseStop;
	Run_Mode = BCVar._Run_Mode;
	WaitFlag = BCVar._WaitFlag;
	
	PowerOn();//给遥控器上电

	BCVar._CloseFlag = 0;
	BCVar._OpenFlag = 0;
	BCVar._PointMove = 0;
	BCVar._ReverseStop = 0;
	BCVar._Run_Mode = 0;
	BCVar._WaitFlag = 0;

}

void SmallCarStop(void)//还需斟酌
{
	Up_Data.Status = (Up_Data.Status&0x87)|0x48;//此时的状态值 down
	PowerOff();//给遥控器断电
	
	CloseFlag = 0;
	OpenFlag = 0;
	PointMove = 0;
	ReverseStop = 0;
	Run_Mode = 0;
	WaitFlag = 0;	
}
