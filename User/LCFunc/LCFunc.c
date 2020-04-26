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

 //0:û�����У�1���������н�����2�����ݳ��Ĺ��ϲ����ѽ���
uint8_t X_MOVE_BIT=0;
uint8_t Y_MOVE_BIT=0;
uint8_t DOWN_BIT=0;
uint8_t UP_BIT=0;

//Сצ�����̶�ʱ�������
uint8_t down_time = 0;
uint8_t down_distance = 0;
uint8_t up_time = 0;
uint8_t up_distance = 0;
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
//�����������
uint8_t ReverseCount = 0;

//С�г��㶯��־λ
uint8_t PointMove = 0;
//�㶯ʱ��
int PointMoveTime = 0; 

//Сצ�����Ͽ�����ʱ�õ�
uint8_t UpOrDown = 0;//0����׼������1�����������

//1����ʾ��£
uint8_t CloseFlag = 0;
//��£��ʱ
int     CloseDelay = 0;
//1:��ʾ��
uint8_t OpenFlag = 0;
//����ʱ
int     OpenDelay = 0;
//����״̬
uint8_t ErrorSmallCar = 0;

int64_t Small_Claw_Up_Delay = 0;//��ʱʱ�� S
uint8_t Small_Claw_Up_Delay_Flag = 0;//�򿪶�ʱ����־λ

int64_t Small_Claw_Up_Delay_Pool = 0;
uint8_t Small_Claw_Up_Delay_Pool_Flag = 0;

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
void ClosePaw(void)
{
	Up_Data.Status = (Up_Data.Status&0x87)|0x30;//��ʱ��״ֵ̬ close
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
*Description : �ɿ�צ��
*Arguments   : none
*Returns     : none
*Notes       : ʹ�ô˺���ǰOpenFlag��1���˹��ܽ�����ὫOpenFlag��OpenDelay����
*****************************************************************************************************************
*/
void ReleasePaw(void)
{
	Up_Data.Status = (Up_Data.Status&0x87)|0x38;//��ʱ��״ֵ̬  release
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
	
	if(abs(err_x)>2000)//ƫ�����һ����Χʱ��Ҫ�ƶ�	
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
			//С�г�����ֹͣ
			if((err_x<(-200))&&(ReverseCount<2))
			{
				RevStop(err_x);			
			}
			else if((err_x<(-200))&&(ReverseCount>=2))//С�г����ϵ㶯
			{
				HorizontalDotMove(err_x,0.0f);
			}
		  else/*X�����ƶ�����*/
			{
				CAR_SOUTH(OFF);
				
				ReverseCount = 0;
				PointMove = 0;
				PointMoveTime = 0;
				X_MOVE_BIT = 1;
			}
		}
		else if(err_x>=0)//С�г��򱱵㶯
		{
			//С�г������ƶ�			
			if((err_x>200)&&(ReverseCount<2))
			{
				RevStop(err_x);
			}			
			else if((err_x>200)&&((ReverseCount)>=2))//С�г��򱱵㶯
			{
				HorizontalDotMove(err_x,0.0f);
			}
			else/*X�����ƶ�����*/
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
*Description : צ���������򣨴��Ͽӳ�������  �������޸ģ�
*Arguments   : float z�����������������ĸ߶�(?)
*Returns     : none
*Notes       : ������ļ���laser.dis1,����down_time,down_distance ��ȷ�������߶�
*****************************************************************************************************************
*/
void UpPawFromLitterPool(float z)
{
	float paw_err=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;

	if (laser.dis1<0)//�˳�ż�����ֵĴ���ֵ
		{
			laser.dis1=laser.last_dis1;
		}
	paw_err = laser.dis1 - z;   
	laser.last_dis1 = laser.dis1;   
	
	Up_Data.Status = (Up_Data.Status&0x87)|0x20;//��ʱ��״ֵ̬ up
	if(laser.dis1>0 && laser.dis1<10000)
	{
		paw_err = abs(laser.dis1 - z);
		if(paw_err>=100)
		{
			PAW_DOWN(ON);
			start_up = 1;
		}		
	}
	if(up_time>=down_time)   //��Сצ����ʱ�����½�ʱ���ĳһ����ʱ��ֹͣ
	{
		PAW_DOWN(OFF);
		start_up = 0;
		down_time =0;
		up_time = 0;
		UP_BIT =1;
	}
	up_distance = laser.dis1;
	if(up_distance>=down_distance)  //��Сצ����ʱ�߶�����һ���½�ǰ�߶���ȣ�ֹͣ
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
*Description : צ���½������½����Ͽ�ץ�ϣ�
*Arguments   : float z���������������ĸ߶�
*Returns     : none
*Notes       : һֱʹ��Сצ���µļ��⣬��¼�½�ǰ�ĸ߶�ֵ�Լ��½�ʱ��
*****************************************************************************************************************
*/
void DownPawToLitterPool(float z)
{
	/*
		�˴�ʹ��Сצ�����¼���
	*/
	float paw_err=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis1<0)//�˳�ż�����ֵĴ���ֵ
	{
		laser.dis1=laser.last_dis1;
	}				
	paw_err = laser.dis1 - z;   
	laser.last_dis1 = laser.dis1;   
		
	//Сצ�½�
	if(abs(paw_err)>1000)//Сצ�½�
	{
		if(paw_err>=0)//Сצ�½�
		{
			PAW_DOWN(ON);	
			down_distance = laser.dis1;
			start_down = 1;
		}
	}
	//Сצ�½���ָ���߶ȣ�׼���㶯
	else if(abs(paw_err)<1000)//��צ�½���ָ���߶ȣ�׼���㶯  (�Ƿ������������Ӧ���ڵ㶯�������)
	{
		/*���1�������������*/
		if(paw_err>200)//��ʼ�㶯
		{
			VerticalDotMove(paw_err);
		}
		else//�㶯����
		{
			PAW_DOWN(OFF);	
			start_down = 0;
			PointMove = 0;
			PointMoveTime = 0;	
			DOWN_BIT = 1;
		}
		/*���2���ڵ㶯ʱצ����б����*/
		if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>10.0f))//צ����б����һ���Ƕȣ�ֹͣ�½�������ѭ��
		{
			PAW_DOWN(OFF);
			start_down = 0;
			PointMove = 0;
			PointMoveTime = 0;	
			DOWN_BIT = 1;
		} 		
		/*���3��צ���޷��½�������������Ȼ�½�*/
		if (abs(paw_err-paw_err_last)<100)//�����Ѿ��½����ײ����������½������
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

//С�г��Լ�״̬
void SelfCheckStatus(void)
{
//	uint8_t count1=0;
//	uint8_t count2=0;
//	
//  RelayOn();//��ң����
//	RequestStart(BIG_CAR);//�����433��������
//	
//	while ((laser.dis6<=0)||(laser.dis7<=0))//�жϴ��г������Ƿ�����
//	{
//		RequestStart(BIG_CAR); //�����433��������	
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
//			RequestStop(BIG_CAR); //�����433ֹͣ��������	
//			RequestStart(BIG_CLAW); //�����צ433��������	
//			
//			while (laser.dis1<=0)//�жϴ�צ�ϴ������Ƿ�����
//			{
//				count2++;
//				if(count2>10)//�����������10�λ�û�еõ���ȷ���ݣ����������ݴ�����·�����⣬��Ҫ���
//				{
//					break;
//				}
//				for (uint8_t i = 0; i < 5; i++)
//				{
//					RequestStop(BIG_CAR); //�����433ֹͣ��������	
//					RequestStart(BIG_CLAW); //�����ץ433��������	
//				}			 
//			}		
//			if(abs(laser.dis1-ORIGIN_Z)<200)
//			{
//				/*����Ҫ��*/
//				RequestStop(BIG_CLAW); //�����צ433ֹͣ��������	
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
	if(SMALL_CAR==task_mode)//����С�г�433
	{	
		if(1==OnorOff)
		{
			if ((laser.dis2<=0)||(laser.dis3<=0))//�ж�С�г������Ƿ�����
			{
				SmallCarDataCorrect = 0;
				RequestStart(SMALL_CAR);  //�����433��������	
			}	
			else
				SmallCarDataCorrect = 1;	
		}
		else if(0==OnorOff)
		{
			RequestStop(SMALL_CAR);
		}
	}
	else if(SMALL_CLAW==task_mode)//�����צ433
	{
		if(1==OnorOff)
		{
			if (laser.dis1<=0)//�ж�Сצ�������Ƿ�����
			{
				SmallClawDataCorrect = 0;
				RequestStart(SMALL_CLAW);  //����Сצ433��������	
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
*Description : ����ֹͣ
*Arguments   : float err��ʵ��λ�þ�Ŀ��λ��ƫ��
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void RevStop(float err)
{
	if(err>0)
	{
		//С�г����Ϸ����ƶ�	
		if(ReverseStop == 0)
		{
			CAR_NORTH(OFF);
			ReverseStop=1;	       				
		}
		else if(ReverseStop==1)
		{
			if(ReverseTime<=10)//1s��
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
			if(ReverseTime<=2)//0.2sͣ
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
	//С�г��򱱷����ƶ�
	else
	{
		//С�г�����ֹͣ
		if(ReverseStop == 0)
		{
			CAR_SOUTH(OFF);
			ReverseStop=1;	       				
		}
		else if(ReverseStop==1)
		{
			if(ReverseTime<=10)//1s��
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
			if(ReverseTime<=2)//0.2sͣ
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
