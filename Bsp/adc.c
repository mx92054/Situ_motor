/*********************************************************************************
 * 文件名  ：adc.c
**********************************************************************************/
#include "adc.h"
#include "bsp_led.h"

#include <string.h>

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

__IO uint16_t ADC_ConvertedValue[CYCLE_LEN*3];
uint16_t 			wGraph[CYCLE_LEN*3] ;
int 					bEffectHandle = 0 ;

extern short wReg[] ;
/*
 * 函数名：ADC1_GPIO_Config
 * 描述  ：使能ADC1和DMA1的时钟，初始化PA.1 PA.2 PA.3
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	
	RCC_APB2PeriphClockCmd(AI1_GPIO_CLK,ENABLE) ;
	/* Configure PA4  as analog input */
	GPIO_InitStructure.GPIO_Pin = AI1_GPIO_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(AI1_GPIO_PORT, &GPIO_InitStructure);				// AI1
	
	RCC_APB2PeriphClockCmd(AI2_GPIO_CLK,ENABLE) ;
	/* Configure PA5  as analog input */
	GPIO_InitStructure.GPIO_Pin = AI2_GPIO_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(AI2_GPIO_PORT, &GPIO_InitStructure);				// AI2	
	
	RCC_APB2PeriphClockCmd(AI3_GPIO_CLK,ENABLE) ;
	/* Configure PA6 as analog input */
	GPIO_InitStructure.GPIO_Pin = AI3_GPIO_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(AI3_GPIO_PORT, &GPIO_InitStructure);				// AI3	
}


/* 函数名：ADC1_Mode_Config
 * 描述  ：配置ADC1的工作模式为MDA模式
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 //ADC地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = CYCLE_LEN*3;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	
	/* Enable DMA Intrupet */
 	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	DMA_ClearITPendingBit(DMA_IT_TC);	
	
	/* Enable DMA channel1 */	
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	/* ADC1 configuration */	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//独立ADC模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 	 		//开启扫描模式，扫描模式用于多通道采集
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//开启连续转换模式，即不停地进行ADC转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//采集数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 3;	 								//要转换的通道数目3
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/*配置ADC时钟，为PCLK2的8分频，即9MHz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	/*配置ADC1的通道11为55.	5个采样周期，序列为1 */ 
	ADC_RegularChannelConfig(ADC1, AI1_CHANNEL, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, AI2_CHANNEL, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, AI3_CHANNEL, 3, ADC_SampleTime_239Cycles5);
	
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/*复位校准寄存器 */   
	ADC_ResetCalibration(ADC1);
	/*等待校准寄存器复位完成 */
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/* ADC校准 */
	ADC_StartCalibration(ADC1);
	/* 等待校准完成*/
	while(ADC_GetCalibrationStatus(ADC1));
	
	/* 由于没有采用外部触发，所以使用软件触发ADC转换 */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
 * 函数名：DMA_NVIC_Configuration
 * 描述  ：DMA中断配置
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
static void DMA_NVIC_Configuration(void)
{
	 NVIC_InitTypeDef NVIC_InitStructure; 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
}

/*
 * 函数名：ADC1_Init
 * 描述  ：无
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void ADC1_Init(void)
{
	DMA_NVIC_Configuration() ;
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}

/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA1 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{	
	if(DMA_GetITStatus(DMA_IT_TC) == SET)
	{
		DMA_ClearITPendingBit(DMA_IT_TC);
		if ( bEffectHandle == 0 )
		{
			memcpy((void*)wGraph,(void*)ADC_ConvertedValue,CYCLE_LEN*3*2) ;
			bEffectHandle = 1 ;
			wReg[16]++ ;
		}
	} 	
}

/************************END OF FILE************/

