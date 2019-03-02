/***************************************************
 * Copyright (C),2019 www.idsse.ac.cn
 * Written by chenming
 * Version 1.0
 * Data  2019-3-3
 * Description: SBG姿态模块通信头文件
 * *************************************************/
#ifndef __SBG_COMM__
#define __SBG_COMM__

#include "stm32f10x.h"

#define SBG_START_ADR 20

void SBG_Init(void);
void SBG_Task(void);
void USART2_IRQHandler(void);
uint16_t calcCRC(unsigned char *pBuf, uint16_t bufSize);

static void SBGDataProcess1(void);
static void SBGDataProcess2(void);
#endif
