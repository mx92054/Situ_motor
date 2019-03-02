#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

void 	SysTick_Init(void);
void 	Delay_ms(__IO u32 nTime);
void 	SysTick_Handler(void) ;
u16 	GetTimer(u8 no) ;
void 	SetTimer(u8 no, u16 val) ;
u16 	GetTimerVal(u8 no); 
void SetTimerVal(u8 no, u16 val, u16 ShiftVal);

#endif /* __SYSTICK_H */
