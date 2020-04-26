#ifndef __FULLAUTO_H
#define	__FULLAUTO_H

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
extern void SmallCarFullAutoMode(void);
extern void RisePawFromPlatform(void);
extern void HorizontalMoving(float x,float y);
extern void DowntoLitterPool(float z);
extern void HFClosePaw(void);
extern void RisePawFromLitterPool(void);
extern void DownClawtoBurnPool(void);
extern void HFOpenPaw(void);
extern void RisePawFromBurnPool(void);
extern void DownToOrigin(float z);
extern void BackToOriginState(void);

#endif /* __TASK_H */

