#ifndef __SBG_COMM__
#define __SBG_COMM__

#include "stm32f10x.h"

void 			SBG_Init(void) ;
void  		SBG_Task(void) ;
void 			USART2_IRQHandler(void) ;
uint16_t 	calcCRC(unsigned char* pBuf, uint16_t bufSize) ;

static void  		SBGDataProcess1(void) ;
static void  		SBGDataProcess2(void) ;
#endif

