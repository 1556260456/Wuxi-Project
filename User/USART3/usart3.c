#include "usart3.h"
#include "Data_type.h"
#include "Manual.h"
#include "LCFunc.h"
#include "relay.h"
#include "RS485.h"
#include "CRC.h"
#include "bsp_led.h" 

#define U3_BUFFSIZERECE  100
#define U3_BUFFSIZESEND  100

uint8_t u3_receive_buff[U3_BUFFSIZERECE] = {0};
uint8_t u3_send_buff[U3_BUFFSIZESEND] = {0};
extern int8_t IsExecute;//1:执行下一步,0：不执行下一步

/*
***************************************************************
*  USART3
*  TX   PB10
*  RX   PB11
*  RE   PA8
***************************************************************
*/

#define USARTx                           USART3
#define USARTx_CLK                       RCC_APB1Periph_USART3
#define USARTx_CLK_INIT                  RCC_APB1PeriphClockCmd
#define USARTx_IRQn                      USART3_IRQn
#define USARTx_IRQHandler                USART3_IRQHandler

#define USARTx_TX_PIN                    GPIO_Pin_10            
#define USARTx_TX_GPIO_PORT              GPIOB                      
#define USARTx_TX_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define USARTx_TX_SOURCE                 GPIO_PinSource10
#define USARTx_TX_AF                     GPIO_AF_USART3

#define USARTx_RX_PIN                    GPIO_Pin_11
#define USARTx_RX_GPIO_PORT              GPIOB
#define USARTx_RX_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define USARTx_RX_SOURCE                 GPIO_PinSource11
#define USARTx_RX_AF                     GPIO_AF_USART3

#define USARTx_DMA                       DMA1
#define USARTx_DMAx_CLK                  RCC_AHB1Periph_DMA1
#define DMACLEAR                         DMA1->LIFCR = DMA_Stream3_IT_MASK;


#define USART_RX_DMA 										DMA1_Stream1
#define USART_RX_DMA_Channel   					DMA_Channel_4
#define USART_RX_DMA_PeripheralBaseAddr  		(uint32_t) (&(USARTx->DR))

#define USART_TX_DMA 										DMA1_Stream3
#define USART_TX_DMA_Channel   					DMA_Channel_4
#define USART_TX_DMA_PeripheralBaseAddr  		(uint32_t) (&(USARTx->DR))


static void bsp_initUSART(u32 bound);
static void USART3_DMA_RxConfig(void);
static void USART3_RX_DMAReset(void);
static void USART3_DMA_Tx_init(uint32_t *BufferSRC, uint32_t BufferSize);

void USART3_Init(u32 bound)
{
	bsp_initUSART(bound);
}


static void bsp_initUSART(u32 bound)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(USARTx_TX_GPIO_CLK|USARTx_RX_GPIO_CLK, ENABLE);//使能GPIOA时钟

	USARTx_CLK_INIT(USARTx_CLK, ENABLE);

	RCC_AHB1PeriphClockCmd(USARTx_DMAx_CLK, ENABLE);  //使能DMA1时钟

	//串口4对应引脚复用映射
	GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);
	GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);

	 //USART端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
	GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN; 
	GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);

  //UART4 初始化设置
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_2;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USARTx, &USART_InitStructure);
	
	USART3_DMA_RxConfig();   //配置DMA
	USART3_DMA_Tx_init(0,10);   //配置发送DMA
	USART_DMACmd(USARTx, USART_DMAReq_Rx, ENABLE);

	USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);
	DMA_ITConfig(USART_TX_DMA, DMA_IT_TC, ENABLE);  //开启相关中断	

   //USART NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;//串口4中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  	//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         	//子优先级0  第一时间响应
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);//根据指定的参数初始化VIC寄存器、

	USART_ITConfig(USARTx,USART_IT_IDLE,ENABLE);
	USART_ITConfig(USARTx,USART_IT_TC,ENABLE);
	USART_Cmd(USARTx, ENABLE);
	USART_ClearFlag(USARTx, USART_FLAG_TC);
}

static void USART3_DMA_RxConfig(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
	DMA_DeInit(USART_RX_DMA);
	DMA_Cmd(USART_RX_DMA, DISABLE); 
	while (DMA_GetCmdStatus(USART_RX_DMA) != DISABLE)
	{
	}
	
	DMA_InitStructure.DMA_BufferSize = U3_BUFFSIZERECE;              
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
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)u3_receive_buff ;
	DMA_Init(USART_RX_DMA,&DMA_InitStructure);
	
	DMA_Cmd(USART_RX_DMA, ENABLE); //开启DMA传输
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


static void USART3_DMA_Tx_init(uint32_t *BufferSRC, uint32_t BufferSize)
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

void USART3_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize)
{
	
//	for(int i = 0;i < BufferSize;i++)
//	{
//		u3_send_buff[i] = BufferSRC[i];
//	}	
//	DMA_Cmd(USART_TX_DMA, DISABLE);
//	while (DMA_GetCmdStatus(USART_TX_DMA) != DISABLE)
//	{
//	}
//	DMA_SetCurrDataCounter(USART_TX_DMA,BufferSize);	
//	DMA_Cmd(USART_TX_DMA, ENABLE); 
	DMA_Cmd(USART_TX_DMA, DISABLE);
	while (DMA_GetCmdStatus(USART_TX_DMA) != DISABLE)
	{
	}
	
	USART_TX_DMA->NDTR = BufferSize;
	USART_TX_DMA->M0AR = (uint32_t)BufferSRC;

	DMA_Cmd(USART_TX_DMA, ENABLE); 
}
/*
void DMA1_Stream3_IRQHandler(void)
{
	//DMA transmission completed?
	if(DMA_GetFlagStatus(USART_TX_DMA,DMA_FLAG_TCIF3) != RESET)
	{
		//Clear flag
		DMA_ClearFlag(USART_TX_DMA, DMA_FLAG_TCIF3 | DMA_FLAG_FEIF3 | 
					  DMA_FLAG_DMEIF3 | DMA_FLAG_TEIF3 | DMA_FLAG_HTIF3);
		
		//Wait for USART1 to send complete flag TC is set
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TC));
		//Clear send complete flag
		USART_ClearFlag(USART3, USART_FLAG_TC); 
		
		RS485_DE = 0;
		RS485_RE = 1;
	}
}
*/
static void USART3_RX_DMAReset(void)
{ 
	
	DMA_Cmd(USART_RX_DMA, DISABLE);   
	while (DMA_GetCmdStatus(USART_RX_DMA) != DISABLE)
	{
	}
 	DMA_SetCurrDataCounter(USART_RX_DMA,U3_BUFFSIZERECE);//DMA通道的DMA缓存的大小
 	DMA_Cmd(USART_RX_DMA, ENABLE); 
}	

//进中断

extern uint8_t WaitFlag;
extern uint8_t Run_Mode;
extern uint8_t Up_Data_Flag;
void USARTx_IRQHandler(void)
{	
	u8 i=0;
  u16 CRC_NUM;
	//static int temp = 0;
	u8 valid_data[100];
	
	if (USART_GetITStatus(USARTx, USART_IT_IDLE) == SET)       //当接收到数据完成
	{
		 USART_ReceiveData(USARTx);                         //读取数据 注意：这句必须要，否则不能够清除中断标志位。
		 uint8_t Uart1_Rec_Len = U3_BUFFSIZERECE - DMA_GetCurrDataCounter(USART_RX_DMA);			//算出接本帧数据长度
	   USART3_RX_DMAReset();
		 //数据帧处理
		 if(0xAB == u3_receive_buff[0])//地址码
		 {
			 //CRC校验
			 for(int j = 3;j<u3_receive_buff[1]-2;j++)
					valid_data[i++] = u3_receive_buff[j];
			 CRC_NUM = (u3_receive_buff[u3_receive_buff[1]-1] << 8 | u3_receive_buff[u3_receive_buff[1]-2]);
			 
			 if(CRC_NUM == CRC16(valid_data,i))//CRC校验通过
			 {
				 #if Manual == 3
					if(1==(u3_receive_buff[2]&0x07))//监听,长时间运行状态
					{
						//WaitFlag = 0;
						Up_Data.Status &= 0xF8;
						Up_Data.Status |= 0x01;//返回该值，说明已成功接收到
						Up_Data_Flag = 1;	
					}
					else if(2==(u3_receive_buff[2]&0x07))//手动
					{
						if(RelayOnflag == -2)// 这里要打开遥控器才会等于-2
						{
							HTaskModeFlag = u3_receive_buff[2]>>3;   //
							if(HTaskModeFlag == 1 || HTaskModeFlag == 2)
							{
								WaitFlag = 1;
								Run_Mode = 1;
								target.x[1] = ((int)u3_receive_buff[14]<<16)|((int)u3_receive_buff[13]<<8)|(u3_receive_buff[12]);
								target.y[1] = ((int)u3_receive_buff[17]<<16)|((int)u3_receive_buff[16]<<8)|(u3_receive_buff[15]); 
							}
							else if((HTaskModeFlag == 3) || (HTaskModeFlag == 6) ){//上升下降的几种模式
//								target.uwbdis[0] = ((int)u3_receive_buff[20]<<16)|((int)u3_receive_buff[19]<<8)|(u3_receive_buff[18]);
//								target.z[0] = ((int)u3_receive_buff[11]<<16)|((int)u3_receive_buff[10]<<8)|(u3_receive_buff[9]);
								target.z[1] = ((int)u3_receive_buff[20]<<16)|((int)u3_receive_buff[19]<<8)|(u3_receive_buff[18]);
								WaitFlag = 1;
								Run_Mode = 1;	
							}
							else if(HTaskModeFlag == 9){//抓料
									WaitFlag = 1;
									Run_Mode = 1;	
							}
							else if(HTaskModeFlag == 10){//放料
									WaitFlag = 1;
									Run_Mode = 1;	
							}
							else if(HTaskModeFlag == 0){
									Up_Data.Status &= 0x87;//此时的状态值  无任务
							}
						}
						if(1 == (u3_receive_buff[21]&0x07)){//打开遥控器
							  WaitFlag = 1;
							  Run_Mode = 1;
							  HTaskModeFlag = 7;
						 }
						else if(2 == (u3_receive_buff[21]&0x07))//关闭遥控器
						{
								WaitFlag = 1;
								Run_Mode = 1;
								HTaskModeFlag = 8;
						}	
						 Up_Data.Status &= 0xF8;
						 Up_Data.Status |= 0x02;//返回该值，说明已成功接收到
						 Up_Data_Flag = 1;							
			 }
			else if(3==(u3_receive_buff[2]&0x07))//半自动
			{
				if((u3_receive_buff[21]&0x08)==0x08)
				{
				
					target.x[0] = ((int)u3_receive_buff[5]<<16)|((int)u3_receive_buff[4]<<8)|(u3_receive_buff[3]);
					target.y[0] = ((int)u3_receive_buff[8]<<15)|((int)u3_receive_buff[7]<<8)|(u3_receive_buff[6]);
					target.z[0] = ((int)u3_receive_buff[11]<<16)|((int)u3_receive_buff[10]<<8)|(u3_receive_buff[9]);
					target.x[1] = ((int)u3_receive_buff[14]<<16)|((int)u3_receive_buff[13]<<8)|(u3_receive_buff[12]);
					target.y[1] = ((int)u3_receive_buff[17]<<15)|((int)u3_receive_buff[16]<<8)|(u3_receive_buff[15]);
					target.z[1] = ((int)u3_receive_buff[20]<<16)|((int)u3_receive_buff[19]<<8)|(u3_receive_buff[18]);
					//target.uwbdis[0] = ((int)u3_receive_buff[20]<<16)|((int)u3_receive_buff[19]<<8)|(u3_receive_buff[18]);

					WaitFlag = 1;
					Run_Mode = 2;
				}
				
				if((u3_receive_buff[21]>>4)==0x01 && IsExecute == 0)  //这里21字节的意思？
					IsExecute = 1;//确认执行下一步
				
				Up_Data.Status &= 0xF8;
				Up_Data.Status |= 0x03;//返回该值，说明已成功接收到		
				Up_Data_Flag = 1;				
			}
			else if(4==(u3_receive_buff[2]&0x07))//全自动
			{
				target.x[0] = ((int)u3_receive_buff[5]<<16)|((int)u3_receive_buff[4]<<8)|(u3_receive_buff[3]);
				target.y[0] = ((int)u3_receive_buff[8]<<15)|((int)u3_receive_buff[7]<<8)|(u3_receive_buff[6]);
				target.z[0] = ((int)u3_receive_buff[11]<<16)|((int)u3_receive_buff[10]<<8)|(u3_receive_buff[9]);
				target.x[1] = ((int)u3_receive_buff[14]<<16)|((int)u3_receive_buff[13]<<8)|(u3_receive_buff[12]);
				target.y[1] = ((int)u3_receive_buff[17]<<15)|((int)u3_receive_buff[16]<<8)|(u3_receive_buff[15]);
				target.z[1] = ((int)u3_receive_buff[20]<<16)|((int)u3_receive_buff[19]<<8)|(u3_receive_buff[18]);

				WaitFlag = 1;
				Run_Mode = 3;		
				Up_Data.Status &= 0xF8;
				Up_Data.Status |= 0x04;//返回该值，说明已成功接收到
			}
		}				
		#endif
		
		 }
		}
		 //直接在中断中接收处理数据
	else if(USART_GetITStatus(USARTx, USART_IT_TC) == SET)
	{
		USART_ClearITPendingBit(USARTx, USART_IT_TC);         //清除中断标志
		
		DMACLEAR;
		DMA_Cmd(USART_TX_DMA, DISABLE);
		RS485_DE = 0;
		RS485_RE = 0;
	}

  }





