#include "FullAuto.h"
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
extern int test_distance;

extern POSITION origin;//��ʼλ��
extern POSITION target;//Ŀ��λ�� 
uint8_t FullAutoStep = 0;
/*
*****************************************************************************************************************
*                                    void SmallCarFullAutoMode(void)
*
*Description : ȫ�Զ�����ִ�к���
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void SmallCarFullAutoMode(void)
{
	if(0==FullAutoStep)//�г��ϵ�
 	{
		PowerOn();
		if(RelayOnflag==-2)
		{
			FullAutoStep++;//������һ��
			RelayOnflag=-1;//��־λ��λ
			mpu.dis = test_distance;// ����ȫ�Զ�����ʹ��
		}
		//Up_Data.Status = (Up_Data.Status&0xF0)|(0xF1);	
	}
	else if(1==FullAutoStep)// ���С�г��Ƿ��ڳ�ʼλ�á���ʼ״̬
	{
		BackToOriginState();
		if(UP_BIT == 1)//��������
		{
			FullAutoStep++;//������һ��
			UP_BIT=0;//��־λ��λ
		}
	}
	else if(2==FullAutoStep)//ˮƽ�ƶ���ץ�ϴ����Ϸ�
	{
		HorizontalMoving(target.x[0],target.y[0]);
		if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
		{
			FullAutoStep++;//������һ��
			X_MOVE_BIT=0;//��־λ��λ
			Y_MOVE_BIT=0;//��־λ��λ
<<<<<<< HEAD
			mpu.dis = test_distance;// ����ȫ�Զ�����ʹ��
=======
>>>>>>> f02ed4fcf33be44e9cc1f4e11d06fb0a95e09135
		}
	}
	else if(3==FullAutoStep)//צ�ӿ�ʼ�½�ȥץ��
	{
		DowntoLitterPool(target.z[0]);
		if(1==DOWN_BIT)
		{
			FullAutoStep++;//������һ��
			DOWN_BIT = 0;//��־λ��λ
			CloseFlag=1;//������ִ�е�ץ�ϱ�־λ��1
		}
	}
	else if(4==FullAutoStep)//ִ��ץ�϶���
	{
		HFClosePaw();
		if(-2==CloseFlag)
		{
			FullAutoStep++;//������һ��
			CloseFlag = 0; //��־λ��λ
<<<<<<< HEAD
			mpu.dis = test_distance;// ����ȫ�Զ�����ʹ��
=======
>>>>>>> f02ed4fcf33be44e9cc1f4e11d06fb0a95e09135
		}
	}
	else if(5==FullAutoStep)//ִ��Сצ��������
	{
		RisePawFromLitterPool();
		if(1==UP_BIT)
		{
			FullAutoStep++;//������һ��
			UP_BIT = 0;//��־λ��λ
		}
	}
	else if(6==FullAutoStep)//ˮƽ�ƶ�������λ��
	{
		HorizontalMoving(target.x[1],target.y[1]);
		if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
		{
			FullAutoStep++;//������һ��
			X_MOVE_BIT=0;  //��־λ��λ
			Y_MOVE_BIT=0;  //��־λ��λ
			mpu.dis = test_distance;// ����ȫ�Զ�����ʹ��
		}
	}
	else if(7==FullAutoStep)//�·�Сצ�ӿ���������
	{
		DowntoLitterPool(target.z[1]);
		if(1==DOWN_BIT)
		{
			FullAutoStep++;//������һ��
			DOWN_BIT=0;    //��־λ��λ	
			OpenFlag=1;    //���ɿ�צ�ӱ�־λ��1
		}
	}
	else if(8==FullAutoStep)//�ɿ�צ�ӷ���
	{
		HFOpenPaw();
		if(-2==OpenFlag)
		{
			FullAutoStep++;//������һ��
			OpenFlag=0;    //��־λ��λ	
			mpu.dis = test_distance;// ����ȫ�Զ�����ʹ��
		}
	}
	else if(9==FullAutoStep)//����צ������ȫ�߶�
	{
		RisePawFromLitterPool();
		if(1==UP_BIT)
		{
			FullAutoStep++;//������һ��
			UP_BIT = 0;
		}
	}
	else if(10==FullAutoStep)//ˮƽ�ƶ�����ʼλ���Ϸ�
	{
		HorizontalMoving(ORIGIN_X,ORIGIN_Y);
		if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
		{
			FullAutoStep++;//������һ��
			X_MOVE_BIT=0;//��־λ��λ
			Y_MOVE_BIT=0;//��־λ��λ
		}
	}
	else if(11==FullAutoStep)//�Ͽ���Դ,���н���
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
			ErrorSmallCar = FullAutoStep;//��¼��������һ��
			Up_Data.Status = Up_Data.Status&0xF0;	
			RelayOffflag=-1;
			FullAutoStep=0;
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
*                                    void HorizontalMoving(float x,float y)
*
*Description : ˮƽ�ƶ�
*Arguments   : ��x,y��:Ŀ�ĵ�����
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void HorizontalMoving(float x,float y)
{
	if(1==SmallCarDataCorrect)//��������
	{
		if(X_MOVE_BIT == 0)
		{
			//С�г�X�����ƶ�
			XMoving(x);			
		}
		else if(Y_MOVE_BIT == 0)
		{
			//С�г�Y�����ƶ�
			YMoving(y);		
		}
		else
		{
			/*ˮƽ�ƶ�����*/
		}
	}
}

/*
*****************************************************************************************************************
*                                   void DowntoLitterPool(float z)
*
*Description : ��ֱ�½����ϳصײ�ץ��
*Arguments   : z:צ���½����������ĸ߶�
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void DowntoLitterPool(float z)
{
	if(1==SmallClawDataCorrect)//��������
	{
		if(DOWN_BIT == 0)
		{
		  DownPawToLitterPool(z);		
		}
		else if(DOWN_BIT == 1)
		{
			/*�½�����*/
		}
	}
}
/*
*****************************************************************************************************************
*                                     void HClosePaw(void)
*
*Description : ץ�Ϻ���
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
*Description : ��������̧��צ��
*Arguments   : none
*Returns     : none
*Notes       : �տ�ʼ��������ļ��⣬���е��ϰ벿����������ļ���
*****************************************************************************************************************
*/
void RisePawFromLitterPool(void)
{
	if(1==SmallClawDataCorrect)//��������
	{
		if(UP_BIT == 0)
		{
		  UpPawFromLitterPool(ORIGIN_Z);		
		}
		else if(UP_BIT == 1)
		{
			/*��������*/
		}
	}	
}

/*
*****************************************************************************************************************
*                                    void MoveBackToOriginState(float x,float y)
*
*Description : ������צ�ӣ���ˮƽ�ƶ�����ʼλ��
*Arguments   : origin��x,y,z��:Ŀ�ĵ�����
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void BackToOriginState(void)
{
	if((1==SmallClawDataCorrect) && (1==SmallCarDataCorrect))//��������
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
*Description : ���Ϻ���
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void HFOpenPaw(void)
{
	ReleasePaw();
}
