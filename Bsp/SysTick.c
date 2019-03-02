/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * 文件名  ：SysTick.c
 * 描述    ：SysTick 系统滴答时钟10us中断函数库,中断时间可自由配置，
**********************************************************************************/
#include "SysTick.h"
#include "Modbus_svr.h"

#define TIMER_NUM  10

extern short wReg[] ;

static __IO u32 TimingDelay = 0 ;  	// 	延时定时器计数器
__IO u16		TimePre[TIMER_NUM] ;		//	计数器预置值
__IO u16		TimeCur[TIMER_NUM] ;		//	计数器当前值
__IO u16		TimeShift[TIMER_NUM] ;	// 	区域计数器中间值
__IO u8			TimerFlag[TIMER_NUM] ;	//  计数器达到标志
__IO u8			TimerSFlag[TIMER_NUM] ;	//  区域计数器达到给定区域标志

//-------------------------------------------------------------------------------
//	@brief	SysTick中断初始化
//	@param	None
//	@retval	None
//-------------------------------------------------------------------------------
void SysTick_Init(void)
{
	int i ;
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 10000   100us中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
	if (SysTick_Config(SystemCoreClock / 10000))	// ST3.5.0库版本
	{ 
		while (1);
	}	
	// Enable SysTick
	SysTick->CTRL |= ~SysTick_CTRL_ENABLE_Msk;
	for( i = 0 ; i < TIMER_NUM ; i++)
	{
		TimePre[i] = 0 ;
		TimeCur[i] = 0 ;
		TimeShift[i] = 0 ;
		TimerFlag[i] = 0 ;
		TimerSFlag[i] = 0 ;
	}
}

//-------------------------------------------------------------------------------
//	@brief	毫秒级延时
//	@param	None
//	@retval	None
//-------------------------------------------------------------------------------
void Delay_ms(__IO u32 nTime)
{ 
	TimingDelay = nTime;	
	while(TimingDelay != 0);
}

//-------------------------------------------------------------------------------
//	@brief	SysTick中断服务程序
//	@param	None
//	@retval	None
//-------------------------------------------------------------------------------
void SysTick_Handler(void)
{
	int i ;
	
	ModbusTimer() ;

	if (!TimingDelay)
		TimingDelay-- ;
	
	for( i = 0 ; i < TIMER_NUM ; i++)
	{
		if ( TimeCur[i] )
		{
			if ( TimeCur[i] == 1 )
				TimerFlag[i] = 1 ;
			if ( TimeCur[i] == TimeShift[i] )
				TimerSFlag[i] = 1 ;
			TimeCur[i]-- ;			
		}
		else
		{
			TimerSFlag[i] = 0 ;
			TimeCur[i] = TimePre[i] ;
		}
	}	
}

//-------------------------------------------------------------------------------
//	@brief	定时器设定
//	@param	no  定时器号   val  定时时间
//	@retval	None
//-------------------------------------------------------------------------------
void SetTimer(u8 no, u16 val)
{
	if ( no < TIMER_NUM )
	{
		TimePre[no]  = val ;
		TimeCur[no] = val ;
	}
}

//-------------------------------------------------------------------------------
//	@brief	定时器设定
//	@param	no  定时器号   val  定时时间
//	@retval	None
//-------------------------------------------------------------------------------
void SetTimerVal(u8 no, u16 val, u16 ShiftVal)
{
	if ( no < TIMER_NUM )
	{
		TimePre[no]  = val ;
		TimeShift[no] = ShiftVal ;
		TimeCur[no] = val ;
	}
}

//-------------------------------------------------------------------------------
//	@brief	获取定时器的状态
//	@param	no 定时器号
//	@retval	1  定时时间到
//					0  定时时间未到
//-------------------------------------------------------------------------------
u16 GetTimer(u8 no)
{
	
	if ( no < TIMER_NUM && TimerFlag[no] )
	{
		TimerFlag[no] = 0 ;
		return 1 ;
	}
	else
		return 0 ;		
}

//-------------------------------------------------------------------------------
//	@brief	获取定时器数据
//	@param	no 定时器号
//	@retval	1  定时时间到达翻转时间
//					0  定时时间未到翻转时间
//-------------------------------------------------------------------------------
u16 GetTimerVal(u8 no) 
{
	if ( no < TIMER_NUM )
	{
		return TimerSFlag[no] ;
	}
	else
		return 0 ;
}

/************************END OF FILE************/

