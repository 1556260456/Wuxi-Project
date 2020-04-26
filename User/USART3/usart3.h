#ifndef USART3_H__
#define USART3_H__

#include "stm32f4xx.h"


extern void USART3_Init(u32 bound);
extern void USART3_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);




#endif




