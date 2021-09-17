#ifndef HT1621_H
#define HT1621_H	
	
#include "stm32f10x.h"
#include "systick.h"
#include "HT1621.h"

 #define uchar unsigned char
 #define uint  unsigned int
	 
 #define HT1621_Delay 10
 
 #define HT1621_CS_LOW()   GPIO_ResetBits(GPIOB, GPIO_Pin_4) 
 #define HT1621_CS_HIGH()  GPIO_SetBits(GPIOB, GPIO_Pin_4) 
 #define HT1621_DAT_LOW()  GPIO_ResetBits(GPIOB, GPIO_Pin_7) 
 #define HT1621_DAT_HIGH() GPIO_SetBits(GPIOB, GPIO_Pin_7)  
 #define HT1621_WR_LOW()   GPIO_ResetBits(GPIOB, GPIO_Pin_6) 
 #define HT1621_WR_HIGH()  GPIO_SetBits(GPIOB, GPIO_Pin_6) 



void Ht1621_Init(void);
void Ht1621WrDataCmd(uchar ucAddr);
void Ht1621WrByte(uchar ucTmpData);
void Ht1621WrByte(uchar ucTmpData);
	

#endif
