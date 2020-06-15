/********************************************************************************************
   串口1用于与小车通讯
*********************************************************************************************/
#include "usart1.h"
#include "Data_type.h"
#include "bsp_led.h" 
#define U1_BUFFSIZERECE  100
#define U1_BUFFSIZESEND  100

uint8_t u1_receive_buff[U1_BUFFSIZERECE] = {0};
uint8_t uart_cmd;
/*
***************************************************************
*  USART1
*  TX   PA9
*  RX   PA10
*  RE   PA8
***************************************************************
*/

#define USARTx                           USART1
#define USARTx_CLK                       RCC_APB2Periph_USART1
#define USARTx_CLK_INIT                  RCC_APB2PeriphClockCmd
#define USARTx_IRQn                      USART1_IRQn
#define USARTx_IRQHandler                USART1_IRQHandler

#define USARTx_TX_PIN                    GPIO_Pin_9            
#define USARTx_TX_GPIO_PORT              GPIOA                     
#define USARTx_TX_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define USARTx_TX_SOURCE                 GPIO_PinSource9
#define USARTx_TX_AF                     GPIO_AF_USART1

#define USARTx_RX_PIN                    GPIO_Pin_10        
#define USARTx_RX_GPIO_PORT              GPIOA                    
#define USARTx_RX_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define USARTx_RX_SOURCE                 GPIO_PinSource10
#define USARTx_RX_AF                     GPIO_AF_USART1

#define USARTx_DMA                       DMA2
#define USARTx_DMAx_CLK                  RCC_AHB1Periph_DMA2
#define DMACLEAR                         DMA2->HIFCR = DMA_Stream7_IT_MASK;



#define USART_RX_DMA 										DMA2_Stream5
#define USART_RX_DMA_Channel   					DMA_Channel_4
#define USART_RX_DMA_PeripheralBaseAddr  		(uint32_t) (&(USARTx->DR))

#define USART_TX_DMA 										DMA2_Stream7
#define USART_TX_DMA_Channel   					DMA_Channel_4
#define USART_TX_DMA_PeripheralBaseAddr  		(uint32_t) (&(USARTx->DR))


static void bsp_initUSART(u32 bound);
static void USART1_DMA_RxConfig(void);
static void USART_RX_DMAReset(void);
static void USART1_DMA_Tx_init(uint32_t *BufferSRC, uint32_t BufferSize);

void USART1_Init(u32 bound)
{
	bsp_initUSART(bound);
}

static void bsp_initUSART(u32 bound)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/*************************************初始化串口1 用于双机通讯*************************************/
	/* 使能 GPIO clock */
	RCC_AHB1PeriphClockCmd(USARTx_TX_GPIO_CLK|USARTx_RX_GPIO_CLK, ENABLE);

	/* 使能 USART clock */
	USARTx_CLK_INIT(USARTx_CLK, ENABLE);

	/* 使能  DMA clock */
	RCC_AHB1PeriphClockCmd(USARTx_DMAx_CLK, ENABLE);

	/* USART1 GPIO configuration -----------------------------------------------*/ 
	/* Connect USART pins to AF7 */
	GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);
	GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);

	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
	GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN; 
	GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	/* When using Parity the word length must be configured to 9 bits */
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USARTx, &USART_InitStructure);
	
	USART1_DMA_RxConfig();                                                                       //配置DMA
	USART1_DMA_Tx_init(0,10);                                                                    //配置发送DMA
	/* 使能 USART DMA RX 请求 */
	USART_DMACmd(USARTx, USART_DMAReq_Rx, ENABLE);

	/* 使能 USART DMA TX 请求 */
	USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);
	/* 使能发送传输完成中断 */
	DMA_ITConfig(USART_TX_DMA, DMA_IT_TC, ENABLE);  	
	
   //USART NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;                                           //串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  	                                //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         	                                //子优先级1  第一时间响应
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                             //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                                                             //根据指定的参数初始化VIC寄存器、

	//开启中断 
	USART_ITConfig(USARTx,USART_IT_IDLE,ENABLE);
	//TC中断，在DMA传输数据完成时，触发TC中断
	USART_ITConfig(USARTx,USART_IT_TC,ENABLE);
	/* Enable USART */
	USART_Cmd(USARTx, ENABLE);
	USART_ClearFlag(USARTx, USART_FLAG_TC);
	
	
}

static void USART1_DMA_RxConfig(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
	/* 
	  使能前检测DMA是否被禁止。
	  请注意，在使用相同数据流的多次传输时，这一步很有用：使能，然后禁止，然后使能
	  ..... 在这种情况下，只有在数据传输结束的时候，DMA的禁止才有效，在确保使能位被
	  硬件清除之前做确认是不可能的，如果DMA传输只做一次，这一步可以忽略。
	*/
	DMA_DeInit(USART_RX_DMA);
	DMA_Cmd(USART_RX_DMA, DISABLE); 
	while (DMA_GetCmdStatus(USART_RX_DMA) != DISABLE)
	{
	}
	
	DMA_InitStructure.DMA_BufferSize = U1_BUFFSIZERECE;                 /* 配置DMA大小 */
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;          /* 在这个程序里面使能或者禁止都可以的 */
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;  /* 设置阀值 */
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;   /* 设置内存为单字节突发模式 */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;/* 设置内存数据的位宽是字节 */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;        /* 使能地址自增 */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                  /* 设置DMA是正常模式 */
	DMA_InitStructure.DMA_PeripheralBaseAddr =USART_RX_DMA_PeripheralBaseAddr ;  /* 设置外设地址 */
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     /* 设置外设为单字节突发模式 */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; /* 设置外设数据的位宽是字节 */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;      /* 禁止外设地址自增 */
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                   /* 设置优先级 */
	
	/* 配置 RX DMA */
	DMA_InitStructure.DMA_Channel = USART_RX_DMA_Channel ;      /* 配置接收通道 */
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;     /* 设置从外设到内存 */
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)u1_receive_buff ; /* 设置内存地址 */
	DMA_Init(USART_RX_DMA,&DMA_InitStructure);

	/* 使能 DMA USART RX Stream */
	DMA_Cmd(USART_RX_DMA, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: USART_DMA_TxConfig
*	功能说明: 串口的DMA发送配置
*	形    参：BufferSRC   源地址指针
*             BufferSize  缓冲区大小
*	返 回 值: 无
*********************************************************************************************************
*/

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


static void USART1_DMA_Tx_init(uint32_t *BufferSRC, uint32_t BufferSize)
{
	DMA_InitTypeDef  DMA_InitStructure;
	

	/* 复位 DMA Stream 寄存器 (用于调试目的) */
 	/* DMA_DeInit(USARTx_TX_DMA_STREAM); */

    /* 
	  使能前检测DMA是否被禁止。
	  请注意，在使用相同数据流的多次传输时，这一步很有用：使能，然后禁止，然后使能
	  ..... 在这种情况下，只有在数据传输结束的时候，DMA的禁止才有效，在确保使能位被
	  硬件清除之前做确认是不可能的，如果DMA传输只做一次，这一步可以忽略。
	*/
	DMA_DeInit(USART_TX_DMA);	
	while (DMA_GetCmdStatus(USART_TX_DMA) != DISABLE)
	{
	}
	DMA_InitStructure.DMA_BufferSize = BufferSize;                                /* 配置DMA大小 */
 	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;                         /* 在这个程序里面使能或者禁止都可以的 */
 	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;                 /* 设置阀值 */
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;                  /* 设置内存为单字节突发模式 */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;               /* 设置内存数据的位宽是字节 */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       /* 使能地址自增 */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                 /* 设置DMA是正常模式 */
	DMA_InitStructure.DMA_PeripheralBaseAddr =USART_TX_DMA_PeripheralBaseAddr;    /* 设置外设地址 */
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;           /* 设置外设为单字节突发模式 */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;;      /* 设置外设数据的位宽是字节 */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              /* 禁止外设地址自增 */
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                           /* 设置优先级 */
	
	/* 配置 TX DMA */
	DMA_InitStructure.DMA_Channel = USART_TX_DMA_Channel ;      /* 配置发送通道 */
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;     /* 设置从内存到外设 */
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)BufferSRC ; /* 设置内存地址 */
	DMA_Init(USART_TX_DMA,&DMA_InitStructure);

	
	/* 使能发送传输完成中断 */
	//DMA_ITConfig(USART_TX_DMA, DMA_IT_TC, ENABLE);  		
	/* 使能 DMA USART TX Stream */
//	RS485SEND_1();
//	DMA_Cmd(DMA2_Stream6, ENABLE); 
	
	/* 
	   检测DMA Stream 是否被正确的使能.如果DMA的参数配置错误了，那么DMA Stream
	   的使能位会被硬件清除掉，从而使得传输停止比如FIFO的阀值配置错误
	*/
}

void USART1_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize)
{
	/* 复位 DMA Stream 寄存器 (用于调试目的) */
 	/* DMA_DeInit(USARTx_TX_DMA_STREAM); */

    /* 
	  使能前检测DMA是否被禁止。
	  请注意，在使用相同数据流的多次传输时，这一步很有用：使能，然后禁止，然后使能
	  ..... 在这种情况下，只有在数据传输结束的时候，DMA的禁止才有效，在确保使能位被
	  硬件清除之前做确认是不可能的，如果DMA传输只做一次，这一步可以忽略。
	*/
	//DMA_DeInit(DMA1_Stream0);
	//DMA1_Stream0->CR  = 0;		
	while (DMA_GetCmdStatus(USART_TX_DMA) != DISABLE)
	{
	}
	
	USART_TX_DMA->NDTR = BufferSize;
	USART_TX_DMA->M0AR = (uint32_t)BufferSRC;
	/* 使能发送传输完成中断 */
	//DMA_ITConfig(USART_TX_DMA, DMA_IT_TC, ENABLE);  		
	/* 使能 DMA USART TX Stream */
	DMA_Cmd(USART_TX_DMA, ENABLE); 
	
	/* 
	   检测DMA Stream 是否被正确的使能.如果DMA的参数配置错误了，那么DMA Stream
	   的使能位会被硬件清除掉，从而使得传输停止比如FIFO的阀值配置错误
	*/
	
}

static void USART_RX_DMAReset(void)
{ 
	DMA_Cmd(USART_RX_DMA, DISABLE);   
	while (DMA_GetCmdStatus(USART_RX_DMA) != DISABLE)
	{
	}
 	DMA_SetCurrDataCounter(USART_RX_DMA,U1_BUFFSIZERECE);//DMA通道的DMA缓存的大小
 	DMA_Cmd(USART_RX_DMA, ENABLE); 
}	

//进中断
u8 sum;
int len1,len2=0;
void USARTx_IRQHandler(void)
{	
	if (USART_GetITStatus(USARTx, USART_IT_IDLE) == SET)       //当接收到数据完成
	{
		 USART_ReceiveData(USARTx);                         //读取数据 注意：这句必须要，否则不能够清除中断标志位。
		 uint8_t Uart1_Rec_Len = U1_BUFFSIZERECE - DMA_GetCurrDataCounter(USART_RX_DMA);			//算出接本帧数据长度
		 len1 = Uart1_Rec_Len;
	   USART_RX_DMAReset();
		 //数据帧处理
		 if(6==Uart1_Rec_Len)
		 {
				if(0xEE == u1_receive_buff[0])//SMALL_CAR  小车是AA?
				{
					for(uint8_t i=0;i<5;i++)
					{
						sum += u1_receive_buff[i];
					}
					if(u1_receive_buff[5] == sum)
					{
						LED1_TOGGLE;
						laser.sampleval2 = (u1_receive_buff[2]<<8|u1_receive_buff[1]);
						laser.sampleval3 = (u1_receive_buff[4]<<8|u1_receive_buff[3]);	
						laser.dis2 = 10.0f*((laser.sampleval2*3300.0f)/4096.0f)-6000.0f+60.0f;//  小车X为2
						laser.dis3= 5.0f*((laser.sampleval3*3300.0f)/4096.0f)-3000.0f+60.0f;//   小车Y为3
						Up_Data.P_x = (int)laser.dis2;
						Up_Data.P_y = (int)laser.dis3;
					}
					sum=0;
				}
		 }
		 //直接在中断中接收处理数据 
	}
	else if(USART_GetITStatus(USARTx, USART_IT_TC) == SET)
	{
		USART_ClearITPendingBit(USARTx, USART_IT_TC);         //清除中断标志		
		DMACLEAR;
		DMA_Cmd(USART_TX_DMA, DISABLE); 
	}

}


//向433发送请求指令
void uart1_tx_task(unsigned char send_date[],uint8_t uart_cmd)
{

	send_date[3] = 0xAA;//请求数据
	send_date[4] = uart_cmd;
	send_date[5] = send_date[4] + send_date[3];
	// printf("send_date[0]= %d\n",send_date[0]);  

	USART1_DMA_TxConfig((u32*)send_date,6);
}


//请求小车433停止发送指令
void RequestStopToSmallCar(void)
{
	for ( int i = 0; i < 5; i++)
	{
			uart1_tx_task(send_request_to_xxx[SMALL_CAR],0);
			delay_ms(200);		
	}	
}

//请求小车433开始发送指令
void RequestStartToSmallCar(void)
{
	for (int i = 0; i < 5; i++)
	{
			uart1_tx_task(send_request_to_xxx[SMALL_CAR],1);
			delay_ms(200);		
	}
	
}

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))      
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

void USART1_Upper_f_Computer(float data1,float data2,float data3,float data4,float data5,float data6)
{
	u8 i,sum;
	static u8 Send_DATA[29];       
	Send_DATA[0] =0xAA;    
	Send_DATA[1] =0xAA;			
	Send_DATA[2] =0xF1;    
	Send_DATA[3] =24;      
	
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

	Send_DATA[20] = BYTE3(data5);
	Send_DATA[21] = BYTE2(data5);
	Send_DATA[22] = BYTE1(data5);
	Send_DATA[23] = BYTE0(data5);
	
	Send_DATA[24] = BYTE3(data6);
	Send_DATA[25] = BYTE2(data6);
	Send_DATA[26] = BYTE1(data6);
	Send_DATA[27] = BYTE0(data6);

	sum = 0;
	for(i=0;i<28;i++)
		sum += Send_DATA[i];
	
	Send_DATA[28]=sum;
	USART1_DMA_TxConfig((u32*)Send_DATA,29);
}



