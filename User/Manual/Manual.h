#ifndef __MANUAL_H
#define	__MANUAL_H

#include "stm32f4xx.h"

/*******************************************************
 *                     结构体定义
 *******************************************************/

 /*
 *************************************************************************
 *                        宏定义
 *************************************************************************  
 */

extern uint8_t HTaskModeFlag;

/************************************************************/

extern void ManualXMoving(float x);
extern void ManualYMoving(float y); 
extern void ManualClose(void);
extern void ManualOpen(void);
extern void ManualRaisePawFromLitterPool(void);
#endif /* __TASK_H */

