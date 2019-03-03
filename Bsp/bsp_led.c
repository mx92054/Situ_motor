/***************************************************
 * Copyright (C),2019 www.idsse.ac.cn
 * Written by chenming
 * Version 1.0
 * Data  2019-3-3
 * Description: DI/DO实现程序
 * *************************************************/
#include "bsp_led.h"
#include "..\User\Modbus_svr.h"

/**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void LED_GPIO_Config(void)
{
	int n;

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LED1_GPIO_CLK | DO_GPIO_CLK | DI_GPIO_CLK, ENABLE);

	//AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// JTAG-DP禁用  SW-DP使能
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	//--------------Lamp-------------------------------------
	GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);

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
	int n;
	short *ptrReg;

	ptrReg = mblock1.ptrRegs + GPIO_OUT_ADR;

	for (n = 0; n < 6; n++)
	{
		if (*ptrReg++ == 0)
			GPIO_ResetBits(DO_GPIO_PORT, GPIO_Pin_7 >> n);
		else
			GPIO_SetBits(DO_GPIO_PORT, GPIO_Pin_7 >> n);
	}
}

/**
  * @brief  采集外部开关量信号，存入wReg寄存器中
  * @param  None
  * @retval None
  */
void InputDigital(void)
{
	int n;
	short *ptrReg;

	ptrReg = mblock1.ptrRegs + GPIO_IN_ADR;

	for (n = 0; n < 5; n++)
	{
		*ptrReg++ = GPIO_ReadInputDataBit(DI_GPIO_PORT, GPIO_Pin_11 << n);
	}
}

//--------------------------------------------------------------
//      Watchdong config
//			cycle = load_val*Prescaler/40k
//---------------------------------------------------------------
void IWDG_Configuration(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_16);
	IWDG_SetReload(0xFFF); // 0xfff*16/40k = 1.6s
	IWDG_ReloadCounter();
	IWDG_Enable();
}

//--------------------------------------------------------------
//      feed dog function
//---------------------------------------------------------------
void IWDG_Feed(void)
{
	IWDG->KR = 0XAAAA;
}

/*********************************************END OF FILE**********************/
