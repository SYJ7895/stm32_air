#include "Sact-misc.h"
#include "systick.h"
#include "data_struct.h"
#include "ctrlbsp.h"




//================================定义按键获取==============================================//

#define ON_OFF   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)  //读取按键 0
#define MODE     GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)  //读取按键 1
#define SPEED    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)  //读取按键 2
#define UP       GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13) //读取按键 3
#define DOWN     GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15) //读取按键 4
#define IS_CLICK 0
#define IS_UP 1


#define Temp_Pos_Set 1
#define Temp_Pos_Real 0




#define LCD_FIX_DISP    0x08  //LCD 显示模式图案
#define LCD_MODE_WARM   0x04  //显示为制热
#define LCD_MODE_COOL   0x02  //显示为制冷
#define LCD_MODE_WIND   0x01  //显示为送风
#define LCD_MODE_OFF    0x00  //显示关闭

#define LCD_FAN_HIGH    0x20  //LCD 显示风速高
#define LCD_FAN_MID     0x40  //LCD 显示风速中
#define LCD_FAN_LOW     0x80  //LCD 显示风速低
#define LCD_FAN_AUTO    0x10  //LCD 显示风速自动
 

#define GET_REAL_TMP    60  //60秒测量一次当前温度
#define REST_TIME       7  //长按10s复位
#define BLOFFTIME       3   //按键按下亮背光3s

#define  RELAY_OPEN     GPIO_Pin_12
#define  RELAY_LOW      GPIO_Pin_11
#define  RELAY_MID      GPIO_Pin_3

extern uint8_t gIsConf;
extern uint8_t gNetState;
extern uint32_t tickrep;

//基于3.3V，STM32 ADC，10kNTC加10k电阻，计算出
static unsigned int Temp_Detect[]={939,978,1017,1058,1098,1140,1182,1225,1269,\
1313,1357,1402,1448,1493,1539,1585,1632,1678,1725,1771,1818,1864,1910,1956,\
2002,2048,2093,2138,2183,2227,2271,2314,2357,2399,2441,2482,2522,2562,2601,\
2639,2677,2714,2750,2786,2821,2855,2888,2921,2953,2984,3014};

// 0-9 字段码
static uchar Ht1621Tab[]={0xDF,0x86,0xEB,0xAF,0xB6,0xBD,0xFD,0x87,0xFF,0xBF};
static uchar SACT_Mode_Array[]={LCD_MODE_WIND,LCD_MODE_COOL,LCD_MODE_WARM};

static uint8_t LCD_First_Disp=1;


static uint8_t AC_On_Ctl = 0;        //风速自动控制开关
static uint8_t Thermal_Real = 25;    //当前实际温度
static uint8_t Thermal_hyst = 1;     //回滞温度
static uint8_t Thermal_Set = 27;     //设置温度

static uint temp_real_toDisp=16;
static uint temp_buffer=27000;
static uint temp_change_store=27000;

static uint8_t ONOFF_State = 0x0;
static uint8_t SACT_FAN_Speed = LCD_FAN_AUTO;//0x10
static uint8_t SACT_Mode = 0x1;


static uint8_t update_lcd = 0;
static uint8_t bloff  = 0;
static uint32_t blTick = 0;

/***************************************************
*RELAY-OPEN	RELAY-LOW	RELAY-MID	电动阀门	     风机
*   0	        -	        -	        关	   风机不转
*   1	        1	        -	        开	   低速
*   1	        0	        0	        开	   中速
*   1	        0	        1	        开	   高速
*****************************************************/


void onoff_set(uint8_t onoff)
{
   if(onoff){
   	  //设置为高电平，打开空调
      GPIO_SetBits(GPIOA,RELAY_OPEN);
   }else{
      GPIO_ResetBits(GPIOA,RELAY_OPEN);
   }
}


void speed_set_low()
{
    GPIO_SetBits(GPIOA,RELAY_LOW);
}

void speed_set_mid()
{
   GPIO_ResetBits(GPIOA,RELAY_LOW);
   GPIO_ResetBits(GPIOB,RELAY_MID);
}

void speed_set_high()
{
   GPIO_ResetBits(GPIOA,RELAY_LOW);
   GPIO_SetBits(GPIOB,RELAY_MID);
}





uint8_t SACT_Temp_Set(uint8_t value)
{
   uint8_t ret = 1;
	//Down
	if(value > 35 || value < 16){
	
	}else{
	   ret = 0;
	   Thermal_Set = value;
	}
	
    if(!ret){
	 update_lcd = 1;
    }
	return ret;
}

void SACT_Temp_up()
{
   //空调开启时才能够设置
	if(ONOFF_State){
	   if(Thermal_Set >= 35){
		
		}else{
		   Thermal_Set ++;
		}
	}
}

void SACT_Temp_down()
{
  //空调开启时才能够设置
  if(ONOFF_State){
	   if(Thermal_Set <= 16){
		
	   }else{
		   Thermal_Set --;
	   }
  }
}



uint8_t SACT_Mode_set(uint8_t mode)
{
    uint8_t ret = 0;
	switch (mode){
		case SET_MODE_COOL:
			SACT_Mode = 1;
			break;
		case SET_MODE_WARM:
			SACT_Mode = 2;
			break;
		case SET_MODE_WIND:
			SACT_Mode = 0;
			break;
		default:
		    ret = 1;
		    break;
	}

    if(!ret){
	 update_lcd = 1;
    }
	return ret;


}


void Speed_change(uint8_t speed)
{
   AC_On_Ctl = 0;
   switch(speed){
      case LCD_FAN_AUTO:
	  	AC_On_Ctl = 1;
	  	break;
      case LCD_FAN_LOW:
	  	speed_set_low();
	  	break;
	  case LCD_FAN_MID:
	  	speed_set_mid();
	  	break;
	  case LCD_FAN_HIGH:
	  	speed_set_high();
	  	break;
	  default:
	  	break;
   }
}

uint8_t SACT_Speed_set(uint8_t speed)
{
    uint8_t ret = 0;
	//空调开启时才能够设置风速
	if(ONOFF_State){
		AC_On_Ctl = 0;
		switch (speed){
			case SET_SPEED_AUTO:
				SACT_FAN_Speed = LCD_FAN_AUTO;
				break;
			case SET_SPEED_LOW:
				SACT_FAN_Speed = LCD_FAN_LOW;
				break;
			case SET_SPEED_MID:
				SACT_FAN_Speed = LCD_FAN_MID;
				break;
			case SET_SPEED_HIGH:
				SACT_FAN_Speed = LCD_FAN_HIGH;
				break;
			default:
			    ret = 1;
			    break;
		}
		Speed_change(SACT_FAN_Speed);
	}

    if(!ret){
	 update_lcd = 1;
    }
	return ret;
}

uint8_t SACT_ONOFF_set(uint8_t onoff)
{
    uint8_t ret = 0;
	switch (onoff){
		case SET_SWITCH_ON:
			ONOFF_State = 0xFF;
	        onoff_set(ONOFF_State);
			break;
		case SET_SWITCH_OFF:
			ONOFF_State = 0;
			onoff_set(ONOFF_State);
			break;
		default:
		    ret = 1;
		    break;
	}

    if(!ret){
	 update_lcd = 1;
    }
	return ret;

}




void SACT_Speed_Change(void)
{
   //空调打开才允许设置
   if(ONOFF_State){
	SACT_FAN_Speed=SACT_FAN_Speed<<1;
	
	if(SACT_FAN_Speed == 0){
		SACT_FAN_Speed=LCD_FAN_AUTO;//0x02
	}
	Speed_change(SACT_FAN_Speed);
   }
}

void SACT_Mode_Switch(void)
{
   //空调打开才允许设置
   if(ONOFF_State){
   	SACT_Mode = SACT_Mode + 1;
	if(SACT_Mode==3){
		SACT_Mode=0;
	}
   }
}

void ONOFF_State_Switch(void)
{
	if (ONOFF_State==0){
		ONOFF_State=0xff;
	}else{
		ONOFF_State=0;
		BL_SetOff;
	}
	onoff_set(ONOFF_State);
}



//风速自动控制
void speed_auto(void)
{
	//空调打开
	if(ONOFF_State&&AC_On_Ctl){
		//Sact实际生效温度设定值-回滞温度，
		if((Thermal_Real < (Thermal_Set - Thermal_hyst) ) ){
            //设置为低风速
			speed_set_low();
		}else if ((Thermal_Real > ( Thermal_Set + Thermal_hyst) ) ){
            //设置为高风速
			speed_set_high();
		}
	}
}


uint8_t get_real_tmp()
{
    return Thermal_Real;
}

uint8_t get_set_tmp()
{
   return Thermal_Set;
}

uint8_t get_onoff()
{
   if(ONOFF_State){
      return SET_SWITCH_ON;
   }else{
      return SET_SWITCH_OFF;
   }
}

uint8_t get_mode()
{
   uint8_t ret = 0;
   switch(SACT_Mode){
   	   case 0:
	   	  ret = SET_MODE_WIND;
	      break;
       case 1:
	   	  ret = SET_MODE_COOL;
	      break;
	   case 2:
	   	  ret = SET_MODE_WARM;
	      break;
   }
   return ret;
}

uint8_t get_speed()
{
    uint8_t ret = 0;
    switch(SACT_FAN_Speed){
   	   case LCD_FAN_AUTO:
	   	  ret = SET_SPEED_AUTO;
	      break;
       case LCD_FAN_LOW:
	   	  ret = SET_SPEED_LOW;
		  break;
	   case LCD_FAN_MID:
	   	  ret = SET_SPEED_MID;
		  break;
	   case LCD_FAN_HIGH:
	   	  ret = SET_SPEED_HIGH;
		  break;
	   default:
	   	  break;
   }

   return ret;

}


	


uint get_temp_res(void)
{
	uint adc_value=0;
	uint8_t i=0;
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);   

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
	
    adc_value = ADC_GetConversionValue(ADC1);
	
	for(i=0;i<51;i++){
		if(adc_value<=Temp_Detect[i]){
			break;
		}
	}
	
	if(i!=0){
		return (i-1)*1000+1000*(adc_value-Temp_Detect[i-1])/(Temp_Detect[i]-Temp_Detect[i-1]);
	}else{
		return 0;
	}
}

void LCD_Set_Figure(u8 LCD_Figures)
{
	HT1621_CS_LOW();
	Ht1621WrDataCmd(8);    //往RAM的地址4写数据  这里的参数范围为:0-31
	 
	Ht1621WrByte(LCD_Figures >> 4 );
	HT1621_CS_HIGH();
	
	HT1621_CS_LOW();
	Ht1621WrDataCmd(9);    //往RAM的地址4写数据  这里的参数范围为:0-31
	Ht1621WrByte(LCD_Figures & 0x0F);
	HT1621_CS_HIGH();

	delay_us(HT1621_Delay); 
}

void LCD_Set_Temp(unsigned int temp, u8 position)
{

	HT1621_CS_LOW();
	Ht1621WrDataCmd(position*4);    //往RAM的地址4写数据  这里的参数范围为:0-31
	 
	Ht1621WrByte(Ht1621Tab[(temp/10) %10] >> 4 );
	HT1621_CS_HIGH();
	 
	delay_us(HT1621_Delay);  
	 
	HT1621_CS_LOW();
	Ht1621WrDataCmd(position*4+1);    //往RAM的地址4写数据  这里的参数范围为:0-31
	Ht1621WrByte(Ht1621Tab[(temp/10) %10] & 0x0F);
	HT1621_CS_HIGH();

	delay_us(HT1621_Delay); 
	
	HT1621_CS_LOW();
	Ht1621WrDataCmd(position*4+2);    //往RAM的地址4写数据  这里的参数范围为:0-31
	 
	Ht1621WrByte(Ht1621Tab[temp %10] >> 4 );
	HT1621_CS_HIGH();
	 
	delay_us(HT1621_Delay);  
	 
	HT1621_CS_LOW();
	Ht1621WrDataCmd(position*4+3);    //往RAM的地址4写数据  这里的参数范围为:0-31
	Ht1621WrByte(Ht1621Tab[temp %10] & 0x0F);
	HT1621_CS_HIGH();

	delay_us(HT1621_Delay); 
}

void LCD_Set_Temp_clear(u8 position)
{

	HT1621_CS_LOW();
	Ht1621WrDataCmd(position*4);    //往RAM的地址4写数据  这里的参数范围为:0-31
	if(ONOFF_State){
		Ht1621WrByte(8);
	}else{
		Ht1621WrByte(0x00);	
	} 
	HT1621_CS_HIGH();
	 
	delay_us(HT1621_Delay);  
	 
	HT1621_CS_LOW();
	Ht1621WrDataCmd(position*4+1);    //往RAM的地址4写数据  这里的参数范围为:0-31
	Ht1621WrByte(0x00);
	HT1621_CS_HIGH();

	delay_us(HT1621_Delay); 
	
	HT1621_CS_LOW();
	Ht1621WrDataCmd(position*4+2);    //往RAM的地址4写数据  这里的参数范围为:0-31
	 
	Ht1621WrByte(0x00);
	HT1621_CS_HIGH();
	 
	delay_us(HT1621_Delay);  
	 
	HT1621_CS_LOW();
	Ht1621WrDataCmd(position*4+3);    //往RAM的地址4写数据  这里的参数范围为:0-31
	Ht1621WrByte(0x00);
	HT1621_CS_HIGH();

	delay_us(HT1621_Delay); 
}



static void Key_Scan(void)
{
	static uint8_t tick = 0;
	static uint8_t onchange = 0;
	static uint8_t modechagne = 0;
	static uint8_t speedchange = 0;
	static uint8_t upchagne = 0;
	static uint8_t downchagne = 0;
	
	if( ON_OFF == IS_CLICK ||
		MODE == IS_CLICK ||
	    SPEED == IS_CLICK ||
	    UP == IS_CLICK ||
	    DOWN == IS_CLICK 
	){
		
	  BL_SetHigh;
	  bloff = 1;
	  blTick = gettick();

//	  delay_ms(500);
//	  update_lcd = 1;
	  
	  if ( ON_OFF == IS_CLICK ){
		  onchange = 1;
	  }else if( MODE == IS_CLICK){
		  if(!ONOFF_State && gIsConf){
			 delay_ms(1000);
		     tick ++;
			//  DEBUG("tick:%d\r\n",tick);
			 if(tick > REST_TIME){
				RESETALL(); 
				BL_SetOff;
				delay_ms(3000);
				systemRest();
			 }
              			  
		  }else{
		    tick = 0;
		  }
		  modechagne = 1;
	  }else if ( SPEED == IS_CLICK ){
		  speedchange = 1;
	  }else if ( UP == IS_CLICK ){
		  upchagne = 1;
	  }else if ( DOWN == IS_CLICK ){
		  downchagne = 1;
	  }else{
	      update_lcd = 0;
	  }
	  
	}else{
		if(onchange){
		  onchange = 0;
		  update_lcd = 1;
		  ONOFF_State_Switch();
		}
		
		if(modechagne){
		  modechagne = 0;
		  update_lcd = 1;
		  SACT_Mode_Switch();
		}
		
		if(speedchange){
		  speedchange = 0;
		  update_lcd = 1;
		  SACT_Speed_Change();
		}
		
		if(upchagne){
		  upchagne = 0;
		  update_lcd = 1;
		  SACT_Temp_up();
		}
		
		if(downchagne){
		  downchagne = 0;
		  update_lcd = 1;
		  SACT_Temp_down();
		}
        
       if(update_lcd){
		   //获取最后一次按键按下的时间
	      tickrep = gettick();
	   }		
	}	
}

void temp_disp_steady(void)
{
	uint temp_minus_tmp;
	uint temp_real_get = get_temp_res();

	if( temp_real_get > temp_change_store )
	{
		temp_minus_tmp = temp_real_get - temp_change_store;
	}
	else
	{
		temp_minus_tmp = temp_change_store - temp_real_get;
	}
	
	if( (temp_minus_tmp > 800) || (LCD_First_Disp) )
	{
		LCD_First_Disp=0;
		temp_real_toDisp = (temp_real_get+500) /1000;
		if(temp_real_toDisp != temp_buffer)//temp has changed
		{
			temp_change_store = temp_real_get;
		}
		else{}
	
		temp_buffer = temp_real_toDisp;
		//temp_real_get = temp_real_get/1000;
		Thermal_Real = (uint8_t)temp_real_toDisp;
		
		LCD_Set_Temp(temp_real_toDisp,Temp_Pos_Real);
	}
	else{}
		
}


void LCD_Disp_Update(void)
{
	static uint32_t upTick  = 0;
	static uint32_t Tick  = 0;
	static uint8_t flicker = 0;
	
	Key_Scan();
	
	
	if(bloff){
	   if((blTick+BLOFFTIME) < gettick()){
		   bloff = 0;
		   BL_SetOff;
	   }
	}
	
	if((upTick + GET_REAL_TMP) < gettick() ){
	   upTick = gettick();
	   temp_disp_steady();
	   speed_auto();
	}
	
	if(update_lcd){
		update_lcd = 0;
		LCD_Set_Temp(Thermal_Set,Temp_Pos_Set);
		LCD_Set_Temp(Thermal_Real,Temp_Pos_Real);
		LCD_Set_Figure( ONOFF_State & (LCD_FIX_DISP | SACT_Mode_Array[SACT_Mode] | SACT_FAN_Speed) );
			
	}
	if(ONOFF_State){
		if( gNetState == 0 ){
			
			Tick = (Tick + 1)%8001;
			if(Tick == 8000){
				if( flicker){
					//没有连接wifi，室内温度闪烁
					LCD_Set_Temp(Thermal_Real,Temp_Pos_Real);
					//没有配置wifi信息，设置温度闪烁
					if(gIsConf != 1){
						LCD_Set_Temp(Thermal_Set,Temp_Pos_Set);
					}
					flicker = 0;
				}else{	
					LCD_Set_Temp_clear(Temp_Pos_Real);
					if(gIsConf != 1){
						LCD_Set_Temp_clear(Temp_Pos_Set);
					}
					flicker = 1;
				}
			}
		 }else{
			 
			 if( flicker){
				LCD_Set_Temp(Thermal_Real,Temp_Pos_Real);
				LCD_Set_Temp(Thermal_Set,Temp_Pos_Set);
				flicker = 0;
			 }
		 
		 }
	}else{
		LCD_Set_Temp_clear(Temp_Pos_Set);
		LCD_Set_Temp_clear(Temp_Pos_Real);	
        flicker = 1;		
	}
}



	
void LCD_Disp_Init(void)
{
//	LCD_Set_Figure(LCD_FIX_DISP);
//	LCD_Set_Temp(Thermal_Set,Temp_Pos_Set);
////	LCD_Set_Temp(26,Temp_Pos_Real);
	temp_disp_steady();
	LCD_Set_Temp_clear(Temp_Pos_Set);
	LCD_Set_Temp_clear(Temp_Pos_Real);	
	LCD_Set_Figure( ONOFF_State & (LCD_FIX_DISP | SACT_Mode_Array[SACT_Mode] | SACT_FAN_Speed) );

}



