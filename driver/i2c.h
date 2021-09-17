#ifndef __I2C_H
#define __I2C_H


#include "stm32f10x.h"

#define EEPROM_Ready_Flag 0x5A

#define SDA_Pin GPIO_Pin_4
#define SCL_Pin GPIO_Pin_3
#define SDA_Port GPIOA
#define SCL_Port GPIOA

#define SDA_H  GPIO_SetBits(SDA_Port, SDA_Pin)
#define SDA_L  GPIO_ResetBits(SDA_Port, SDA_Pin)

#define SCL_H  GPIO_SetBits(SCL_Port, SCL_Pin)
#define SCL_L  GPIO_ResetBits(SCL_Port, SCL_Pin)

#define SDA_read GPIO_ReadInputDataBit(SDA_Port ,SDA_Pin)


void Delay1us(void);
uint8_t I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NoAck(void);
void I2C_SendByte(uint8_t SendByte) ;
uint8_t I2C_ReceiveByte(void);
uint8_t I2C_WaitAck(void);
uint8_t EEPROM_WriteByte8(uint8_t regaddr, uint8_t data);
uint8_t EEPROM_ReadByte8(uint8_t regaddr);
uint8_t EEPROM_WriteByte16(u16 regaddr, uint8_t data);
uint8_t EEPROM_ReadByte16(u16 regaddr);

#endif

