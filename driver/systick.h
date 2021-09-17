#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "core_cm3.h"

void Systick_Init(void);
//void delay_us(volatile u32 nus);
//void delay_ms(volatile u16 nms);
void delay_us(uint16_t Time);
void delay_ms(uint16_t Time);
uint32_t gettick(void);
void settick(uint32_t Value);
void systemRest(void);
void RESETALL(void);


#endif /* __SYSTICK_H */
