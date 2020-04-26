#ifndef UART2_H__
#define UART2_H__

#include "stm32f4xx.h"
#include "bsp_SysTick.h"

extern void USART2_Init(u32 bound);
extern void USART2_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
extern void USART2_Upper_f_Computer(float data1,float data2,float data3,float data4);	 
void uart2_tx_task(unsigned char send_date[],uint8_t uart_cmd);
void RequestStopToSmallClaw(void);
void RequestStartToSmallClaw(void);
#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
	 


#endif

