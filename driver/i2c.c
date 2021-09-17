#include "i2c.h"
#include "stm32f10x_i2c.h"
#include "systick.h"





//64M对应20个为0.92uS，30个为1.16uS
void Delay1us(void)
{
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	

}

uint8_t I2C_Start(void)
{
	SDA_H;
	SCL_H;
	Delay1us();
	
	if(!SDA_read){
		return 0;
	} 
	
	SDA_L;
	Delay1us();
	if(SDA_read){
		return 0;
	}
	
	SCL_L;
	//Delay1us();
	return 1;
}

void I2C_Stop(void)
{
	SCL_L;
	Delay1us();
	SDA_L;
	Delay1us();
	SCL_H;
	Delay1us();
	SDA_H;
	Delay1us();
}

void I2C_Ack(void)
{
	//SCL_L;
	//Delay1us();
	SDA_L;
	Delay1us();
	SCL_H;
	Delay1us();
	SCL_L;
	//Delay1us();
}

void I2C_NoAck(void)
{
	//SCL_L;
	//Delay1us();
	SDA_H;
	Delay1us();
	SCL_H;
	Delay1us();
	SCL_L;
	//Delay1us();
}

uint8_t I2C_WaitAck(void) 
{
	//SCL_L;
	//Delay1us();
	SDA_H;
	Delay1us();
	SCL_H;
	Delay1us();
	
	if(SDA_read){
		Delay1us();
		SCL_L;
		return 0;
	}
	
	Delay1us();
	SCL_L;
	return 1;
}

void I2C_SendByte(uint8_t SendByte) 
{
	uint8_t i=8;
	while(i--){
		SCL_L;
		if(SendByte&0x80){
			SDA_H;
		}else{
			SDA_L; 
		}
		SendByte<<=1;
		Delay1us();
		SCL_H;
		Delay1us();
	}
	SCL_L;
	//delay_us(10);
}


uint8_t I2C_ReceiveByte(void) 
{ 
	uint8_t i=8;
	uint8_t ReceiveByte=0;
	SDA_H;
	while(i--){
		ReceiveByte<<=1;
		SCL_L;
		Delay1us();
		SCL_H;
		Delay1us();
		if(SDA_read){
			ReceiveByte|=0x01;
		}else{
		
		}
		Delay1us();
	}
	SCL_L;
	return ReceiveByte;
}


uint8_t EEPROM_WriteByte16(u16 regaddr, uint8_t data)
{
	uint8_t regaddr1,regaddr2;
	regaddr1=(uint8_t)(regaddr>>8);
	regaddr2=(uint8_t)regaddr;
	if(!I2C_Start()){
		return 0;
	}
	I2C_SendByte(0xa0);
	if(!I2C_WaitAck()){
		I2C_Stop();
		return 0;
	}else{
	
	}
	I2C_SendByte(regaddr1);
	if(!I2C_WaitAck()){
		I2C_Stop();
		return 0;
	}else{
	
	}
	I2C_SendByte(regaddr2);
	if(!I2C_WaitAck()){
		I2C_Stop();
		return 0;
	}else{
	
	}
	I2C_SendByte(data); 
	
	if(!I2C_WaitAck()){
		I2C_Stop(); 
		return 0;
	}else{
	
	}
	I2C_Stop();
	delay_ms(5);
	return 1;
}

uint8_t EEPROM_ReadByte16(u16 regaddr)
{
	uint8_t temp;
	uint8_t regaddr1,regaddr2;
	regaddr1=(uint8_t)(regaddr>>8);
	regaddr2=(uint8_t)regaddr;
	
	if(!I2C_Start()){
		return 0;
	}
	
	I2C_SendByte(0xa0);
	
	if(!I2C_WaitAck()){
		I2C_Stop(); 
		return 0;
	}

	I2C_SendByte(regaddr1); 
	
	if(!I2C_WaitAck()){
		I2C_Stop(); 
		return 0;
	}

	
	I2C_SendByte(regaddr2);
	
	if(!I2C_WaitAck()){
		I2C_Stop(); 
		return 0;
	}

	I2C_Start();
	I2C_SendByte(0xa1);
	if(!I2C_WaitAck()){
		I2C_Stop(); 
		return 0;
	}

	temp=I2C_ReceiveByte();
	I2C_Stop();
	delay_ms(5);
	return temp;
}


uint8_t EEPROM_WriteByte8(uint8_t regaddr, uint8_t data)
{
	if(!I2C_Start()){
		return 0;
	}

	I2C_SendByte(0xa0);
	if(!I2C_WaitAck()){
		I2C_Stop();
		return 0;
	}
	
	I2C_SendByte(regaddr); 
	if(!I2C_WaitAck()){
		I2C_Stop();
		return 0;
	}

	I2C_SendByte(data); 
	if(!I2C_WaitAck()){
		I2C_Stop(); 
		return 0;
	}

	I2C_Stop();
	delay_ms(5);
	return 1;
}

uint8_t EEPROM_ReadByte8(uint8_t regaddr)
{
	uint8_t temp;
	if(!I2C_Start()){
		return 0;
	}

	I2C_SendByte(0xa0);
	if(!I2C_WaitAck()){
		I2C_Stop(); 
		return 0;
	}
	
	I2C_SendByte(regaddr); 
	if(!I2C_WaitAck()){
		I2C_Stop(); 
		return 0;
	}

	I2C_Start();
	I2C_SendByte(0xa1);
	if(!I2C_WaitAck()){
		I2C_Stop(); 
		return 0;
	}

	temp=I2C_ReceiveByte();
	I2C_Stop();
	//Delay_ms(1);
	delay_us(100);
	return temp;
}



