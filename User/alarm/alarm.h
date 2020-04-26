#ifndef __ALARM_H
#define	__ALARM_H

#include "stm32f4xx.h"

//Òý½Å¶¨Òå
/*******************************************************/


/************************************************************/
#define alarm        PBout(12)      

#define ALARM_ON     (alarm=1)
#define ALARM_OFF    (alarm=0)

void Alarm_GPIO_Init(void);


#endif 
