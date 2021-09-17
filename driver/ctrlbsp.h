#ifndef CTRLBSP_H
#define CTRLBSP_H_H			

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_it.h"
#include "stm32f10x_adc.h"
#include "misc.h"
#include "systick.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_flash.h"

#ifdef __BrdConf_SACTV200//ÎÂ¿ØV2.00
#define TIM2_Time 10
#define TIM3_Time 1
#endif

#define  __DEBUG
#define  USE_EEPROM

void CTRLBSP_Init(void);//°å¼¶ÅäÖÃ
void USART_printf ( USART_TypeDef * USARTx, char * Data, ... );
int str2int(const char *str);


#ifdef __DEBUG

#define DEBUG(format,...)    USART_printf(USART3,format, ##__VA_ARGS__)

#else

#define DEBUG(format,...)

#endif


#endif
