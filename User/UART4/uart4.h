#ifndef UART4_H__
#define UART4_H__

#include "stm32f4xx.h"

#define U4_BUFFSIZERECE  100
#define U4_BUFFSIZESEND  100
extern int Openmv_Down_Dis_F;
extern int Openmv_Down_Dis_B;
extern int Openmv_Down_RTDis_F;
extern int Openmv_Down_RTDis_B;
extern u8 hoe_down;
extern u8 hoe_up;
extern u8 slow_down;
extern char u4_receive_buff[U4_BUFFSIZERECE];
typedef union
{
 int   i;
 char  ch[4];
}INT_CONV4;//4个单字节数据转换

extern void UART4_Init(u32 bound);
extern void UART4_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
void UART4_Upper_f_Computer(float data1,float data2,float data3,float data4);	 

#endif

