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
#define AMP_FACTOR_ADR 31
#define AMP_ZERO_ADR 34
#define QUEUE_LEN 10

static void Handle_Effect(void);

//----------------------------------------------
typedef struct _queue
{
	int ptr;
	short val[QUEUE_LEN];
	int sum;
} queue;

//----------------------------------------------
void queue_init(queue *q)
{
	int i = QUEUE_LEN;
	q->ptr = 0;
	while (i--)
		q->val[i] = 0;
	q->sum = 0;
}

//----------------------------------------------
void queue_in(queue *q, short data)
{
	q->sum -= q->val[q->ptr];
	q->sum += data;
	q->val[q->ptr] = data;
	q->ptr = (q->ptr + 1) % QUEUE_LEN;
}

//----------------------------------------------
short queue_avg(queue *q)
{
	return (short)(q->sum / QUEUE_LEN);
}

queue qvol[3];
//----------------------------------------------------------------

//-----------------------------------------------------------------
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
	SetTimer(4, 20);
	SetTimer(3, 1000);

	IWDG_Configuration();
	queue_init(&qvol[0]);
	queue_init(&qvol[1]);
	queue_init(&qvol[2]);

	while (1)
	{
		Modbus_task();
		SBG_Task();

		//-----------------------------------------------------------------
		if (GetTimer(1))
		{
			OutputDigital();
			InputDigital();
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
	uint16_t *pData;
	uint32_t lEff[3] = {0, 0, 0}; //Effective value : sum of square
	int i, j, tmp;
	short *ptrVal, *ptrAmp, *ptrZero, *ptrFactor;

	pData = wGraph;
	for (i = 0; i < CYCLE_LEN; i++)
	{
		for (j = 0; j < 3; j++)
		{
			lEff[j] += *pData++;
		}
	}

	ptrVal = mblock1.ptrRegs + AMP_ACT_ADR;
	ptrAmp = mblock1.ptrRegs + AMP_START_ADR;
	ptrZero = mblock1.ptrRegs + AMP_ZERO_ADR;
	ptrFactor = mblock1.ptrRegs + AMP_FACTOR_ADR;

	for (i = 0; i < 3; i++)
	{
		tmp = lEff[i] / CYCLE_LEN;
		*ptrVal++ = tmp;
		tmp -= *ptrZero++;
		tmp = tmp * 100 / *ptrFactor++;
		queue_in(&qvol[i], tmp);
		*ptrAmp++ = queue_avg(&qvol[i]);
	}
}
