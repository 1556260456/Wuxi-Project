 /*
 *************************************************************************
 * LCFunc.c  
 * ��Ҫ�����г���ײ��ÿһ�����ڵ�ֱ�ӿ��ƣ����Ҫ����
 * ��Ͽ��ƣ�ֻ��Ҫ������ĺ����������
 *************************************************************************  
 */
 
#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "usart2.h"
#include "relay.h"

 /*
 *************************************************************************
 *                       ȫ�ֱ���
 *************************************************************************  
 */
extern uint8_t WaitFlag;
extern uint8_t Run_Mode;
extern uint8_t Origin_state;
 //0:û�����У�1���������н�����2�����ݳ��Ĺ��ϲ����ѽ���
uint8_t X_MOVE_BIT=0;
uint8_t Y_MOVE_BIT=0;
uint8_t DOWN_BIT=0;
uint8_t UP_BIT=0;


//Сצ�����̶�ʱ�������
int down_time = 1;
int down_distance = -1;
int up_time = 2;
int up_distance = -2;
//�½���������ʱ��ʼ��־λ
uint8_t start_up = 0;
uint8_t start_down = 0;


//0:���ݲ�������1:��������
uint8_t SmallCarDataCorrect = 0; 
uint8_t SmallClawDataCorrect = 0;

//С�г�����ֹͣ��־λ
uint8_t ReverseStop = 0;
//����ʱ��
int ReverseTime = 0; 

//С�г��㶯��־λ
uint8_t PointMove = 0;
//�㶯ʱ��
int PointMoveTime = 0; 

//1����ʾ��£
short int CloseFlag = -1;
//��£��ʱ
int     CloseDelay = 0;
//1:��ʾ��
short int OpenFlag = -1;
//����ʱ
int   OpenDelay = 0;
//����״̬
uint8_t ErrorSmallCar = 0;

int64_t Small_Claw_Up_Delay = 0;//��ʱʱ�� S
uint8_t Small_Claw_Up_Delay_Flag = 0;//�򿪶�ʱ����־λ

Provar BCVar;
/*
*****************************************************************************************************************
*                                     void ClosePaw(void) 
*
*Description : �Ͻ�צ��
*Arguments   : none
*Returns     : none
*Notes       : ʹ�ô˺���ǰCloseFlag��1���˹��ܽ�����ὫCloseFlag��CloseDelay����
*****************************************************************************************************************
*/
static void Close(void)
{
	Up_Data.Status = (Up_Data.Status&0x87)|0x38;//��ʱ��״ֵ̬  close
	if(CloseFlag>=0 && CloseFlag <= 50)
	{
		PAW_CLOSE(ON);
	}
	else if(CloseFlag>=50 && CloseFlag <=60)
	{		
		PAW_CLOSE(OFF);
	}	 
	else
	{
		CloseFlag = -2;
	}
}
void ClosePaw(void)
{
	if(CloseFlag==-1)
	{
		CloseFlag = 0;
	}
	if(CloseFlag>=0)
	{
		Close();
	}
}
/*
*****************************************************************************************************************
*                                     void ReleasePaw(void)
*
*Description : �ɿ�צ��
*Arguments   : none
*Returns     : none
*Notes       : ʹ�ô˺���ǰOpenFlag��1���˹��ܽ�����ὫOpenFlag��OpenDelay����
*****************************************************************************************************************
*/
static void Open(void)
{
	Up_Data.Status = (Up_Data.Status&0x87)|0x40;//��ʱ��״ֵ̬  release
	if(OpenFlag>=0 && OpenFlag <=50)
	{
		PAW_RELEASE(ON);
	}
	else if(OpenFlag>=50 && OpenFlag <=60)
	{		
		PAW_RELEASE(OFF);
	}	 
	else
	{
		OpenFlag = -2;
	}
}
void ReleasePaw(void)
{
	if(OpenFlag==-1)
	{
		OpenFlag = 0;
	}
	if(OpenFlag>=0)
	{
		Open();
	}
}
/*
*****************************************************************************************************************
*                                     void XMoving(float x)
*
*Description : X�����˶�
*Arguments   : float x��Ŀ��λ��x����
*Returns     : none
*Notes       : X�����ƶ�ʹ�ü���laser.dis2
*****************************************************************************************************************
*/
void XMoving(float x)
{
	float err_x = 0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis2<0)//�˳�ż�����ֵĴ���ֵ
	{
		if(same_dis_count<10)
		{
			same_dis_count++;
			laser.dis2=laser.last_dis2;
		}
		else//һֱ���ָ�ֵ��ֹͣ�˶���������
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
	laser.last_dis2=laser.dis2;
	if(abs(err_x)>=2000)//ƫ�����һ����Χʱ��Ҫ�ƶ�	
	{
		
		err_x = laser.dis2 - x;    //24��
		//Now is moving to the X destination
		if(err_x<0)//С�г������ƶ�
		{
			Up_Data.Status = (Up_Data.Status&0x87)|0x08;//��ʱ��״ֵ̬ x-
			//С�г���ʼ�����ƶ�
			CAR_SOUTH(ON);
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);  		
		}
		else if(err_x>0)//С�г���ʼ���˶�
		{
			Up_Data.Status = (Up_Data.Status&0x87)|0x00;//��ʱ��״ֵ̬ x+
			//С�г���ʼ���˶�
			CAR_NORTH(ON);		
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);	
		}		
	}	
	//С�г�X����ʼ�㶯
	else if(abs(err_x)<2000)//С�г�X����㶯ģʽ
	{		
		
		if(err_x<0)//С�г����ϵ㶯
		{
			if(err_x<(-200))//С�г����ϵ㶯
			{
				HorizontalDotMove(err_x,0.0f);
			}
		  else/*X�����ƶ�����*/
			{
				CAR_SOUTH(OFF);
				PointMove = 0;
				PointMoveTime = 0;
				X_MOVE_BIT = 1;
			}
		}
		else if(err_x>=0)//С�г��򱱵㶯
		{
			if(err_x>200)//С�г��򱱵㶯
			{
				HorizontalDotMove(err_x,0.0f);
			}
			else/*X�����ƶ�����*/
			{
				CAR_NORTH(OFF);					
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
*Description : Y�����˶�
*Arguments   : float y��Ŀ��λ��y����
*Returns     : none
*Notes       : Y�����ƶ�ʹ�ü���laser.dis3
*****************************************************************************************************************
*/
void YMoving(float y)
{
	float err_y = 0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis3<0)//�˳�ż�����ֵĴ���ֵ
	{
		if(same_dis_count<10)
		{
			same_dis_count++;
			laser.dis3=laser.last_dis3;
		}
		else//һֱ���ָ�ֵ��ֹͣ�˶���������
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
	
	
	//С�г�Y�����ƶ�
	if(abs(err_y)>2000)//С�г�Y�����ƶ�
	{	
		//Now is moving to the Y destnation
		if ((err_y>0))//С�г����˶�
		{
			Up_Data.Status = (Up_Data.Status&0x87)|0x10;//��ʱ��״ֵ̬ y+
			CAR_EAST(ON); 
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
		}
		else if ((err_y<0))//С�г������˶�
		{
			Up_Data.Status = (Up_Data.Status&0x87)|0x18;//��ʱ��״ֵ̬ y-
			CAR_WEST(ON);
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
		}	
	}
	//С�г�Y����㶯
	else if ((abs(err_y)<2000))//С�г�Y����㶯
	{	
		if(err_y<0)//С�г������㶯
		{
			if(err_y<(-200))
			{
				HorizontalDotMove(0,err_y);
			}
			else//Y�����ƶ�����
			{
				CAR_WEST(OFF);
				
				PointMove = 0;
				PointMoveTime = 0;	
				Y_MOVE_BIT = 1;				
			}
		}
		else if(err_y>0)//С�г��򶫵㶯
		{
			if(err_y>200)
			{
				HorizontalDotMove(0,err_y);
			}
			else//Y�����ƶ�����
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
*Description : צ����������--���Ͽ�����  
*Arguments   : float z���������ļ���λ��
*Returns     : none
*Notes       : ������ļ���laser.dis1,����down_time,down_distance ��ȷ�������߶�
*****************************************************************************************************************
*/

void UpPawFromLitterPool(float z)
{
	float paw_err = 0; 
	float uwb_dis_err = 0;
	static float paw_err_last = 0;
	static int same_dis_count = 0;
	static uint8_t dis1_err_count = 0;
	
	uwb_dis_err = mpu.dis - z;
	if(Run_Mode == 1)
	{
		if( uwb_dis_err>=300 && mpu.dis>2500 && mpu.dis<9000) //��ʾצ��λ�ñȽϵ�
		{
			PAW_UP(ON);
			Up_Data.Status = (Up_Data.Status&0x87)|0x28;//��ʱ��״ֵ̬ up
		}
		if(uwb_dis_err<300 || (mpu.dis>0 && mpu.dis<=2500))
		{
			PAW_UP(OFF);
			UP_BIT=1;
		}
		if(((mpu.angle_x>=25.0f) || mpu.angle_y>=25.0f))
		{
			PAW_UP(OFF);
			UP_BIT = 2;
		}
		if(mpu.dis<=0)
		{
			PAW_UP(OFF);
			UP_BIT=2;
		}
		
		
	}
	if(Run_Mode>1)
	{
		if(mpu.dis>2500 && mpu.dis<9000) //��ʾצ��λ�ñȽϵ�
		{
			PAW_UP(ON);
			start_up = 1;
			Up_Data.Status = (Up_Data.Status&0x87)|0x28;//��ʱ��״ֵ̬ up
		}
		if( mpu.dis<=2500 || uwb_dis_err <=300 )
		{
			PAW_UP(OFF);
			start_up = 0;
			UP_BIT=1;
		}
	 if(((mpu.angle_x>=25.0f) || mpu.angle_y>=25.0f))
		{
			PAW_UP(OFF);
			start_up = 0;
			UP_BIT = 2;
		}
		if(mpu.dis<0 || mpu.dis>=15000)        //����UWB���ݴ��󣬲�ʹ�ü�������
		{
			PAW_UP(OFF);
			start_up = 0;
			UP_BIT = 2;
		}
	}
}
//			if (laser.dis1<0)//�˳�ż�����ֵĴ���ֵ
//			{
//				if(dis1_err_count<10)
//				{
//					dis1_err_count++;
//					laser.dis1=laser.last_dis1;
//				}
//				else
//				{
//					PAW_UP(OFF);
//					dis1_err_count = 0;
//					UP_BIT = 2;
//				}
//			}
//			paw_err = z-laser.dis1;  
//			laser.last_dis1 = laser.dis1;   
//			if(laser.dis1>0 && laser.dis1<9000)    //צ������������ʵ�ʾ������������С��0.1��ʱֹͣ
//			{ 
//				if(abs(paw_err)>=300)
//				{
//					PAW_UP(ON);
//					Up_Data.Status = (Up_Data.Status&0x87)|0x28;//��ʱ��״ֵ̬ up
//					start_up = 1;
//				}
//				else 
//				{
//					PAW_UP(OFF);
//					UP_BIT=1;
//					start_up = 0;
//				}
//			}
//			if((mpu.angle_x>=25.0f) || mpu.angle_y>=25.0f)   //���3����צ����б�ǶȽϴ�ʱ��ֹͣ�˶�
//			{
//				PAW_UP(OFF);
//				start_up = 0;
//				down_time =1;
//				up_time = 2;
//				UP_BIT = 1;
//			}
//	
//			if(abs(paw_err-paw_err_last)<50)    ///���4��    �����Ѿ���������λ�����   �˴��ľ�����ۼƴ�����Ҫ΢��
//			{
//				same_dis_count = same_dis_count+1;
//				if (same_dis_count>100)
//				{
//					PAW_UP(OFF);
//					start_up = 0;
//					down_time =1;
//					up_time = 2;
//					same_dis_count=0;			
//					UP_BIT = 1;
//				}	
//			}
//			else
//			{
//				same_dis_count=0;
//			}	
//			paw_err_last=paw_err;	
//		}
//	}
//}
	
	
	
//		if( (Origin_state==0) && (Run_Mode>1) )  //  ���2�� ���ֶ�ģʽ�Ƿ���Ҫ�˹���
//	  {
//			if(up_time>=down_time)   //��Сצ����ʱ�����½�ʱ���ĳһ����ʱ��ֹͣ
//			{
//				PAW_UP(OFF);
//				start_up = 0;
//				down_time =1;
//				up_time = 2;
//				UP_BIT =1;
//			}
//			up_distance = laser.dis1;
//			if(up_distance>=down_distance)  //��Сצ����ʱ�߶�����һ���½�ǰ�߶���ȣ�ֹͣ
//			{
//				PAW_UP(OFF);
//				start_up = 0;
//				down_time =1;
//				up_time = 2;
//				UP_BIT = 1;
//			}
			

/*
*****************************************************************************************************************
*                                     void DownPawToLitterPool(float z)
*
*Description : צ���½������½����Ͽ�ץ�ϣ�
*Arguments   : float z���г���צ����uwb֮�����Ծ��룬���г�Ϊ������
*Returns     : none
*Notes       : ���ֶ�ģʽ������ģʽ���ֶ�ģʽֻ��uwb���ݣ�����ģʽ����uwb�����ü�������
*****************************************************************************************************************
*/
void DownPawToLitterPool(float z)
{
	/*
		�˴�ʹ��Сצ�����¼���
	*/
	
	float paw_err=0; 
	float uwb_dis_err = 0;
	static uint8_t  Dis1ErrCount=0;
	static float paw_err_last=0;
	static uint8_t same_dis_count=0;
	
	uwb_dis_err = z - mpu.dis;
	paw_err = laser.dis1 - (z-mpu.dis);
	if(Run_Mode == 1)   //�ֶ�ģʽ��ʹ��uwb���ݣ���������ʱ�½�
	{
		if(mpu.dis>0 && mpu.dis<9000 && uwb_dis_err>300)//ʹ��uwb���ݣ������ϴ󣬴�ʱ�½�
		{
			PAW_DOWN(ON);
			Up_Data.Status = (Up_Data.Status&0x87)|0x30;//��ʱ��״ֵ̬ down
		}
		else if(mpu.dis>0 && mpu.dis<9000 && uwb_dis_err<300)//ʹ��uwb���ݣ�������С����ʱֹͣ�½�
		{
			PAW_DOWN(OFF);
			DOWN_BIT = 1;			
		}
		else 
		{
			PAW_DOWN(OFF);
			DOWN_BIT = 2;		
		}
	}
	//�ڰ��Զ�����ȫ�Զ�����£�����uwb���ݣ����ü�������
	if(Run_Mode > 1)  //���ﻹ��Ҫ��֤uwb�����Ƿ�����
	{
		if(mpu.dis>0 && mpu.dis<9000 && uwb_dis_err>1000)//ʹ��uwb���ݣ������ϴ󣬴�ʱ�½�
		{
			PAW_DOWN(ON);
			start_down = 1;
			Up_Data.Status = (Up_Data.Status&0x87)|0x30;//��ʱ��״ֵ̬ down
		}
		else
		{
			PAW_DOWN(OFF);
			start_down = 0;
			PointMove = 0;
			PointMoveTime = 0;	
			DOWN_BIT = 1;
		}
//		else if(mpu.dis>0 && uwb_dis_err<=2000 )//ʹ��uwb���ݣ�������С����ʱʹ�ü������ݲ��ҵ㶯
//		{
//			if (laser.dis1<0)//�˳�ż�����ֵĴ���ֵ
//			{
//				if(Dis1ErrCount<100)
//				{
//					Dis1ErrCount++;
//					laser.dis1=laser.last_dis1;
//				}
//				else//һֱ���ָ�ֵ��ֹͣ�˶���������
//				{
//					PAW_DOWN(OFF);
//					Dis1ErrCount=0;
//					DOWN_BIT = 2;			
//				}
//			}				
//			paw_err =laser.dis1- (z - mpu.dis);   
//			laser.last_dis1 = laser.dis1;
//			if(abs(paw_err)>1000)//Сצ�½�
//			{
//				if(paw_err>=0)//Сצ�½�
//				{
//					PAW_DOWN(ON);
////					if(start_down==0)
////					{
////						down_distance = laser.dis1;
////					}
////					start_down = 1;
//				}
//			}
			//Сצ�½���ָ���߶ȣ�׼���㶯
//			else if(abs(paw_err)<1000)//��צ�½���ָ���߶ȣ�׼���㶯  (�Ƿ������������Ӧ���ڵ㶯�������)
//			{
//			
//				/*���1�������������*/
//				if(paw_err>200)//��ʼ�㶯
//				{
//					VerticalDotMove(paw_err);
//				}
//				else//�㶯����
//				{
//					PAW_DOWN(OFF);
//					//start_down = 0;
//					PointMove = 0;
//					PointMoveTime = 0;	
//					DOWN_BIT = 1;
//				}
//				/*���2���ڵ㶯ʱצ����б����*/
//				if ((abs(mpu.angle_x)>25.0f)||(abs(mpu.angle_y)>25.0f))//צ����б����һ���Ƕȣ�ֹͣ�½�������ѭ��
//				{
//					PAW_DOWN(OFF);
//					//start_down = 0;
//					PointMove = 0;
//					PointMoveTime = 0;	
//					DOWN_BIT = 1;
//				} 	
//				/*���3��צ���޷��½�������������Ȼ�½�*/
//				if (abs(paw_err-paw_err_last)<50)//�����Ѿ��½����ײ����������½������
//				{
//					same_dis_count = same_dis_count+1;
//					
//					if (same_dis_count>100)
//					{
//						PAW_DOWN(OFF);
//						//start_down = 0;
//						same_dis_count=0;
//						
//						PointMove = 0;
//						PointMoveTime = 0;
//						same_dis_count=0;				
//						DOWN_BIT = 1;
//					}	
//				}
//				else
//				{
//					same_dis_count=0;
//				}
//				paw_err_last=paw_err;	
//			}
//		}
//	}		
}
	}

//С�г��Լ�״̬
void SelfCheckStatus(void)
{
//	uint8_t count1=0;
//	uint8_t count2=0;
//	
//  RelayOn();//��ң����
//	RequestStart(Small_CAR);//����С��433��������
//	
//	while ((laser.dis6<=0)||(laser.dis7<=0))//�жϴ��г������Ƿ�����
//	{
//		RequestStart(Small_CAR); //����С��433��������	
//		
//		count1++;
//		if(count1>10)//�����������10�λ�û�еõ���ȷ���ݣ����������ݴ�����·�����⣬��Ҫ���
//		{
//			break;
//		}
//	}
//	if(count1<=10)//��������
//	{
//		/*
//			�����������ϴ����ݣ��˶κ�����Ҫ����
//		*/
//		
//		//�ж��г�λ��
//		if((abs(laser.dis6-ORIGIN_X)<200)&&(abs(laser.dis7-ORIGIN_Y)<200))
//		{
//			//(x,y)�����ڳ�ʼλ�ã������ж�z�������Ƿ��ڳ�ʼλ�ø���
//			RequestStop(Small_CAR); //����С��433ֹͣ��������	
//			RequestStart(Small_CLAW); //����Сצ433��������	
//			
//			while (laser.dis1<=0)//�ж�Сצ�ϴ������Ƿ�����
//			{
//				count2++;
//				if(count2>10)//�����������10�λ�û�еõ���ȷ���ݣ����������ݴ�����·�����⣬��Ҫ���
//				{
//					break;
//				}
//				for (uint8_t i = 0; i < 5; i++)
//				{
//					RequestStop(Small_CAR); //����С��433ֹͣ��������	
//					RequestStart(Small_CLAW); //����Сצ433��������	
//				}			 
//			}		
//			if(abs(laser.dis1-ORIGIN_Z)<200)
//			{
//				/*����Ҫ��*/
//				RequestStop(Small_CLAW); //����Сצ433ֹͣ��������	
//			}
//			else
//			{
//				VerticalMoveOrigin(ORIGIN_Z);//�ƶ�����ʼλ��
//			}
//		}
//		else
//		{
//			//�����жϴ�צ�������������Ϸ�����ƽ̨�Ϸ�
//			if(laser.dis6<ADD_X)//��ƽ̨�Ϸ�
//			{		
//				RisePawFromBurnPool();
//				HorizontalMoving(ORIGIN_X,ORIGIN_Y);
//				VerticalMoveOrigin(ORIGIN_Z);
//			}
//			else//���������Ϸ�
//			{
//				RisePawFromLitterPool();
//				HorizontalMoving(ORIGIN_X,ORIGIN_Y);
//				VerticalMoveOrigin(ORIGIN_Z);				
//			}			
//		}
//	}
//	ResetFlagBit();
//	RelayOff();//�ر�ң���� 
//	
}
/*
*****************************************************************************************************************
*                                     DataCommunicateManage(uint8_t task_mode,uint8_t OnorOff)
*
*Description : ����ͨѶ�������ڴ�ĳһ·�ϴ���������
*Arguments   : uint8_t task_mode������ѡ��ͬ�豸�ϵļ��� OnorOff:1����on,0����off
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
//����ͨѶ����
void DataCommunicateManage(uint8_t task_mode,uint8_t OnorOff)
{
	float dis1_err = 0.0f;
	float dis2_err = 0.0f;
	float dis3_err = 0.0f;
	
	if(SMALL_CAR==task_mode)//����С�г�433
	{	
		if(1==OnorOff)
		{
			dis2_err = laser.dis2 - laser.last_dis2;
			dis3_err = laser.dis3 - laser.last_dis3; 
			
			if ((dis2_err==0)&&(dis3_err==0))//�ж�С�г������Ƿ�����  ���������UWB
			{
				SmallCarDataCorrect = 0;
				RequestStart(SMALL_CAR);  //����С��433��������	
			}	
			else
				SmallCarDataCorrect = 1;	
			
			laser.last_dis2 = laser.dis2;    //������ʷֵ
			laser.last_dis3 = laser.dis3;
		}
		else if(0==OnorOff)
		{
			RequestStop(SMALL_CAR);
		}
	}
	else if(SMALL_CLAW==task_mode)//����Сצ433
	{
		if(1==OnorOff)
		{
			dis1_err = laser.dis1 - laser.last_dis1;
			
			if (dis1_err ==0.0f )//�ж�Сצ�������Ƿ�����
			{
				SmallClawDataCorrect = 0;
				RequestStart(SMALL_CLAW);  //����Сצ433��������	
			}	
			else
				SmallClawDataCorrect = 1;		
			
			laser.last_dis1 = laser.dis1;//������ʷֵ
		}
		else if(0==OnorOff)
		{
			RequestStop(SMALL_CLAW);
		}
	}
}

/*
*****************************************************************************************************************
*                                    void HorizontalDotMove(float err_x,float err_y)
*
*Description : С�г��㶯
*Arguments   : float err_x,float err_y������λ��ƫ����õ���һ���ᴫ0
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
*Description : Сצ�ӵ㶯
*Arguments   : float err_z������λ��ƫ��
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
}//��λ��־λ
/*
*****************************************************************************************************************
*                                    void ResetFlagBit(void)
*
*Description : ��λ��־λ
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
*Description : ����С�г�/Сצ��ֹͣ����ָ��
*Arguments   : dev:�豸��
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
*Description : ������г�/��צ�ӿ�ʼ����ָ��
*Arguments   : dev:�豸��
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
//��ͣ
void TaskSuspend(void)
{
	Up_Data.Status = (Up_Data.Status&0x87)|0x50;//��ʱ��״ֵ̬ ��ͣ
	
	BCVar._CloseFlag = CloseFlag;
	BCVar._OpenFlag = OpenFlag;
	BCVar._PointMove = PointMove;
	BCVar._ReverseStop = ReverseStop;
	BCVar._Run_Mode = Run_Mode;
	BCVar._WaitFlag = WaitFlag;
	
	PowerOff();//��ң�����ϵ�
	
	CloseFlag = 0;
	OpenFlag = 0;
	PointMove = 0;
	ReverseStop = 0;
	Run_Mode = 0;
	WaitFlag = 0;
	//Up_Data.Status   //��ͣ
}
//�ָ�����
void ConExecute(void)
{

	Up_Data.Status = (Up_Data.Status&0x87)|0x58;//��ʱ��״ֵ̬ �ָ�
	CloseFlag = BCVar._CloseFlag;
	OpenFlag = BCVar._OpenFlag;
	PointMove = BCVar._PointMove;
	ReverseStop = BCVar._ReverseStop;
	Run_Mode = BCVar._Run_Mode;
	WaitFlag = BCVar._WaitFlag;
	
	PowerOn();//��ң�����ϵ�

	BCVar._CloseFlag = 0;
	BCVar._OpenFlag = 0;
	BCVar._PointMove = 0;
	BCVar._ReverseStop = 0;
	BCVar._Run_Mode = 0;
	BCVar._WaitFlag = 0;

}

void SmallCarStop(void)//��������
{
	Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ down
	PowerOff();//��ң�����ϵ�
	
	CloseFlag = 0;
	OpenFlag = 0;
	PointMove = 0;
	ReverseStop = 0;
	Run_Mode = 0;
	WaitFlag = 0;	
}
