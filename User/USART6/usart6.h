#ifndef USART6_H__
#define USART6_H__

#include "stm32f4xx.h"		   
 
extern void USART6_Init(u32 bound);		 
extern void USART6_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
extern void USART6_Upper_f_Computer(float data1,float data2,float data3,float data4,float data5,float data6);
#endif

