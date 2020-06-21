
#include "HalfAuto.h"
#include "FullAuto.h"
#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "usart2.h"
#include "relay.h"

 /*
 *************************************************************************
 *                        �궨��
 *************************************************************************  
 */
 //1:�ڴ˲���ɺ���Ҫ�����Ƿ�ִ����һ��
 //0:�ڴ˲���ɺ���Ҫ����ֱ��ִ����һ��
 #define RESTEP_1      0// �Ƿ���г��ϵ�
 #define RESTEP_2      0// �Ƿ��Сצ����������ȫλ��
 #define RESTEP_3      0// �Ƿ�С�г�ˮƽ�ƶ���ץ��λ��
 #define RESTEP_4      0// �Ƿ��·�Сצ��
 #define RESTEP_5      0// �Ƿ�ִ��ץ�϶���
 #define RESTEP_6      0// �Ƿ�Сצץ�Ϻ�����
 #define RESTEP_7      0// �Ƿ�ˮƽ�ƶ���Ͷ��λ��
 #define RESTEP_8      0// �Ƿ�׼���·�צ��Ͷ��
 #define RESTEP_9      0// �Ƿ���צ����
 #define RESTEP_10     0// �Ƿ�Ͷ�Ϻ�ץ������
 #define RESTEP_11     0// �Ƿ�С�г�ˮƽ�ƶ�����ʼλ��
 /*
 *************************************************************************
 *                       ȫ�ֱ���
 *************************************************************************  
 */
extern POSITION origin;//��ʼλ��
extern POSITION target;//Ŀ��λ�� 

extern uint8_t WaitFlag;
extern uint8_t Run_Mode;
extern int test_distance;
uint8_t HalfAutoStep = 0;
uint8_t SingleStepOver = 0;//0:����˲�����ִ�У�1:�����ڴ˲�ִ�н���
int8_t IsExecute = -1;//1:ִ����һ��,0����ִ����һ��
int8_t Origin_state =0;
/*
*****************************************************************************************************************
*                                    void SmallCarHalfAutoMode(void)
*
*Description : ���Զ�����ִ�к���
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void SmallCarHalfAutoMode(void)
{
	if(0==HalfAutoStep)//�г��ϵ�
 	{
		if(0==SingleStepOver)
		{
			PowerOn();
			if(RelayOnflag==-2)
			{
				Up_Data.Status = (Up_Data.Status&0x87)|0x68;//��ʱ��״ֵ̬  ң�����Ѵ�
				#if RESTEP_1==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				RelayOnflag=-1;//��־λ��λ	
				mpu.dis = test_distance;//��������ʹ��				
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}	
		}
		Up_Data.Status = (Up_Data.Status&0xF0)|(0xF1);  //�����״̬��ʾʲô��˼
	}
	else if(1==HalfAutoStep)        
	{
		if(0==SingleStepOver)
		{
			Origin_state = 1;
<<<<<<< HEAD
			BackToOriginState();		//�Ƿ���Ҫȷ��С�г��ڳ�ʼ״̬������ֻ��ȷ��С�г�צ�����𼴿�
=======
			BackToOriginState();		//ȷ��С�г��ڳ�ʼ״̬
>>>>>>> f02ed4fcf33be44e9cc1f4e11d06fb0a95e09135
			Origin_state = 0;
			if(UP_BIT == 1)         //��������
			{
				#if RESTEP_2==0       //ֱ�ӽ�����һ��
					HalfAutoStep++; 
				#elif RESTEP_1==1     //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				UP_BIT=0;//��־λ��λ
			}			
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}	
		}
	}
	else if(2==HalfAutoStep)//ˮƽ�ƶ���ץ�ϴ����Ϸ�
	{
		if(0==SingleStepOver)
		{
			HorizontalMoving(target.x[0],target.y[0]);
			if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
			{
				#if RESTEP_3==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				X_MOVE_BIT=0;//��־λ��λ
				Y_MOVE_BIT=0;//��־λ��λ		
				mpu.dis = test_distance;  //��������ʹ��				
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}			
		}
	}
	else if(3==HalfAutoStep)//צ�ӿ�ʼ�½�ȥץ��
	{
		if(0==SingleStepOver)
		{
			DowntoLitterPool(target.z[0]);
			if(1==DOWN_BIT)
			{
				#if RESTEP_4==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				  CloseFlag=1;//������ִ�е�ץ�ϱ�־λ��1
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				DOWN_BIT = 0;//��־λ��λ
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				CloseFlag=1;//������ִ�е�ץ�ϱ�־λ��1
				IsExecute=0;
			}				
		}
	}
	else if(4==HalfAutoStep)//ִ��ץ�϶���
	{
		if(0==SingleStepOver)
		{
			HFClosePaw();
			if(-2==CloseFlag)
			{
				#if RESTEP_5==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				CloseFlag = 0; //��־λ��λ		
				mpu.dis = test_distance;//��������ʹ��				
			}			
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}		
		}
	}
	else if(5==HalfAutoStep)//ִ��Сצ��������
	{
		if(0==SingleStepOver)
		{
			RisePawFromLitterPool();
			if(1==UP_BIT)
			{
				#if RESTEP_6==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				UP_BIT = 0;//��־λ��λ
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}	
		}
	}
	else if(6==HalfAutoStep)//ˮƽ�ƶ���Ͷ��λ������
	{
		if(0==SingleStepOver)
		{
			HorizontalMoving(target.x[1],target.y[1]);
			if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
			{
				#if RESTEP_7==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				X_MOVE_BIT=0;  //��־λ��λ
				Y_MOVE_BIT=0;  //��־λ��λ
				mpu.dis = test_distance;//��������ʹ��
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
	else if(7==HalfAutoStep)//�·�Сצ�ӿ���������
	{
		if(0==SingleStepOver)
		{
			DowntoLitterPool(target.z[1]);
			if(1==DOWN_BIT)
			{
				#if RESTEP_8==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				  OpenFlag=1;    //���ɿ�צ�ӱ�־λ��1
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				DOWN_BIT=0;    //��־λ��λ	
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				OpenFlag=1;    //���ɿ�צ�ӱ�־λ��1
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
	else if(8==HalfAutoStep)//�ɿ�צ�ӷ���
	{
		if(0==SingleStepOver)
		{
			HFOpenPaw();
			if(-2==OpenFlag)
			{
				#if RESTEP_9==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				OpenFlag=0;    //��־λ��λ
				mpu.dis = test_distance;//��������ʹ��				
			}
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
	else if(9==HalfAutoStep)//����צ������ȫ�߶�
	{
		if(0==SingleStepOver)
		{
			BackToOriginState();
			//UpPawFromLitterPool(ORIGIN_Z);
			if(1==UP_BIT)
			{
				#if RESTEP_10==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				UP_BIT = 0;
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
	else if(10==HalfAutoStep)//ˮƽ�ƶ�����ʼλ���Ϸ�
	{
		if(0==SingleStepOver)
		{
			HorizontalMoving(ORIGIN_X,ORIGIN_Y);
			if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
			{
				#if RESTEP_11==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				X_MOVE_BIT=0;//��־λ��λ
				Y_MOVE_BIT=0;//��־λ��λ
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
<<<<<<< HEAD
	else if(11==HalfAutoStep)//�Ͽ���Դ,���н���
=======
	else if(10==HalfAutoStep)//�Ͽ���Դ,���н���
>>>>>>> f02ed4fcf33be44e9cc1f4e11d06fb0a95e09135
	{
		PowerOff();
		if(RelayOffflag==-2)
		{
			Up_Data.Status = (Up_Data.Status&0x87)|0x40;//��ʱ��״ֵ̬ ���
			RelayOffflag=-1;
			WaitFlag=0;      
			Run_Mode=0;
		}
	}
	
	if((2==X_MOVE_BIT)||(2==Y_MOVE_BIT)||(2==DOWN_BIT)||(2==UP_BIT))//���г���
	{
		PowerOff();
		if(RelayOffflag==-2)
		{
			ErrorSmallCar = HalfAutoStep;//��¼��������һ��
			Up_Data.Status =  Up_Data.Status&0x7F;
			RelayOffflag=-1;
			HalfAutoStep=0;
			X_MOVE_BIT=0;//��־λ��λ
			Y_MOVE_BIT=0;//��־λ��λ
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
*Description : ������ÿһ���м�����ȷ���Ƿ�ִ����һ��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ConfirmNextStep(uint8_t step)
{
	Up_Data.HalfStep = step;
}
