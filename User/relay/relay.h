#ifndef __RELAY_H
#define	__RELAY_H

#include "stm32f4xx.h"
#include "bsp_SysTick.h"
//Òý½Å¶¨Òå
/*******************************************************/


/************************************************************/
//#define car_east        PBout(12)    
//#define car_west        PBout(13)    
//#define car_south       PBout(14) 
//#define car_north       PBout(15) 
//#define paw_up          PDout(13)    
//#define paw_down        PDout(14)    
//#define paw_close       PDout(15) 
//#define paw_release     PEout(8) 
//#define car_start       PEout(9) 
//#define car_stop        PEout(10) 


//#define RELAY_ON_car_east       (PBout(12)=0)
//#define RELAY_ON_car_west       (PBout(13)=0)
//#define RELAY_ON_car_south      (PBout(14)=0)
//#define RELAY_ON_car_north      (PBout(15)=0)
//#define RELAY_ON_paw_up         (PDout(13)=0)
//#define RELAY_ON_paw_down       (PDout(14)=0)
//#define RELAY_ON_paw_close      (PDout(15)=0)
//#define RELAY_ON_paw_release    (PEout(8)=0)
//#define RELAY_ON_car_start      (PEout(9)=0)
//#define RELAY_ON_car_stop       (PEout(10)=0)


//#define RELAY_OFF_car_east      (PBout(12)=1)
//#define RELAY_OFF_car_west      (PBout(13)=1)
//#define RELAY_OFF_car_south     (PBout(14)=1)
//#define RELAY_OFF_car_north     (PBout(15)=1)
//#define RELAY_OFF_paw_up        (PDout(13)=1)
//#define RELAY_OFF_paw_down      (PDout(14)=1)
//#define RELAY_OFF_paw_close     (PDout(15)=1)
//#define RELAY_OFF_paw_release   (PEout(8)=1)
//#define RELAY_OFF_car_start     (PEout(9)=1)
//#define RELAY_OFF_car_stop      (PEout(10)=1)

#define CAR_EAST_PIN                    GPIO_Pin_8                 
#define CAR_EAST_GPIO_PORT              GPIOE  

#define CAR_WEST_PIN                    GPIO_Pin_9                 
#define CAR_WEST_GPIO_PORT              GPIOE  

#define CAR_SOUTH_PIN                   GPIO_Pin_10                 
#define CAR_SOUTH_GPIO_PORT             GPIOE  

#define CAR_NORTH_PIN                   GPIO_Pin_11                 
#define CAR_NORTH_GPIO_PORT             GPIOE  

#define PAW_UP_PIN                      GPIO_Pin_12               
#define PAW_UP_GPIO_PORT                GPIOB  

#define PAW_DOWN_PIN                    GPIO_Pin_13                 
#define PAW_DOWN_GPIO_PORT              GPIOB  

#define PAW_RELEASE_PIN                 GPIO_Pin_14                
#define PAW_RELEASE_GPIO_PORT           GPIOB  

#define PAW_CLOSE_PIN                   GPIO_Pin_15                 
#define PAW_CLOSE_GPIO_PORT             GPIOB 

#define CAR_START_PIN                   GPIO_Pin_8                 
#define CAR_START_GPIO_PORT             GPIOD  

#define CAR_STOP_PIN                    GPIO_Pin_9                
#define CAR_STOP_GPIO_PORT              GPIOD  

#define ON      1
#define OFF     0

#define CAR_EAST(a)	if (a)	\
					GPIO_ResetBits(CAR_EAST_GPIO_PORT,CAR_EAST_PIN);\
					else		\
					GPIO_SetBits(CAR_EAST_GPIO_PORT,CAR_EAST_PIN)
					
#define CAR_WEST(a)	if (a)	\
					GPIO_ResetBits(CAR_WEST_GPIO_PORT,CAR_WEST_PIN);\
					else		\
					GPIO_SetBits(CAR_WEST_GPIO_PORT,CAR_WEST_PIN)
					
#define CAR_SOUTH(a)	if (a)	\
					GPIO_ResetBits(CAR_SOUTH_GPIO_PORT,CAR_SOUTH_PIN);\
					else		\
					GPIO_SetBits(CAR_SOUTH_GPIO_PORT,CAR_SOUTH_PIN)			

#define CAR_NORTH(a)	if (a)	\
					GPIO_ResetBits(CAR_NORTH_GPIO_PORT,CAR_NORTH_PIN);\
					else		\
					GPIO_SetBits(CAR_NORTH_GPIO_PORT,CAR_NORTH_PIN)			

#define PAW_UP(a)	if (a)	\
					GPIO_ResetBits(PAW_UP_GPIO_PORT,PAW_UP_PIN);\
					else		\
					GPIO_SetBits(PAW_UP_GPIO_PORT,PAW_UP_PIN)			

#define PAW_DOWN(a)	if (a)	\
					GPIO_ResetBits(PAW_DOWN_GPIO_PORT,PAW_DOWN_PIN);\
					else		\
					GPIO_SetBits(PAW_DOWN_GPIO_PORT,PAW_DOWN_PIN)		

#define PAW_CLOSE(a)	if (a)	\
					GPIO_ResetBits(PAW_CLOSE_GPIO_PORT,PAW_CLOSE_PIN);\
					else		\
					GPIO_SetBits(PAW_CLOSE_GPIO_PORT,PAW_CLOSE_PIN)				

#define PAW_RELEASE(a)	if (a)	\
					GPIO_ResetBits(PAW_RELEASE_GPIO_PORT,PAW_RELEASE_PIN);\
					else		\
					GPIO_SetBits(PAW_RELEASE_GPIO_PORT,PAW_RELEASE_PIN)				

#define CAR_START(a)	if (a)	\
					GPIO_ResetBits(CAR_START_GPIO_PORT,CAR_START_PIN);\
					else		\
					GPIO_SetBits(CAR_START_GPIO_PORT,CAR_START_PIN)		

#define CAR_STOP(a)	if (a)	\
					GPIO_ResetBits(CAR_STOP_GPIO_PORT,CAR_STOP_PIN);\
					else		\
					GPIO_SetBits(CAR_STOP_GPIO_PORT,CAR_STOP_PIN)		

extern int  RelayOnflag;

extern int  RelayOffflag;
					
void Relay_GPIO_Init(void);
void RelayOn(void);
void RelayOff(void);
void PowerOn(void);
void PowerOff(void);					
#endif 
