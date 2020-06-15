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
#define Manual 3  //1:手动方式1 2：手动方式2  3：手动方式3
extern uint8_t HTaskModeFlag;
extern uint8_t ManualError;

/************************************************************/

extern void ManualXMoving(float x);
extern void ManualYMoving(float y); 
extern void ManualClose(void);
extern void ManualOpen(void);
extern void ManualRaisePawFromLitterPool(float z);
extern void ManualDowntoLitterPool(float z);
extern void ManualPowerOn(void);
extern void ManualPowerOff(void);
#endif /* __TASK_H */

