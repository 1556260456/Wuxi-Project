#ifndef __DATA_TYPE_H
#define	__DATA_TYPE_H

#include "stm32f4xx.h"

/*******************************************************
 *                     �궨��
 *******************************************************/
//�޷�
#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )
/*******************************************************
 *                     �ṹ�嶨��
 *******************************************************/
 //���㼤��
typedef struct{
	float dis1;   //Сצ��������ļ���
	float dis2;   //С�г�X
	float dis3;   //С�г�Y
	
	float last_dis1;
	float last_dis2;
	float last_dis3;
		
	float sampleval1;
	float sampleval2;
	float sampleval3;
	
}Laser;
extern Laser laser;

//mpu6050
typedef struct{
	float dis;
	float angle_x;
	float angle_y;
	float angle_z;
}MPU6050;
extern MPU6050 mpu;


//λ������
typedef struct{
    int x[2];
    int y[2];
    int z[2];
		int uwbdis[2];
}POSITION; 
extern POSITION origin;//��ʼλ��
extern POSITION target;//Ŀ��λ�� 

//���ظ���λ�����͵���������
typedef struct
{
	uint8_t       Add;     //��ַ��
	int           P_x;     //x����
	int           P_y;     //y����
	int           P_z;	   //z����
	int16_t       A_x;     //6050x��Ƕ�
	int16_t       A_y;     //6050y��Ƕ�
	uint8_t       Status;  //�г�����״̬
	uint8_t       HalfStep;//���Զ�״̬����Ҫ������²�����
}UP_DATA;
extern UP_DATA Up_Data;
typedef struct
{																						  
	u16 time_1ms;                             //
	u16 time_2ms;                             //
	u16	time_5ms;                             //
	u16 time_10ms; 														//
	u16	time_25ms;	                          //
	u16 time_100ms;                           //
	u16 time_200ms;                           //
	u16 time_1000ms;                          //
} Task_loop_t;
extern Task_loop_t task_tim;

//�豸��
enum Dev_Obj
{
	SMALL_CAR = 0,
	SMALL_CLAW,
	ALL_DEV
};



extern unsigned char send_request_to_xxx[2][6];
#endif
