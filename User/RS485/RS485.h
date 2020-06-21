#ifndef __RS485_H
#define	__RS485_H

#include "stm32f4xx.h"
#include "GPIO.h"


#define RS485_DE  PAout(1)
#define RS485_RE  PAout(2)

extern void RS485_Init(u32 bound);
extern void RS485_Send_Data(void);

#endif

