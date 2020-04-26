#ifndef UART1_H__
#define UART1_H__


#include "stm32f4xx.h"		  
#include "bsp_SysTick.h"
/*******************************************************
 *                     结构体定义
 *******************************************************/


extern uint16_t  re_ch[16]; 

extern void USART1_Init(u32 bound);		
extern void USART1_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
extern void USART1_Upper_f_Computer(float data1,float data2,float data3,float data4,float data5,float data6);
void uart1_tx_task(unsigned char send_date[],uint8_t uart_cmd);	 
void RequestStopToSmallCar(void);
void RequestStartToSmallCar(void);

#ifdef __cplusplus
}
#endif

#endif








