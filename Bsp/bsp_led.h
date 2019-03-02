#ifndef __LED_H
#define	__LED_H


#include "stm32f10x.h"


/* 定义LED连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的LED引脚 */
// R-红色
#define LED1_GPIO_PORT    	GPIOA			              /* GPIO端口 */
#define LED1_GPIO_CLK 	    RCC_APB2Periph_GPIOA		/* GPIO端口时钟 */
#define LED1_GPIO_PIN				GPIO_Pin_1			        /* 连接到SCL时钟线的GPIO */

#define DO_GPIO_PORT				GPIOB
#define	DO_GPIO_CLK					RCC_APB2Periph_GPIOB
#define DO_GPIO_PINS				GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4  |\
														GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7

#define DI_GPIO_PORT				GPIOB
#define	DI_GPIO_CLK					RCC_APB2Periph_GPIOB
#define DI_GPIO_PINS				GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | \
														GPIO_Pin_14 | GPIO_Pin_15


/** the macro definition to trigger the led on or off 1 - off 0 - on */
#define ON  0
#define OFF 1

#define LED1(a)	if (a)	\
					GPIO_SetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);\
					else		\
					GPIO_ResetBits(LED1_GPIO_PORT,LED1_GPIO_PIN)

/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //输出为高电平		
#define digitalLo(p,i)		 {p->BRR=i;}	 //输出低电平
#define digitalToggle(p,i) {p->ODR ^=i;} //输出反转状态


/* 定义控制IO的宏 */
#define LED1_TOGGLE		 digitalToggle(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED1_OFF		   digitalLo(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED1_ON			   digitalHi(LED1_GPIO_PORT,LED1_GPIO_PIN)

void LED_GPIO_Config(void) ;
void OutputDigital(void) ;
void InputDigital(void) ;

void BKP_Init(void) ;
void ReloadParameter(void) ;
void SaveaBKPParameter(void) ;

void IWDG_Configuration(void);
void IWDG_Feed(void) ;

#endif /* __LED_H */
