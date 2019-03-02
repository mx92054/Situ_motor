#include "SBG_Comm.h"
#include "../bsp/bsp_led.h"

extern short wReg[] ;

u8		SBGBuffer[300] ;		//　通信帧缓冲器
u8*		tsk_ptr1 ;					//　06通信帧缓存
u8* 	tsk_ptr2 ;					//  03通信帧缓存
u8*		isr_ptr ;						//  串口中断1接收器缓存

u8  	SBGIdle1 = 1 ;			//   06通信帧处理标志
u8  	SBGIdle2 = 1 ;			//   03通信帧处理标志

u16		SBGPos = 0 ;				//	 串口接收字符存储位置指示
u8		sbg_len ;						//	 串口通信帧长度
//-------------------------------------------------------------------------------
void USART2_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			     	
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//-------------------------------------------------------------------------------
void USART2_Config(u32 baud)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef 	GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, 	ENABLE); 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, 	ENABLE);
  
  //usart_init----------------------------------------------------
  
  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		   
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 
  
  USART_InitStructure.USART_BaudRate =baud;											
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;	
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			
  USART_InitStructure.USART_Parity = USART_Parity_No;						
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					

  /* Configure USART1 */
  USART_Init(USART2, &USART_InitStructure);							
  
  /* Enable USART2 Receive and Transmit interrupts */
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);             
  //USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);				   
  
  /* Enable the USART1 */
  USART_Cmd(USART2, ENABLE);	
}

//----------------------------------------------------------------------------------
void 	SBG_Init(void)
{
	tsk_ptr1 = SBGBuffer ;
	tsk_ptr2 = SBGBuffer + 100 ;
	isr_ptr = SBGBuffer + 200 ;	
	
	USART2_NVIC_Configuration() ;
	USART2_Config(115200) ;
}


//------------------------------------------------------------------------------
void  SBG_Task(void)
{
	if ( !SBGIdle1 )
		{
			SBGDataProcess1() ;
			SBGIdle1 = 1 ;
		}
		
	if ( !SBGIdle2 )
		{
			SBGDataProcess2() ;
			SBGIdle2 = 1 ;
		}		
}

//---------------------------------------------------------------------------------
static void SBGDataProcess1(void)
{
	uint16_t 	crc1,crc2 ;
	u32				stamp ;
	float			val ; 
	
	crc1 = calcCRC(tsk_ptr1 + 2, 36) ;
	crc2 = tsk_ptr1[39] << 8 | tsk_ptr1[38] ;
	if ( crc1 != crc2 )
		return ;	
	
	stamp = *((uint32_t*)(tsk_ptr1 +6)) ;		//time stamp ;
	wReg[20] = (uint16_t)(stamp/1000000) ;

	val = *((float*)(tsk_ptr1 + 10)) ;
	val = val*1800.0/3.14;
	wReg[21] = (int16_t)(val) ;					//ROLL angle
	val = *((float*)(tsk_ptr1 + 14)) ;
	val = val*1800.0/3.14;
	wReg[22] = (int16_t)(val) ;					//PITCH angle
	val = *((float*)(tsk_ptr1 + 18)) ;
	val = val*1800.0/3.14;
	wReg[23] = (int16_t)(val) ;					//YAW angle					
}

//----------------------------------------------------------------------------------
static void  SBGDataProcess2(void)
{
	uint16_t 	crc1,crc2 ;
	float			val ; 
	
	crc1 = calcCRC(tsk_ptr2 + 2, 62) ;
	crc2 = tsk_ptr2[65] << 8 | tsk_ptr2[64] ;
	if ( crc1 != crc2 )
		return ;	
		
	val = *((float*)(tsk_ptr2 + 12)) ;
	val = val*1000.0 ;
	wReg[24] = (int16_t)(val) ;					// X axis acc
	val = *((float*)(tsk_ptr2 + 16)) ;
	val = val*1000.0 ;
	wReg[25] = (int16_t)(val) ;					// Y axis acc
	val = *((float*)(tsk_ptr2 + 20)) ;
	val = val*1000.0 ;
	wReg[26] = (int16_t)(val) ;					// Z axis acc
	
	val = *((float*)(tsk_ptr2 + 24)) ;				
	val = val*1800.0/3.14 ;
	wReg[27] = (int16_t)(val) ;					// GYRD X
	val = *((float*)(tsk_ptr2 + 28)) ;
	val = val*1800.0/3.14 ;
	wReg[28] = (int16_t)(val) ;					// GYRD Y
	val = *((float*)(tsk_ptr2 + 32)) ;
	val = val*1800.0/3.14 ;
	wReg[29] = (int16_t)(val) ;					// GYRD Z
	
	val = *((float*)(tsk_ptr2 + 36)) ;
	val = val*10.0 ;
	wReg[30] = (uint16_t)(val) ;				// SBG inner tempature	
}

//----------------------------------------------------------------------------------
void USART2_IRQHandler(void)
{
		u8		ch ;
		u8*		ptr ;

		if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)	   //判断读寄存器是否非空
  	{	
			USART_ClearITPendingBit(USART2,USART_IT_RXNE);
			// Read one byte from the receive data register     	
			ch = USART_ReceiveData(USART2);   //将读寄存器的数据缓存到接收缓冲区里
			
			if ( SBGPos == 0 )										//当前通信帧为空
			{
				if ( ch == 0xFF )
					isr_ptr[SBGPos++] = ch ;			
			}
			else
				{
					if ( SBGPos == 1 )								//当前通信帧长度为1
						{
							if ( ch == 0x5A )							//通信帧前两个字符是否为0xFF,0x5A
								isr_ptr[SBGPos++] = ch ;
							else
								SBGPos = 0 ;
						}
					else															//当前通信帧长度大于1
						{
							if (SBGPos == 5 )							//收到6个字符，其中第5、第6个为数据长度
								{
									sbg_len = ((ch << 8) | isr_ptr[4]) + 9 ;
									if ( sbg_len > 100 )
										SBGPos = 0 ;
								}
																
							isr_ptr[SBGPos++] = ch ;
								
							if ( SBGPos > 6 && SBGPos >= sbg_len && ch == 0x33 )
							{
								if ( isr_ptr[2] == 0x06)
									{
										ptr = isr_ptr ;
										isr_ptr = tsk_ptr1 ;
										tsk_ptr1 = ptr ;		
										SBGIdle1 = 0 ;								
									}
								if ( isr_ptr[2] == 0x03)
									{
										ptr = isr_ptr ;
										isr_ptr = tsk_ptr2 ;
										tsk_ptr2 = ptr ;		
										SBGIdle2 = 0 ;								
									}
								SBGPos = 0 ;
							}							
						}								
				}
   	} 
		
  	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)                     
  	{ 
     	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
  	}	  	
}

//----------------------------------------------------------------------------------
uint16_t calcCRC(unsigned char* pBuf, uint16_t bufSize)
{
	unsigned char* pBytesArray = pBuf ;
	uint16_t 	poly = 0x8408 ;
	uint16_t	crc = 0 ;
	unsigned char carry, i_bits ;
	uint16_t	j ;
	
	for( j = 0 ; j < bufSize ; j++)
	{
		crc = crc ^ pBytesArray[j] ;
		for( i_bits = 0 ; i_bits < 8 ; i_bits++)
		{
			carry = crc & 1 ;
			crc = crc >> 1 ;
			if ( carry )
				crc ^= poly ;
		}
	}
	
	return crc ;
}


		
//-------------------end of file----------------------------------
