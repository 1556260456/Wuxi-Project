#ifndef __LCFUNC_H
#define	__LCFUNC_H

#include "stm32f4xx.h"




/*******************************************************
 *                     �ṹ�嶨��
 *******************************************************/

 /*
 *************************************************************************
 *                        �궨��
 *************************************************************************  
 */
#define GARBAGE_X        16000       //������y�᳤
#define GARBAGE_Y        500         //������x�᳤

#define ORIGIN_X            1500     //Сצ�ӳ�ʼXλ��
#define ORIGIN_Y            2500     //Сצ�ӳ�ʼYλ��
#define ORIGIN_Z            5000     //Сצ�ӳ�ʼZλ��

#define abs(x)  ((x<0)?(-x):x)       //��x�ľ���ֵ

extern uint8_t X_MOVE_BIT;            //�г���צ���ƶ���־λ
extern uint8_t Y_MOVE_BIT;
extern uint8_t DOWN_BIT;
extern uint8_t UP_BIT;


extern uint8_t SmallCarDataCorrect; 
extern uint8_t SmallClawDataCorrect;
extern uint8_t HTaskModeFlag;
extern uint8_t ReverseStop;
extern int ReverseTime;
extern uint8_t PointMove;
extern int PointMoveTime;
extern uint8_t UpOrDown;
extern uint8_t ErrorSmallCar;
//1����ʾ��£
extern uint8_t CloseFlag;
//��£��ʱ
extern int     CloseDelay;
//1:��ʾ��
extern uint8_t OpenFlag;
//����ʱ
extern int     OpenDelay;

extern int64_t Small_Claw_Up_Delay;//��ʱʱ�� S
extern uint8_t Small_Claw_Up_Delay_Flag;//�򿪶�ʱ����־λ

extern int64_t Small_Claw_Up_Delay_Pool;
extern uint8_t Small_Claw_Up_Delay_Pool_Flag;

//��ͣʱ��Ҫ�����ı���
typedef struct
{
	uint8_t _WaitFlag;//��ʱͨѶ״̬
	uint8_t _Run_Mode;//����ģʽ
	uint8_t _ReverseStop;//����ֹͣ��־λ
	uint8_t _PointMove;//�㶯��־λ
	uint8_t _CloseFlag;//צ�Ӻ�£��־λ
	uint8_t _OpenFlag;//צ���ſ���־λ
}Provar;
/************************************************************/

extern void ClosePaw(void);
extern void ReleasePaw(void);
extern void XMoving(float x);
extern void YMoving(float y);
extern void UpPawFromLitterPool(float z);
extern void DownPawToLitterPool(float z);
extern void SelfCheckStatus(void);
extern void DataCommunicateManage(uint8_t task_mode,uint8_t OnorOff);
extern void RevStop(float err);
extern void HorizontalDotMove(float err_x,float err_y);
extern void VerticalDotMove(float err_z);
extern void ResetFlagBit(void);
extern void RequestStop(uint8_t dev);
extern void RequestStart(uint8_t dev);
extern void TaskSuspend(void);
extern void ConExecute(void);
extern void SmallCarStop(void);
extern void BackToOriginState(void);
#endif /* __TASK_H */

