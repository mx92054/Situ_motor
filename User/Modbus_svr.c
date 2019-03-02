/***************************************************
 * Copyright (C),2019 www.idsse.ac.cn
 * Written by chenming
 * Version 1.0
 * Data  2019-3-3
 * Description: modbus接口实现文件
 * *************************************************/

#include <stdio.h>
#include "Modbus_svr.h"
#include "..\bsp\bsp_led.h"

#define ADDRESS wReg[100] //当前站地址
#define BAUDRATE wReg[101]

Modbus_block mblock1;

//-------------------------------------------------------------------------------
//	@brief	协议栈初始化
//	@param	None
//	@retval	None
//-------------------------------------------------------------------------------
void Modbus_init(void)
{
	char msg[100];

	ModbusSvr_block_init(&mblock1);
	MODBUS_Config(mblock1.baudrate);

	sprintf(msg, "\r\nStation No: %d, Baudrate: %d", mblock1.station, mblock1.baudrate);
	Usart_SendString(MODBUS_USARTx, msg);
	sprintf(msg, "\r\nCoil Start adr: %4d, Len: %4d", mblock1.uCoilStartAdr, mblock1.uCoilLen);
	Usart_SendString(MODBUS_USARTx, msg);
	sprintf(msg, "\r\nReg  Start adr: %4d, Len: %4d", mblock1.uRegStartAdr, mblock1.uRegLen);
	Usart_SendString(MODBUS_USARTx, msg);
	sprintf(msg, "\r\nRom  Start adr: %4d, Len: %4d", mblock1.uRomStartAdr, mblock1.uRomLen);
	Usart_SendString(MODBUS_USARTx, msg);
}

//-------------------------------------------------------------------------------
//	@brief	协议任务调度
//	@param	None
//	@retval	None
//-------------------------------------------------------------------------------
void Modbus_task(void)
{
	ModbusSvr_task(&mblock1, MODBUS_USARTx);
}

//-------------------------------------------------------------------------------
//	@brief	modbus recieve counter
//	@param	None
//	@retval	None
//-------------------------------------------------------------------------------
void ModbusTimer(void)
{
	mblock1.nMBInterval++;
}

//-------------------------------------------------------------------------------
//	@brief	串口中断服务程序
//	@param	None
//	@retval	None
//-------------------------------------------------------------------------------
void MODBUS_USART_IRQHandler(void)
{
	ModbusSvr_isr(&mblock1, MODBUS_USARTx);

}

void Modbus_SavePara(void)
{
	ModbusSvr_save_para(&mblock1) ;
}

//-------------------------------------------------------------------------------
//	@brief	串口初始化
//	@param	None
//	@retval	None
//-------------------------------------------------------------------------------
static void MODBUS_Config(u32 baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	MODBUS_USART_GPIO_APBxClkCmd(MODBUS_USART_GPIO_CLK, ENABLE);
	MODBUS_USART_APBxClkCmd(MODBUS_USART_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = MODBUS_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MODBUS_USART_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = MODBUS_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(MODBUS_USART_RX_GPIO_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(MODBUS_USARTx, &USART_InitStructure);

	ModbusSvr_NVIC_Configuration(MODBUS_USART_IRQ);

	USART_ITConfig(MODBUS_USARTx, USART_IT_RXNE, ENABLE);

	USART_Cmd(MODBUS_USARTx, ENABLE);
}
//-----------------end of file---------------------------------------------
