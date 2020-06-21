#include "Data_type.h"   

//发送给 Small_CAR/Small_CLAW/  433传感器的指令
unsigned char send_request_to_xxx[2][6] = {{0x00,0x01,0x0A},//SMALL_CAR
																					 {0x00,0x01,0x0A},//SMALL_CLAW
																				   };
Laser laser;
MPU6050 mpu; 
UP_DATA Up_Data;
//POSITION origin;//起始位置
//POSITION target;//目标位置 
/*********************************************END OF FILE**********************/
