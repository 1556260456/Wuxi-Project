#ifndef __KEY_H
#define	__KEY_H

#include "stm32f4xx.h"


//Òý½Å¶¨Òå
/*******************************************************/
#define KEY1_PIN                  GPIO_Pin_3                 
#define KEY1_GPIO_PORT            GPIOE                      
#define KEY1_GPIO_CLK             RCC_AHB1Periph_GPIOE

#define KEY2_PIN                  GPIO_Pin_4                 
#define KEY2_GPIO_PORT            GPIOE                      
#define KEY2_GPIO_CLK             RCC_AHB1Periph_GPIOE

#define KEY3_PIN                  GPIO_Pin_5
#define KEY3_GPIO_PORT            GPIOE
#define KEY3_GPIO_CLK             RCC_AHB1Periph_GPIOE
/*******************************************************/

#define KEY1 		PEin(3)   	                                                                       //PE3
#define KEY2 		PEin(4)		                                                                         //PE4 
#define KEY3  	PEin(5)		                                                                         //P35

#define KEY1_PRES                 1
#define KEY2_PRES                 2
#define KEY3_PRES                 3



void Key_Init(void);
u8   Key_Scan(void);



#endif /* __LED_H */

