#include "stm32f10x.h"   
#include <math.h>

#include "..\bsp\bsp_led.h"
#include "..\bsp\SysTick.h"
#include "..\bsp\bsp_exti.h"
#include "..\bsp\bsp_innerflash.h"
#include "..\bsp\adc.h"

#include "modbus_svr.h"
#include "SBG_Comm.h"

#define BIT_VALUE(val,bit)    ((val & (0x0001 << bit)) >> bit)

short wReg[200] ;
short coils[200] ;

extern u8 bSaved ;

static void Handle_Effect(void) ;
//--------------------------------------------------------------------------------
int main(void)
{
	int n ;
	
	SysTick_Init() ;
	
	LED_GPIO_Config() ;
	Modbus_init() ;
	SBG_Init() ;
	ADC1_Init() ;
	
	InternalFlashRead(wReg, 200) ;
	wReg[110]++ ;			// 启动次数加1
	bSaved = 1 ;	
	for(n = 10 ; n < 16 ; n++)
		wReg[n] = 0 ;

	SetTimer(1,200) ;
	SetTimer(2,500) ;
	SetTimer(3,1000) ;
	
	//IWDG_Configuration() ;
	LED1_OFF ;
	while(1)
	{
		Modbus_task() ;
		//SBG_Task() ;
		
		//-----------------------------------------------------------------
		if ( GetTimer(1) )	
		{
			OutputDigital() ;
			InputDigital() ;			
		}		
		
		//------------------------------------------------------------------
		if ( GetTimer(2) )
		{
			LED1_TOGGLE ;
			IWDG_Feed() ;
			if ( bEffectHandle )
			{
				//Handle_Effect() ;
				wReg[0] = wGraph[0];
				wReg[1] = wGraph[1];
				wReg[2] = wGraph[2];
				bEffectHandle = 0 ;
			}
		}
		
		if ( GetTimer(3) && bSaved )
		{
			InternalFlashWrite(wReg, 200) ;
			bSaved = 0 ;
		}
	}	
}

//--------------------------------------------------------------
//        有效值计算程序
//---------------------------------------------------------------
//wReg[20] - Middle Value of voltage
static void Handle_Effect(void)
{
	uint16_t 	i,j ;
	uint16_t  *pData,Temp ;
	uint32_t	lEff[3] ;					//Effective value : sum of square

	for(i = 0 ; i < 3 ; i++)
	{
		lEff[i] = 0 ;
	}	
	
	pData = wGraph ;	
	for(i = 0 ; i < 120 ; i++)
	{
		for( j = 0 ; j < 3 ; j++)
		{
			Temp = *pData++ ;
//			if ( Temp > wReg[20 + j] ) 
//				Temp -= wReg[20 + j] ;
//			else
//				Temp = wReg[20 + j] - Temp ;
		
			lEff[j] += Temp*Temp ;
		}
	}	
	
	for(i = 0 ; i < 3 ; i++)
	{
		wReg[i] = (short)sqrt(lEff[i]/120) ;		//Effective value is square root
	}
	
	for(i = 2 ; i < 122 ; i += 3)
		wReg[i/3 + 31] = wGraph[i] ;
}



