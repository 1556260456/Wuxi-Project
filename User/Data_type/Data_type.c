#include "Data_type.h"   

//���͸� Small_CAR/Small_CLAW/  433��������ָ��
unsigned char send_request_to_xxx[2][6] = {{0x00,0x01,0x0A},//SMALL_CAR
																					 {0x00,0x01,0x0A},//SMALL_CLAW
																				   };
Laser laser;
MPU6050 mpu; 
UP_DATA Up_Data;
//POSITION origin;//��ʼλ��
//POSITION target;//Ŀ��λ�� 
/*********************************************END OF FILE**********************/
