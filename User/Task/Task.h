#ifndef __TASK_H
#define	__TASK_H

#include "stm32f4xx.h"

/*******************************************************
 *                     �ṹ�嶨��
 *******************************************************/

 /*
 *************************************************************************
 *                        �궨��
 *************************************************************************  
 */



/************************************************************/

//static void SmallCarTask(void);         //δȷ���Ƿ���Ҫ
static void SmallCarRunning(void);
static void HorizontalMoving(float x,float y);
static void VerticalMoving(float z);
//static void VerticalMoveOrigin(float z);   δȷ���Ƿ���Ҫ
static void RisePawFromLitterPool(void);
#endif /* __TASK_H */

