#include "systick.h"
#include "misc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_tim.h"
#include "flash.h"
#include "Esp8266.h"

#define Second_Handler TIM4_IRQHandler

static volatile uint32_t Systick = 0;


void Second_Handler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) 			//溢出中断
	{
		Systick++;
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);				//清除中断标志位
	
	}
}

/*****************************************************
*func: 定时器配置
*Parma：period  重装值
*parma: prescaler 分频值
*  Tout = (period+1)*(prescaler+1) / TCLK    tclk = 32M
*
*****************************************************/
void TIM4_Init2sec(u16 period, u16 prescaler)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	//使能TIM4时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);		
	
	//TIM4中断
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	
    //先占优先级1级	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    //从优先级1级	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    //IRQ通道被使能	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
    //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	
	NVIC_Init(&NVIC_InitStructure);								
	
	//总的值设置为(period-1)，设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_Period = period-1;
    //预分频器	
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler-1;	
    //设置时钟分割:TDTS = Tck_tim	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    //TIM向上计数模式	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);				
	//开启定时器
	TIM_Cmd(TIM4, ENABLE);
    //开启定时器更新中断	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);					
}



void Systick_Init(void)
{

    RCC_DeInit();
    RCC_LSICmd(ENABLE);

	while ((RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)){

	}

	//RCC_HSEConfig(RCC_HSE_ON);
    RCC_HSICmd(ENABLE);

    //检查PLL准备好标志设置与否
    while( RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET ){

	}

    //使能预取指缓存
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	
    //设置FLASH存储器延时时钟周期数
    FLASH_SetLatency(FLASH_Latency_2);

	//   0 wait state if   0MHz < SYSCLK <= 24MHz
    //   1 wait state if 24MHz < SYSCLK <= 48MHz
	//   2 wait state if 48MHz < SYSCLK <= 72MHz

    //AHB时钟=系统时钟
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
	
    //高速AHB时钟=AHB时钟=系统时钟=64M
    RCC_PCLK2Config(RCC_HCLK_Div1);

	//高速AHB时钟=AHB时钟/4,=16M
	RCC_PCLK1Config(RCC_HCLK_Div4);

	//设置PLL时钟源及倍频系数,64MHz
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_16);

	//使能PLL
	RCC_PLLCmd(ENABLE);

	//检查PLL准备好标志设置与否
	while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLLRDY)){

	}

	//选择PLL做为系统时钟，SYSCLK=(HSI/2)*16=64MHz
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	//判断PLL是否是系统时钟
	while(0x08 != RCC_GetSYSCLKSource()){

	}
	
	//生成1秒中断
	TIM4_Init2sec(1000,32000);

}

uint32_t gettick(void)
{
	return Systick;

}

void settick(uint32_t Value)
{
	Systick = Value;

}


void delay_us(uint16_t Time)  
{  
    uint8_t i;  
      
    while(Time --) for(i = 0; i < 8; i ++);  
}  
  
void delay_ms(uint16_t Time)  
{  
    while(Time --) delay_us(1000);  
}  

#if 0
void delay_us(volatile u32 nus)
{
	while(nus--)
	{
		__NOP();
	}
}

/**********************************************/
void delay_ms(volatile u16 nms)
{
  volatile u16 i=0;
	while(nms--)
	{
		i=800;
		while(i--)
		{
			__NOP();
		}
	}
 }
#endif
void systemRest(void)
{
	 ESP8266_ExitUnvarnishSend ();
	 ESP8266_Rst();
	 delay_ms(1000);
	 __NVIC_SystemReset();
}

void RESETALL(void)
{
	uint8_t isConf = 0;
	flash_write(ISCONFIG, (uint8_t*)&isConf, sizeof(isConf));
	ESP8266_Rstore();
}


