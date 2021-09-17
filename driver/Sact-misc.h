/**
  ******************************************************************************
  * @file    Sact-misc.h
  * @author  gjf
  * @version V1.0.0
  * @date    2018-6-9
  * @brief   
  ******************************************************************************

  ******************************************************************************
  */


	/* Define to prevent recursive inclusion -------------------------------------*/


#ifndef SACTMISC_H
#define SACTMISC_H	



	/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "systick.h"
#include "HT1621.h"
#include "stm32f10x_tim.h"

#define BL_High 90
#define BL_Low 10
#define BL_Off 0

#define BL_SetHigh TIM_SetCompare2(TIM3,BL_High)
#define BL_SetLow TIM_SetCompare2(TIM3,BL_Low)
#define BL_SetOff TIM_SetCompare2(TIM3,BL_Off)
	
	
void LCD_Disp_Init(void);
void LCD_Disp_Update(void);
void LCD_Set_Temp(unsigned int temp, u8 position);
void LCD_Set_Temp_clear(u8 position);
uint8_t SACT_Temp_Set(uint8_t value);
uint8_t SACT_Mode_set(uint8_t mode);
uint8_t SACT_Speed_set(uint8_t speed);
uint8_t SACT_ONOFF_set(uint8_t onoff);


uint8_t get_real_tmp(void);
uint8_t get_set_tmp(void);
uint8_t get_onoff(void);
uint8_t get_mode(void);
uint8_t get_speed(void);







#endif
