/********************************************************************************************
   串口2用于与小爪通讯
*********************************************************************************************/
#include "usart2.h"
#include "bsp_led.h"
#include "Data_type.h"

#define U2_BUFFSIZERECE  1000
#define U2_BUFFSIZESEND  100

char u2_receive_buff[U2_BUFFSIZERECE] = {0};

#define USARTx                           USART2
#define USARTx_CLK                       RCC_APB1Periph_USART2
#define USARTx_CLK_INIT                  RCC_APB1PeriphClockCmd
#define USARTx_IRQn                      USART2_IRQn
#define USARTx_IRQHandler                USART2_IRQHandler

#define USARTx_TX_PIN                    GPIO_Pin_2              
#define USARTx_TX_GPIO_PORT              GPIOA                       
#define USARTx_TX_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define USARTx_TX_SOURCE                 GPIO_PinSource2
#define USARTx_TX_AF                     GPIO_AF_USART2

#define USARTx_RX_PIN                    GPIO_Pin_3         
#define USARTx_RX_GPIO_PORT              GPIOA                    
#define USARTx_RX_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define USARTx_RX_SOURCE                 GPIO_PinSource3
#define USARTx_RX_AF                     GPIO_AF_USART2

#define USARTx_DMA                       DMA1
#define USARTx_DMAx_CLK                  RCC_AHB1Periph_DMA1
#define DMACLEAR                         USARTx_DMA->HIFCR = DMA_Stream6_IT_MASK


#define USART_RX_DMA 							       DMA1_Stream5
#define USART_RX_DMA_Channel   					 DMA_Channel_4
#define USART_RX_DMA_PeripheralBaseAddr  		(uint32_t) (&(USARTx->DR))

#define USART_TX_DMA 							       DMA1_Stream6
#define USART_TX_DMA_Channel   					 DMA_Channel_4
#define USART_TX_DMA_PeripheralBaseAddr  		(uint32_t) (&(USARTx->DR))



static void bsp_initUSART(u32 bound);
static void USART_DMA_RxConfig(void);
static void USART_RX_DMAReset(void);
static void USART_DMA_Tx_init(uint32_t *BufferSRC, uint32_t BufferSize);

void USART2_Init(u32 bound)
{
	bsp_initUSART(bound);
}

static void bsp_initUSART(u32 bound)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(USARTx_RX_GPIO_CLK, ENABLE);//使能GPIOA时钟
//	RCC_AHB1PeriphClockCmd(USARTx_TX_GPIO_CLK|USARTx_RX_GPIO_CLK, ENABLE);//使能GPIOA时钟
	USARTx_CLK_INIT(USARTx_CLK, ENABLE);

	RCC_AHB1PeriphClockCmd(USARTx_DMAx_CLK, ENABLE);  //使能DMA1时钟

	//串口2对应引脚复用映射
//	GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);
	GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);

	 //USART端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
/*
	GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
	GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);
*/
	GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN; 
	GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);

  //USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx;
	USART_Init(USARTx, &USART_InitStructure);
	
	USART_DMA_RxConfig();   //配置DMA
	USART_DMA_Tx_init(0,10);   //配置发送DMA
	USART_DMACmd(USARTx, USART_DMAReq_Rx, ENABLE);

//	USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);
	DMA_ITConfig(USART_TX_DMA, DMA_IT_TC, ENABLE);  //开启相关中断	

   //USART NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;//串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  	//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;         	//子优先级0  第一时间响应
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);//根据指定的参数初始化VIC寄存器、

	USART_ITConfig(USARTx,USART_IT_IDLE,ENABLE);
	USART_ITConfig(USARTx,USART_IT_TC,ENABLE);
	USART_Cmd(USARTx, ENABLE);
	USART_ClearFlag(USARTx, USART_FLAG_TC);
		
}



static void USART_DMA_RxConfig(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
	DMA_DeInit(USART_RX_DMA);
	DMA_Cmd(USART_RX_DMA, DISABLE); 
	while (DMA_GetCmdStatus(USART_RX_DMA) != DISABLE)
	{
	}
	
	DMA_InitStructure.DMA_BufferSize = U2_BUFFSIZERECE;              
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; 
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;   
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;       
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                 
	DMA_InitStructure.DMA_PeripheralBaseAddr =USART_RX_DMA_PeripheralBaseAddr ; 
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;     
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                  
	

	DMA_InitStructure.DMA_Channel = USART_RX_DMA_Channel ;    
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;    
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)u2_receive_buff ;
	DMA_Init(USART_RX_DMA,&DMA_InitStructure);
	
	DMA_Cmd(USART_RX_DMA, ENABLE); //开启DMA传输
}


#define TRANSFER_IT_ENABLE_MASK (uint32_t)(DMA_SxCR_TCIE | DMA_SxCR_HTIE | \
                                           DMA_SxCR_TEIE | DMA_SxCR_DMEIE)

#define DMA_Stream0_IT_MASK     (uint32_t)(DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 | \
                                           DMA_LISR_TEIF0 | DMA_LISR_HTIF0 | \
                                           DMA_LISR_TCIF0)

#define DMA_Stream1_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK << 6)
#define DMA_Stream2_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK << 16)
#define DMA_Stream3_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK << 22)
#define DMA_Stream4_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK | (uint32_t)0x20000000)
#define DMA_Stream5_IT_MASK     (uint32_t)(DMA_Stream1_IT_MASK | (uint32_t)0x20000000)
#define DMA_Stream6_IT_MASK     (uint32_t)(DMA_Stream2_IT_MASK | (uint32_t)0x20000000)
#define DMA_Stream7_IT_MASK     (uint32_t)(DMA_Stream3_IT_MASK | (uint32_t)0x20000000)


static void USART_DMA_Tx_init(uint32_t *BufferSRC, uint32_t BufferSize)
{
	DMA_InitTypeDef  DMA_InitStructure;
	

	DMA_DeInit(USART_TX_DMA);	
	while (DMA_GetCmdStatus(USART_TX_DMA) != DISABLE)
	{
	}
	DMA_InitStructure.DMA_BufferSize = BufferSize;/* 配置DMA大小 */
 	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;                  /* 在这个程序里面使能或者禁止都可以的 */
 	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;          /* 设置阀值 */
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;           /* 设置内存为单字节突发模式 */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;        /* 设置内存数据的位宽是字节 */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                /* 使能地址自增 */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                          /* 设置DMA是正常模式 */
	DMA_InitStructure.DMA_PeripheralBaseAddr =USART_TX_DMA_PeripheralBaseAddr; /* 设置外设地址 */
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    /* 设置外设为单字节突发模式 */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;; /* 设置外设数据的位宽是字节 */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/* 禁止外设地址自增 */
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;             /* 设置优先级 */
	
	/* 配置 TX DMA */
	DMA_InitStructure.DMA_Channel = USART_TX_DMA_Channel ;      /* 配置发送通道 */
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;     /* 设置从内存到外设 */
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)BufferSRC ; /* 设置内存地址 */
	DMA_Init(USART_TX_DMA,&DMA_InitStructure);
	
}

void USART2_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize)
{
		
	while (DMA_GetCmdStatus(USART_TX_DMA) != DISABLE)
	{
	}
	
	USART_TX_DMA->NDTR = BufferSize;
	USART_TX_DMA->M0AR = (uint32_t)BufferSRC;

	DMA_Cmd(USART_TX_DMA, ENABLE); 
	
	
}

static void USART_RX_DMAReset(void)
{ 
	DMA_Cmd(USART_RX_DMA, DISABLE);   
	while (DMA_GetCmdStatus(USART_RX_DMA) != DISABLE)
	{
	}
 	DMA_SetCurrDataCounter(USART_RX_DMA,U2_BUFFSIZERECE);//DMA通道的DMA缓存的大小
 	DMA_Cmd(USART_RX_DMA, ENABLE); 
}	
int sum1;
void USARTx_IRQHandler(void)
{	
	if (USART_GetITStatus(USARTx, USART_IT_IDLE) == SET)       
	{
		 USART_ReceiveData(USARTx);                       
		 uint8_t Uart2_Rec_Len = U2_BUFFSIZERECE - DMA_GetCurrDataCounter(USART_RX_DMA);			
	   USART_RX_DMAReset();
		 //数据帧处理
		 if(14==Uart2_Rec_Len)//SMALL_CLAW
		 {
				if(0xAA == u2_receive_buff[0])
				{
					for(uint8_t i=0;i<13;i++)
					{
						sum1 += u2_receive_buff[i];
					}
					if(u2_receive_buff[13] == sum1)
					{
						mpu.acc_z     = ((short)(u2_receive_buff[2]<<8| u2_receive_buff[1]))/32767.0*16;
						mpu.gyro_z    = ((short)(u2_receive_buff[4]<<8| u2_receive_buff[3]))/32767.0*2000;
						mpu.angle_x   = ((short)(u2_receive_buff[6]<<8| u2_receive_buff[5]))/32767.0*180;
						mpu.angle_y   = ((short)(u2_receive_buff[8]<<8| u2_receive_buff[7]))/32767.0*180;
						mpu.angle_z   = ((short)(u2_receive_buff[10]<<8| u2_receive_buff[9]))/32767.0*180;  
						laser.sampleval1 = (u2_receive_buff[12]<<8|u2_receive_buff[11]);
						laser.dis1       =  5.0f*((laser.sampleval1*3300.0f)/4096.0f)-3000.0f;   //下
						Up_Data.P_z = (int)laser.dis1;
						Up_Data.A_x = (int16_t)mpu.angle_x;
						Up_Data.A_y = (int16_t)mpu.angle_y;
					}
					sum1=0;					
				}
		 }
		 //直接在中断中接收处理数据 		
        				
	}else if(USART_GetITStatus(USARTx, USART_IT_TC) == SET)
	{
		USART_ClearITPendingBit(USARTx, USART_IT_TC);         //清除中断标志
		DMACLEAR;
		DMA_Cmd(USART_TX_DMA, DISABLE); 
	}
}
//向433发送请求指令
void uart2_tx_task(unsigned char send_date[],uint8_t uart_cmd)
{

	send_date[3] = 0xAA;//请求数据
	send_date[4] = uart_cmd;
	send_date[5] = send_date[4] + send_date[3];
	// printf("send_date[0]= %d\n",send_date[0]);  

	USART2_DMA_TxConfig((u32*)send_date,6);
}

//请求小爪433停止发送指令
void RequestStopToSmallClaw(void)
{
	for ( int i = 0; i < 5; i++)
	{
			uart2_tx_task(send_request_to_xxx[SMALL_CLAW],0);
			delay_ms(200);		
	}	
}

//请求小爪433开始发送指令
void RequestStartToSmallClaw(void)
{
	for (int i = 0; i < 5; i++)
	{
			uart2_tx_task(send_request_to_xxx[SMALL_CLAW],1);
			delay_ms(200);		
	}	
}
#define BYTE0(dwTemp)       (*(char *)(&dwTemp))      
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
void USART2_Upper_f_Computer(float data1,float data2,float data3,float data4)
{
	u8 i,sum;
	static u8 Send_DATA[21];       
	Send_DATA[0] =0xAA;    
	Send_DATA[1] =0xAA;			
	Send_DATA[2] =0xF1;    
	Send_DATA[3] =16;      
	
	Send_DATA[4]  = BYTE3(data1);
	Send_DATA[5]  = BYTE2(data1);
	Send_DATA[6]  = BYTE1(data1);
	Send_DATA[7]  = BYTE0(data1);
	
	Send_DATA[8]  = BYTE3(data2);
	Send_DATA[9]  = BYTE2(data2);
	Send_DATA[10] = BYTE1(data2);
	Send_DATA[11] = BYTE0(data2);
	
	Send_DATA[12] = BYTE3(data3);
	Send_DATA[13] = BYTE2(data3);
	Send_DATA[14] = BYTE1(data3);
	Send_DATA[15] = BYTE0(data3);
	
	Send_DATA[16] = BYTE3(data4);
	Send_DATA[17] = BYTE2(data4);
	Send_DATA[18] = BYTE1(data4);
	Send_DATA[19] = BYTE0(data4);
	
	sum = 0;
	for(i=0;i<20;i++)
		sum += Send_DATA[i];
	
	Send_DATA[20]=sum;
	USART2_DMA_TxConfig((u32*)Send_DATA,21);
}

