#ifndef __TASK_H
#define	__TASK_H

#include "stm32f4xx.h"

/*******************************************************
 *                     结构体定义
 *******************************************************/

 /*
 *************************************************************************
 *                        宏定义
 *************************************************************************  
 */



/************************************************************/

//static void SmallCarTask(void);         //未确定是否需要
static void SmallCarRunning(void);
static void HorizontalMoving(float x,float y);
static void VerticalMoving(float z);
//static void VerticalMoveOrigin(float z);   未确定是否需要
static void RisePawFromLitterPool(void);
#endif /* __TASK_H */

