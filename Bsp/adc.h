#ifndef __ADC_H
#define	__ADC_H


#include "stm32f10x.h"

#define CYCLE_LEN 400


#define AI1_GPIO_PORT    	GPIOA			              /* GPIO端口 */
#define AI1_GPIO_CLK 	    RCC_APB2Periph_GPIOA		/* GPIO端口时钟 */
#define AI1_GPIO_PIN			GPIO_Pin_4			        /* 连接到SCL时钟线的GPIO */
#define AI1_CHANNEL				ADC_Channel_4

#define AI2_GPIO_PORT    	GPIOA			              /* GPIO端口 */
#define AI2_GPIO_CLK 	    RCC_APB2Periph_GPIOA		/* GPIO端口时钟 */
#define AI2_GPIO_PIN			GPIO_Pin_5			        /* 连接到SCL时钟线的GPIO */
#define AI2_CHANNEL				ADC_Channel_5

#define AI3_GPIO_PORT    	GPIOA			              /* GPIO端口 */
#define AI3_GPIO_CLK 	    RCC_APB2Periph_GPIOA		/* GPIO端口时钟 */
#define AI3_GPIO_PIN			GPIO_Pin_6			        /* 连接到SCL时钟线的GPIO */
#define AI3_CHANNEL				ADC_Channel_6

extern  __IO uint16_t 		ADC_ConvertedValue[] ;
extern uint16_t	 					wGraph[] ;
extern int 								bEffectHandle ;

void ADC1_Init(void);
void DMA1_Channel1_IRQHandler(void);

#endif /* __ADC_H */

