#include "Manual.h"
#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "relay.h"

 /*
 *************************************************************************
 *                       ȫ�ֱ���
 *************************************************************************  
 */
extern uint8_t WaitFlag;
extern uint8_t Run_Mode;

//�ֶ�״̬�µ����в��� 1:ֹͣ,2:X,3:Y,4:��,4:��,6:ץ,7:��,
uint8_t HTaskModeFlag = 0;
uint8_t ManualError = 0;
/*
*****************************************************************************************************************
*                                     void ManualXMoxing(float x)
*
*Description : �ֶ�X�����ƶ�
*Arguments   : Ŀ���x����
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualXMoving(float x)
{
//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
//------------------------------------------------------------	
 if( (1==SmallCarDataCorrect)&&(RelayOnflag == -2) )//��������
	{		
		if(0==X_MOVE_BIT)
		{
				XMoving(x);	
		}
		else if(1==X_MOVE_BIT)//���н���
		{
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				X_MOVE_BIT = 0;//��λ
			  Run_Mode = 0;//ģʽ��λ
			  PointMove = 0;
			  Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ �������
		}
		else if(2==X_MOVE_BIT)//�����쳣���½���
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				X_MOVE_BIT = 0;//��λ
			}
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ManualXMoxing(float y)
*
*Description : �ֶ�Y�����ƶ�
*Arguments   : Ŀ���y����
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualYMoving(float y)
{
//------------------------------------------------------------
//	PowerOn();//�г��ϵ�
//------------------------------------------------------------	
 if((1==SmallCarDataCorrect)&&(RelayOnflag == -2))//��������
	{
		
		if(0==Y_MOVE_BIT)
		{
				YMoving(y);
		}
		else if(1==Y_MOVE_BIT)//���н���
		{
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
			  Run_Mode = 0;//ģʽ��λ
				Y_MOVE_BIT = 0;//��λ
			  Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ �������
		}
		else if(2==Y_MOVE_BIT)//���н���
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				Y_MOVE_BIT = 0;//��λ
			}
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ManualClose(void)
*
*Description : ��£Сצ��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualClose(void)
{
	ClosePaw();
	if(CloseFlag==-2)
	{
		Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ �������
		HTaskModeFlag=0;//������ģʽ
		WaitFlag = 0;//����ȴ�״̬
		Run_Mode = 0;//ģʽ��λ
		OpenFlag = -1;//��λ	
	}		
}
/*
*****************************************************************************************************************
*                                     void ManualOpen(void)
*
*Description : �ſ�Сצ��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualOpen(void)
{
	ReleasePaw();
	if(OpenFlag==-2)
	{
		Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ �������
		HTaskModeFlag=0;//������ģʽ
		WaitFlag = 0;//����ȴ�״̬
		Run_Mode = 0;//ģʽ��λ
		CloseFlag = -1;//��λ 	
	}	
}
/*
*****************************************************************************************************************
*                                     void ManualRaisePawFromLitterPool(float z)
*
*Description : ��צ�Ӵ�������̧��  (��һ���ֶ����µĳ���)
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualRaisePawFromLitterPool(float z)
{
<<<<<<< HEAD
=======
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //------------------------------------------------------------
>>>>>>> f02ed4fcf33be44e9cc1f4e11d06fb0a95e09135
	
	if( (1==SmallClawDataCorrect) && (RelayOnflag == -2) )//��������
	{
		if(UP_BIT == 0)//��������
		{
		  UpPawFromLitterPool(z);		
		}
		else if(UP_BIT == 1)//�������н���
		{
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			Run_Mode = 0;//ģʽ��λ
			UP_BIT = 0;//��λ
			Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ �������
		}
		else if(UP_BIT == 2)//���й����г���
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				ManualError = 1;//��ʾ����
			  Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				UP_BIT = 0;//��λ
			}	
		}
	}		
}

/*
*****************************************************************************************************************
*                                   void ManualDowntoLitterPool(float z)
*
*Description : ��ֱ�½����ϳصײ�ץ��
*Arguments   : z:צ���½����������ĸ߶�
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualDowntoLitterPool(float z)
{
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //------------------------------------------------------------	
	if((1==SmallClawDataCorrect)&&(RelayOnflag == -2))//��������
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToLitterPool(z);		
		}
		else if(DOWN_BIT == 1)//�������н���
		{
			/*�½�����*/
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				DOWN_BIT = 0;//��λ
		}
		else if(DOWN_BIT == 2)//���й����г���
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			Run_Mode = 0;//ģʽ��λ
			DOWN_BIT = 0;//��λ
		
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ManualPowerOn(void)
*
*Description : ��ң����
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualPowerOn(void){
	
	//------------------------------------------------------------	
	PowerOn();//�г��ϵ�
	//------------------------------------------------------------
	if(RelayOnflag == -2){//ң�����Ѵ�
		/*����һ���Ѵ�ң������ָ��*/
		Up_Data.Status = (Up_Data.Status&0x87)|0x68;//��ʱ��״ֵ̬  ң�����Ѵ�	
		WaitFlag = 0;
		Run_Mode = 0; 
		HTaskModeFlag = 0;
		RelayOffflag = -1;
	}	
}
/*
*****************************************************************************************************************
*                                     void ManualPowerOff(void)
*
*Description : �ر�ң����
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualPowerOff(void){
	//------------------------------------------------------------
	PowerOff();//�ر�ң����
	//------------------------------------------------------------
	if(RelayOffflag == -2){//ң�����ѹر�
		/*����һ���ѹر�ң������ָ��*/
		Up_Data.Status = (Up_Data.Status&0x87)|0x70;//��ʱ��״ֵ̬  ң�����ѹر�
		WaitFlag = 0;
		Run_Mode = 0; 
		HTaskModeFlag = 0;	
		RelayOnflag = -1;		
	}
}
