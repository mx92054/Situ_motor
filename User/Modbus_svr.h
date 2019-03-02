
#ifndef __MODBUS_COM__
#define __MODBUS_COM__

#include "stm32f10x.h"
#include <stdio.h>


#define  USE_USART1

//-------------------串口1宏定义------------------------------------
#ifdef USE_USART1
// 串口1-USART1
#define  MODBUS_USARTx                   USART1
#define  MODBUS_USART_CLK                RCC_APB2Periph_USART1
#define  MODBUS_USART_APBxClkCmd         RCC_APB2PeriphClockCmd
#define  MODBUS_USART_BAUDRATE           115200

// USART GPIO 引脚宏定义
#define  MODBUS_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  MODBUS_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  MODBUS_USART_TX_GPIO_PORT       GPIOA   
#define  MODBUS_USART_TX_GPIO_PIN        GPIO_Pin_9
#define  MODBUS_USART_RX_GPIO_PORT       GPIOA
#define  MODBUS_USART_RX_GPIO_PIN        GPIO_Pin_10

#define  MODBUS_USART_IRQ                USART1_IRQn
#define  MODBUS_USART_IRQHandler         USART1_IRQHandler
#endif

//-------------------串口2宏定义------------------------------------
#ifdef USE_USART2
// 串口1-USART1
#define  MODBUS_USARTx                   USART2
#define  MODBUS_USART_CLK                RCC_APB1Periph_USART2
#define  MODBUS_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  MODBUS_USART_BAUDRATE           115200

// USART GPIO 引脚宏定义
#define  MODBUS_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  MODBUS_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  MODBUS_USART_TX_GPIO_PORT       GPIOA   
#define  MODBUS_USART_TX_GPIO_PIN        GPIO_Pin_2
#define  MODBUS_USART_RX_GPIO_PORT       GPIOA
#define  MODBUS_USART_RX_GPIO_PIN        GPIO_Pin_3

#define  MODBUS_USART_IRQ                USART2_IRQn
#define  MODBUS_USART_IRQHandler         USART2_IRQHandler
#endif

//-------------------MODBUS DATA DEFINE--------------------------------
#define REG_LEN								200
#define COIL_LEN							200
#define BIT2BYTE(n)						((((n) & 0x0007) == 0)?((n) >> 3) : (((n) >> 3) + 1))
#define SETBIT_BYTE(n,bit)		( (n) | (0x01 << (bit)))
#define RESETBIT_BYTE(n,bit)	( (n) & (~(0x01 << (bit))))
#define GETBIT_BYTE(n,bit)		( ((n) >> (bit)) & 0x01 )

extern u8 bSaved ;

//------------------Function Define ----------------------------------
void	Modbus_init(void) ;
void	Modbus_task(void) ;
void	ModbusTimer(void) ;
u16 	CRC16(const uint8_t *nData, uint8_t wLength) ;
void 	MODBUS_USART_IRQHandler(void) ;
void 	SaveToBKP(u16 nAddr, u16 val) ;

static void	Error_response(u8 errno) ;
static void  Normal_response(void) ;
static u8		Modbus_Procotol_Chain(void) ;

void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);


#endif
//------------end fo file----------------------------------

