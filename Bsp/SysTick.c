/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * �ļ���  ��SysTick.c
 * ����    ��SysTick ϵͳ�δ�ʱ��10us�жϺ�����,�ж�ʱ����������ã�
**********************************************************************************/
#include "SysTick.h"
#include "Modbus_svr.h"

#define TIMER_NUM  10

extern short wReg[] ;

static __IO u32 TimingDelay = 0 ;  	// 	��ʱ��ʱ��������
__IO u16		TimePre[TIMER_NUM] ;		//	������Ԥ��ֵ
__IO u16		TimeCur[TIMER_NUM] ;		//	��������ǰֵ
__IO u16		TimeShift[TIMER_NUM] ;	// 	����������м�ֵ
__IO u8			TimerFlag[TIMER_NUM] ;	//  �������ﵽ��־
__IO u8			TimerSFlag[TIMER_NUM] ;	//  ����������ﵽ���������־

//-------------------------------------------------------------------------------
//	@brief	SysTick�жϳ�ʼ��
//	@param	None
//	@retval	None
//-------------------------------------------------------------------------------
void SysTick_Init(void)
{
	int i ;
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 10000   100us�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
	if (SysTick_Config(SystemCoreClock / 10000))	// ST3.5.0��汾
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
//	@brief	���뼶��ʱ
//	@param	None
//	@retval	None
//-------------------------------------------------------------------------------
void Delay_ms(__IO u32 nTime)
{ 
	TimingDelay = nTime;	
	while(TimingDelay != 0);
}

//-------------------------------------------------------------------------------
//	@brief	SysTick�жϷ������
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
//	@brief	��ʱ���趨
//	@param	no  ��ʱ����   val  ��ʱʱ��
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
//	@brief	��ʱ���趨
//	@param	no  ��ʱ����   val  ��ʱʱ��
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
//	@brief	��ȡ��ʱ����״̬
//	@param	no ��ʱ����
//	@retval	1  ��ʱʱ�䵽
//					0  ��ʱʱ��δ��
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
//	@brief	��ȡ��ʱ������
//	@param	no ��ʱ����
//	@retval	1  ��ʱʱ�䵽�﷭תʱ��
//					0  ��ʱʱ��δ����תʱ��
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

