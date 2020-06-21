#include "ADC.h"
#include "bsp_SysTick.h"

u16 ADC_Converted_Buff[2];  //ADC数据缓存

void DMA_ADC_Init(void)
{
	DMA_InitTypeDef    DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA1时钟使能 
	DMA_DeInit(DMA2_Stream0);
	while(DMA_GetCmdStatus(DMA2_Stream0)!=DISABLE);//等待DMA可以配置

  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  //通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&ADC1->DR;//(u32)&ADC1->DR;//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_Converted_Buff;//DMA 内存基地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//外设到存储器模式
  DMA_InitStructure.DMA_BufferSize = 2;//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器非增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据长度:16位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;//存储器数据长度:16位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// 工作在循环缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);//初始化DMA Stream
 
	DMA_Cmd(DMA2_Stream0,ENABLE);                           //打开DMA传输
}


void  ADC_DMA_Init(void)
{
  GPIO_InitTypeDef  		GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);			//使能GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);			//使能GPIOC时钟
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);			//使能ADC1时钟

	//先初始化IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;							//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;					//下拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);										//初始化  
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;							//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;					//下拉
  GPIO_Init(GPIOC, &GPIO_InitStructure);										//初始化  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);				//ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);			//复位结束	 
 
 
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;	//独立模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1; //DMA失能，每完成一次转换产生一个DMA请求
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; 
  ADC_CommonInit(&ADC_CommonInitStructure);
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;								//12位模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;													//多通道扫描模式	
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;										//连续转换模式
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;								//右对齐	
  ADC_InitStructure.ADC_NbrOfConversion = 2;														//2个转换在规则序列中 也就是只转换规则序列2 
  ADC_Init(ADC1, &ADC_InitStructure);
	 
	ADC_Cmd(ADC1, ENABLE);//开启AD转换器	 
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 2, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    

	DMA_ADC_Init();//DMA ADC1 初始化
	ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能		
	
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);//数据源变换时开启DMA传输
	ADC_DMACmd(ADC1, ENABLE);       //ADC1 DMA传送数据打开		
}

/*
********************************************************************************************************************
*                  u16 Get_Vol(u16 AdcVal)
*
*Description :得到电压值
*Arguments   :AdcVal:  ADC的采样值
*Returns     :大爪子重量
*Notes       : none
********************************************************************************************************************
*/
float Get_Weight(u16 AdcVal)
{
//	static float vol;

//	vol=0.98f*vol+0.02f*AdcVal;
//	
//	return (vol*0.013293457f+1.5f);
	return 1;
}







