#include "task.h"
#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "relay.h"

 /*
 *************************************************************************
 *                       全局变量
 *************************************************************************  
 */
 
 

extern POSITION origin;//起始位置
extern POSITION target;//目标位置 

extern uint8_t WaitFlag;





void BigCarTask(void)
{
	BigCarRunning();
}
void BigCarRunning(void)
{
	/*big car starts running!*/
	RelayOn();//打开遥控器

  RisePawFromBurnPool();
	target.x[0] = (GARBAGE_X-target.x[0]-BIG_CAR_X_OFFSET)+ADD_X;
	HorizontalMoving(target.x[0],target.y[0]);//水平移动爪子至垃圾池
	VerticalMoving(target.z[0]);//爪子下降
	ClosePaw();
	ResetFlagBit(); 
	
	RisePawFromLitterPool();//向上提升爪子
	HorizontalMoving(BURN_POOL_X,BURN_POOL_Y);//将爪子移至焚烧池正上方
	LowerClawtoBurnPool();//下降爪子至焚烧池
	ReleasePaw(); 
	ResetFlagBit(); 
	
	RisePawFromBurnPool();//从焚烧池向上提爪子
	HorizontalMoving(origin.x[0],origin.y[0]);//水平移动之初始位置
	VerticalMoveOrigin(origin.z[0]);//下降至四楼平台
	RelayOff();//关闭遥控器 
}
//水平移动
void HorizontalMoving(float x,float y)
{
	if((0!=HTaskModeFlag)&&(0==BigCarDataCorrect))//数据不正常
	{
		DataCommunicateManage(HTaskModeFlag,1);//请求数据
	}
	else if(1==BigCarDataCorrect)//数据正常
	{
		if(X_MOVE_BIT == 0)
		{
			//大行车X方向移动
			XMoving(x);			
		}
		else if(Y_MOVE_BIT == 0)
		{
			//大行车Y方向移动
			YMoving(y);		
		}
		else
		{
			RequestStop(BIG_CAR); //请求大车433停止发送数据	
		}
	}
}
//竖直移动去抓料
void VerticalMoving(float z)
{
	if((0!=HTaskModeFlag)&&(0==BigClawDataCorrect))//数据不正常
	{
		DataCommunicateManage(HTaskModeFlag,1);//请求数据
		UpOrDown = 0;//0代表上半部，1代表下半部
	}
	else if(1==BigClawDataCorrect)//数据正常
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToLitterPool(z);		
		}
		else if(DOWN_BIT == 1)
		{
			RequestStop(BIG_CLAW);//请求大爪433停止发送数据
		}
	}
}
//竖直降落到初始位置
void VerticalMoveOrigin(float z)
{
	if((0!=HTaskModeFlag)&&(0==BigClawDataCorrect))//数据不正常
	{
		DataCommunicateManage(HTaskModeFlag,1);//请求数据
		laser.last_dis1 = laser.dis1; //先给laser.last_dis1初始化一个值
	}
	else if(1==BigClawDataCorrect)//数据正常
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToPlatform(z);		
			
		}
		else if(DOWN_BIT == 1)
		{
			RequestStop(BIG_CLAW);//请求大爪433停止发送数据
		}
	}
}

//从焚烧池抬升爪子
void RisePawFromBurnPool(void)
{
	if((0!=HTaskModeFlag)&&(0==BigClawDataCorrect))//数据不正常
	{
		DataCommunicateManage(HTaskModeFlag,1);//请求数据
		laser.last_dis8 = laser.dis8; //先给laser.last_dis8初始化一个值

	}
	else if(1==BigClawDataCorrect)//数据正常
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromBurnPool(BURN_POOL_UZ);		
		}
		else if(DOWN_BIT == 1)
		{
			RequestStop(BIG_CLAW);//请求大爪433停止发送数据
		}
	}		
	/*
	float err = 0.0f;
  uint8_t count=0;
  uint8_t stop_count=0;	
	
	laser.last_dis5 = laser.dis5;
	RequestStart(BIG_CAR);//请求大车433发送数据
	err=laser.last_dis5 - laser.dis5;	
	
	while(err == 0.0f)
	{
		laser.last_dis5 = laser.dis5;
		RequestStart(BIG_CAR);//请求大车433发送数据
		delay_ms(100);
		err=laser.last_dis5 - laser.dis5;		
	}
	
	if (laser.dis5<2000)//消除误检
	{
			laser.dis5 = 7000;
	}	
	
	err = laser.dis5 - BIG_CLAW_UP_STOP;
	
	if(abs(err)>200)//绝对值大于200
	{
		Big_Claw_Up_Delay_Pool_Flag=1;
		PAW_UP(ON);//爪子开始上升
	}
	
	while(abs(err)>200)
	{
		if (laser.dis5<2000)//消除误检
		{
			laser.dis5 = 7000;
		}
		err = laser.dis5 - BIG_CLAW_UP_STOP;		
		
		if (abs(err)<200)//滤除误检
		{
				count = count+1;
				err = 500;
				if (count>10)
				{
						count = 0;
						Big_Claw_Up_Delay_Pool=0;
						Big_Claw_Up_Delay_Pool_Flag=0;
						break;
				}
		}
		else
		{
				count = 0;
		}

		if (laser.dis5<5900)//判断限位使爪子停住
		{
				if (abs(laser.dis5-laser.last_dis5)<100)
				{
						stop_count=stop_count+1;
						if (stop_count>10)
						{
								stop_count=0;
								Big_Claw_Up_Delay_Pool=0;
								Big_Claw_Up_Delay_Pool_Flag=0;
								break;
						}						
				}
				else
				{
						stop_count=0;
				}
				
				laser.last_dis5 = laser.dis5;
		}
		if (Big_Claw_Up_Delay_Pool>10)//时间到了也会退出
		{
				Big_Claw_Up_Delay_Pool=0;
				Big_Claw_Up_Delay_Pool_Flag=0;
				break;
		}
	}
	PAW_UP(OFF);//爪子停止上升
	
	while ((abs(mpu.angle_x)>10)||(abs(mpu.angle_y)>10))//等待mpu6050数值稳定
	{
					
	}	
	
	RequestStop(BIG_CAR);//请求大车433停止发送数据
	laser.dis5 = 0.0f;
	*/
}
//从五楼平台抬升爪子
void RisePawFromPlatform(void)
{
	if((0!=HTaskModeFlag)&&(0==BigClawDataCorrect))//数据不正常
	{
		DataCommunicateManage(HTaskModeFlag,1);//请求数据
		laser.last_dis1 = laser.dis1; //先给laser.last_dis1初始化一个值

	}
	else if(1==BigClawDataCorrect)//数据正常
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromPlatform(PLATFORM_UZ);		
		}
		else if(UP_BIT == 1)
		{
			RequestStop(BIG_CLAW);//请求大爪433停止发送数据
		}
	}
}
//从垃圾池抬升爪子
void RisePawFromLitterPool(void)
{
	if((0!=HTaskModeFlag)&&(0==BigClawDataCorrect))//数据不正常
	{
		DataCommunicateManage(HTaskModeFlag,1);//请求数据
		UpOrDown = 1;//0代表上半部，1代表下半部
	}
	else if(1==BigClawDataCorrect)//数据正常
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromLitterPool(BURN_POOL_UZ);		
		}
		else if(UP_BIT == 1)
		{
			RequestStop(BIG_CLAW);//请求大爪433停止发送数据
		}
	}	
	/*
	float err = 0.0;
	uint8_t count=0;
	uint8_t stop_count=0;	
	
	laser.last_dis5 = laser.dis5;
	RequestStart(BIG_CAR);//请求大车433发送数据
	err=laser.last_dis5 - laser.dis5;	

	while(err == 0.0f)
	{
		laser.last_dis5 = laser.dis5;
		RequestStart(BIG_CAR);//请求大车433发送数据
		delay_ms(100);
		err=laser.last_dis5 - laser.dis5;		
	}
	
	if (laser.dis5<2000)//消除误检
	{
		laser.dis5 = 7000;
	}	
	
	err = laser.dis5 - BIG_CLAW_UP_STOP;	
	if(abs(err)>200)
	{
		Big_Claw_Up_Delay_Flag=1;
		PAW_UP(ON);
	}
	while (abs(err)>200)
	{
		//printf("up:NowDis=%f,err=%f\r\n",laser.dis5,err);
		delay_ms(100); 
		if (laser.dis5<2000)
		{
			laser.dis5 = 7000;
		}
		err = laser.dis5 - BIG_CLAW_UP_STOP;		
		
		if (abs(err<200))//滤除误差
		{
			count = count+1;
			err = 500;
			if (count>10)
			{
				count = 0;
				Big_Claw_Up_Delay = 0;
				Big_Claw_Up_Delay_Flag = 0;
				break;
			}
		}
		else
		{
			count = 0;
		}		
		if (((laser.dis5<5900)&&(laser.dis5>5500)))//判断限位使爪子停住
		{
				if (abs(laser.dis5-laser.last_dis5)<100)
				{
						stop_count=stop_count+1;
						if (stop_count>10)
						{
								stop_count=0;
								Big_Claw_Up_Delay = 0;
								Big_Claw_Up_Delay_Flag = 0;
								break;
						}	
				}
				else
				{
						stop_count=0;
				}
				
				laser.last_dis5 = laser.dis5;
		}	
		if (Big_Claw_Up_Delay>70)//延时
		{
				Big_Claw_Up_Delay = 0;
				Big_Claw_Up_Delay_Flag = 0;
				break;
		}		
	}
	PAW_UP(OFF);
	//爪子上升结束
	RequestStop(BIG_CAR);//请求大车433停止发送数据
	laser.dis5 = 0.0f;
	*/
}
//下放爪子至焚料池
void LowerClawtoBurnPool(void)
{
	if((0!=HTaskModeFlag)&&(0==BigClawDataCorrect))//数据不正常
	{
		DataCommunicateManage(HTaskModeFlag,1);//请求数据
		laser.last_dis8 = laser.dis8; //先给laser.last_dis8初始化一个值

	}
	else if(1==BigClawDataCorrect)//数据正常
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToLitterPool(BURN_POOL_DZ);		
		}
		else if(DOWN_BIT == 1)
		{
			RequestStop(BIG_CLAW);//请求大爪433停止发送数据
		}
	}	
/*
	float err2 = 0.0,err3 = 0.0,err4 = 0.0;
	uint8_t count = 0;	
	
	RequestStart(BURN_POOL);//给焚烧池料坑上433发送启动发送数据指令

	while ((err2==0)&&(err3==0)&&(err4==0))//判断数据是否正常
	{
		laser.last_dis2 = laser.dis2;
		laser.last_dis3 = laser.dis3;
		laser.last_dis4 = laser.dis4;
		count++;
		if (count>5)
		{
				for (uint8_t i = 0; i < 3; i++)
				{
						RequestStop(BIG_CAR);
						RequestStart(BURN_POOL);
				}
				count=0;
		}
		delay_ms(100);
		err2 = laser.last_dis2 - laser.dis2;
		err3 = laser.last_dis3 - laser.dis3;
		err4 = laser.last_dis4 - laser.dis4;
		//printf("down:Dis2=%f,err2=%f,Dis3=%f,err3=%f,Dis4=%f,err4=%f\r\n",laser.dis2,err2,laser.dis3,err3,laser.dis4,err4);
	}	
	
	if((abs(err2)>500)&&(abs(err3)>500)&&(abs(err3)>500))
	{
			PAW_DOWN(ON);
			//爪子正在下降
	}
	while ((abs(err2)>500)&&(abs(err3)>500)&&(abs(err3)>500))
	{
			//printf("down:Dis2=%f,err2=%f,Dis3=%f,err3=%f,Dis4=%f,err4=%f\r\n",laser.dis2,err2,laser.dis3,err3,laser.dis4,err4);
			delay_ms(100); 
			err2 = laser.dis2 - BIG_CLAW_DOWN_STOP2;
			err3 = laser.dis3 - BIG_CLAW_DOWN_STOP3;
			err4 = laser.dis4 - BIG_CLAW_DOWN_STOP4;
			if (((laser.dis2>0)&&(laser.dis2<4500))||((laser.dis3>0)&&(laser.dis3<3500))||((laser.dis4>0)&&(laser.dis4<4500)))
			{
					break;
			}
			
	}	
	PAW_DOWN(OFF);
	//爪子下降结束
	RequestStop(BURN_POOL);
*/
}
