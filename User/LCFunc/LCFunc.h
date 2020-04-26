#ifndef __LCFUNC_H
#define	__LCFUNC_H

#include "stm32f4xx.h"




/*******************************************************
 *                     结构体定义
 *******************************************************/

 /*
 *************************************************************************
 *                        宏定义
 *************************************************************************  
 */
#define GARBAGE_X        16000       //垃圾池y轴长
#define GARBAGE_Y        500         //垃圾池x轴长

#define ORIGIN_X            1500     //小爪子初始X位置
#define ORIGIN_Y            2500     //小爪子初始Y位置
#define ORIGIN_Z            5000     //小爪子初始Z位置

#define abs(x)  ((x<0)?(-x):x)       //求x的绝对值

extern uint8_t X_MOVE_BIT;            //行车及爪子移动标志位
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
//1：表示合拢
extern uint8_t CloseFlag;
//合拢延时
extern int     CloseDelay;
//1:表示打开
extern uint8_t OpenFlag;
//打开延时
extern int     OpenDelay;

extern int64_t Small_Claw_Up_Delay;//延时时间 S
extern uint8_t Small_Claw_Up_Delay_Flag;//打开定时器标志位

extern int64_t Small_Claw_Up_Delay_Pool;
extern uint8_t Small_Claw_Up_Delay_Pool_Flag;

//暂停时需要保护的变量
typedef struct
{
	uint8_t _WaitFlag;//此时通讯状态
	uint8_t _Run_Mode;//任务模式
	uint8_t _ReverseStop;//反向停止标志位
	uint8_t _PointMove;//点动标志位
	uint8_t _CloseFlag;//爪子合拢标志位
	uint8_t _OpenFlag;//爪子张开标志位
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

