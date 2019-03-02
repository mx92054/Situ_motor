/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   led应用函数接口
  */
  
#include "bsp_led.h"   


extern short wReg[] ;

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void LED_GPIO_Config(void)
{
	int n ;
	
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(LED1_GPIO_CLK | DO_GPIO_CLK | DI_GPIO_CLK, ENABLE);
  
	//AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	
	// JTAG-DP禁用  SW-DP使能
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	
//--------------Lamp-------------------------------------
	GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED1_GPIO_PORT,&GPIO_InitStructure);
	
//--------------Digital Output-------------------------------
	GPIO_InitStructure.GPIO_Pin = DO_GPIO_PINS ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(DO_GPIO_PORT,&GPIO_InitStructure);

	for(n = 0 ; n < 6 ; n++)
			GPIO_ResetBits(DO_GPIO_PORT, GPIO_Pin_7 >> n) ;
	
//--------------Digital Input_-------------------------------
	GPIO_InitStructure.GPIO_Pin = DI_GPIO_PINS ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(DI_GPIO_PORT,&GPIO_InitStructure);
}

 /**
  * @brief  输出寄存器值到外部
  * @param  None
  * @retval None
  */
void OutputDigital(void) 
{
	int n ;
	
	for(n = 0 ; n < 6 ; n++)
	{
		if ( wReg[10 + n] == 0 )
			GPIO_ResetBits(DO_GPIO_PORT, GPIO_Pin_7 >> n) ;
		else
			GPIO_SetBits(DO_GPIO_PORT, GPIO_Pin_7 >> n) ;		
	}
}

 /**
  * @brief  采集外部开关量信号，存入wReg寄存器中
  * @param  None
  * @retval None
  */
void InputDigital(void) 
{
	int n ;
	
	for(n = 0 ; n < 5 ; n++)
	{
		wReg[3 + n] = GPIO_ReadInputDataBit(DI_GPIO_PORT, GPIO_Pin_11 << n) ;
	}
}


//---------------------------------------------------------------------------------- 
void BKP_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE) ;
	PWR_BackupAccessCmd(ENABLE) ;
	BKP_ClearFlag() ;
}

//---------------------------------------------------------------------------------- 
void ReloadParameter(void)
{
	int i ;
	
	// Load default parameter from BKP area
	for( i = 100 ; i < 110 ; i++)
		wReg[i] = BKP_ReadBackupRegister(BKP_DR1 + (i-100)*4) ;
	for( i = 110 ; i < 120 ; i++)
		wReg[i] = BKP_ReadBackupRegister(BKP_DR11 + (i-110)*4) ;
	
	// Reboot counter 
	wReg[120] = BKP_ReadBackupRegister(BKP_DR21) + 1 ;
	BKP_WriteBackupRegister(BKP_DR20, wReg[120]) ;	
}

//---------------------------------------------------------------------------------- 
void SaveaBKPParameter(void)
{
	int i ;
	
	// Load default parameter from BKP area
	for( i = 0 ; i < 10 ; i++)
		BKP_WriteBackupRegister(BKP_DR1 + 4*i, wReg[100+i]) ;
	
	for( i = 0 ; i < 10 ; i++)
		BKP_WriteBackupRegister(BKP_DR10 + 4*i, wReg[110+i]) ;
}

//--------------------------------------------------------------
//      Watchdong config
//			cycle = load_val*Prescaler/40k
//---------------------------------------------------------------
void IWDG_Configuration(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 
    IWDG_SetPrescaler(IWDG_Prescaler_16); 
    IWDG_SetReload(0xFFF);    // 0xfff*16/40k = 1.6s
    IWDG_ReloadCounter(); 
    IWDG_Enable(); 
}

//--------------------------------------------------------------
//      feed dog function
//---------------------------------------------------------------
void IWDG_Feed(void)   
{
    IWDG->KR=0XAAAA;                                  
}


/*********************************************END OF FILE**********************/
