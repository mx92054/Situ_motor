/***************************************************
 * Copyright (C),2019 www.idsse.ac.cn
 * Written by chenming
 * Version 1.0
 * Data  2019-3-3
 * Description: 主程序
 * *************************************************/
#include "stm32f10x.h"

#include "..\bsp\bsp_led.h"
#include "..\bsp\SysTick.h"
#include "..\bsp\bsp_exti.h"
#include "..\bsp\bsp_innerflash.h"
#include "..\bsp\adc.h"

#include "modbus_svr.h"
#include "SBG_Comm.h"

#define BIT_VALUE(val, bit) ((val & (0x0001 << bit)) >> bit)
#define STAND_NUM 16

#define AMP_START_ADR 0
#define AMP_ACT_ADR 40

short wReg[200];
short coils[200];

short Zero[3] = {17, 60, 19};
int nStandard[4][STAND_NUM] = {
	{0, 138, 677, 1150, 1655, 2173, 2529, 3086, 3707, 4083, 4695, 5100, 5612, 6106, 6575, 7080},
	{0, 44, 214, 370, 524, 690, 801, 988, 1129, 1192, 1274, 1320, 1370, 1417, 1483, 1509},
	{0, 43, 212, 368, 521, 686, 797, 988, 1125, 1187, 1270, 1316, 1366, 1410, 1475, 1502},
	{0, 44, 212, 367, 519, 682, 794, 977, 1114, 1176, 1257, 1302, 1351, 1394, 1421, 1437}};

static void Handle_Effect(void);

//--------------------------------------------------------------------------------
int main(void)
{
	SysTick_Init();

	LED_GPIO_Config();
	Modbus_init();
	SBG_Init();
	ADC1_Init();
	LED1_ON;

	SetTimer(1, 200);
	SetTimer(2, 500);
	SetTimer(4, 200);
	SetTimer(3, 1000);

	IWDG_Configuration();

	while (1)
	{
		Modbus_task();
		SBG_Task();

		//-----------------------------------------------------------------
		if (GetTimer(1))
		{
			//OutputDigital();
			//InputDigital();
		}

		//------------------------------------------------------------------
		if (GetTimer(2))
		{
			LED1_TOGGLE;
			IWDG_Feed();
		}

		if (GetTimer(3))
		{
			Modbus_SavePara();
		}

		if (GetTimer(4) && bEffectHandle)
		{
			Handle_Effect();
			bEffectHandle = 0;
		}
	}
}

//--------------------------------------------------------------
//        有效值计算程序
//---------------------------------------------------------------
//wReg[20] - Middle Value of voltage
static void Handle_Effect(void)
{
	uint16_t i, j;
	uint16_t *pData, Temp;
	uint32_t lEff[3] = {0, 0, 0}; //Effective value : sum of square
	int nDeltaDig = 1;
	int nDeltaAmp = 0;
	int nLastDig = 0;
	int nVal = 0;
	short *ptrVal, *ptrAmp, tmp;

	pData = wGraph;
	for (i = 0; i < CYCLE_LEN; i++)
	{
		for (j = 0; j < 3; j++)
		{
			Temp = *pData++;
			lEff[j] += Temp;
		}
	}

	ptrVal = mblock1.ptrRegs + AMP_ACT_ADR;
	ptrAmp = mblock1.ptrRegs + AMP_START_ADR;
	for (i = 0; i < 3; i++)
	{
		tmp = lEff[i] / CYCLE_LEN - Zero[i]; //Effective value is square root
		if (tmp < 0)
			tmp = 0;

		for (j = 1; j < STAND_NUM; j++)
		{
			if (tmp < nStandard[i + 1][j] && tmp >= nStandard[i + 1][j - 1])
				break;
		}

		if (j >= STAND_NUM)
		{
			nDeltaDig = 1;
			nDeltaAmp = 0;
			nVal = nStandard[0][STAND_NUM - 1];
			nLastDig = nStandard[i + 1][STAND_NUM - 1];
		}
		else
		{
			nDeltaDig = nStandard[i + 1][j] - nStandard[i + 1][j - 1];
			nDeltaAmp = nStandard[0][j] - nStandard[0][j - 1];
			nVal = nStandard[0][j - 1];
			nLastDig = nStandard[i + 1][j - 1];
		}
		nVal += (tmp - nLastDig) * nDeltaAmp / nDeltaDig;
		*ptrAmp = (short)(nVal / 100);
		*ptrVal = tmp;
		ptrAmp++;
		ptrVal++;
	}
}
