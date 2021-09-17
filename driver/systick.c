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
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) 			//����ж�
	{
		Systick++;
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);				//����жϱ�־λ
	
	}
}

/*****************************************************
*func: ��ʱ������
*Parma��period  ��װֵ
*parma: prescaler ��Ƶֵ
*  Tout = (period+1)*(prescaler+1) / TCLK    tclk = 32M
*
*****************************************************/
void TIM4_Init2sec(u16 period, u16 prescaler)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	//ʹ��TIM4ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);		
	
	//TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	
    //��ռ���ȼ�1��	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    //�����ȼ�1��	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    //IRQͨ����ʹ��	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
    //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	
	NVIC_Init(&NVIC_InitStructure);								
	
	//�ܵ�ֵ����Ϊ(period-1)���趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_Period = period-1;
    //Ԥ��Ƶ��	
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler-1;	
    //����ʱ�ӷָ�:TDTS = Tck_tim	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    //TIM���ϼ���ģʽ	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);				
	//������ʱ��
	TIM_Cmd(TIM4, ENABLE);
    //������ʱ�������ж�	
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

    //���PLL׼���ñ�־�������
    while( RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET ){

	}

    //ʹ��Ԥȡָ����
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	
    //����FLASH�洢����ʱʱ��������
    FLASH_SetLatency(FLASH_Latency_2);

	//   0 wait state if   0MHz < SYSCLK <= 24MHz
    //   1 wait state if 24MHz < SYSCLK <= 48MHz
	//   2 wait state if 48MHz < SYSCLK <= 72MHz

    //AHBʱ��=ϵͳʱ��
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
	
    //����AHBʱ��=AHBʱ��=ϵͳʱ��=64M
    RCC_PCLK2Config(RCC_HCLK_Div1);

	//����AHBʱ��=AHBʱ��/4,=16M
	RCC_PCLK1Config(RCC_HCLK_Div4);

	//����PLLʱ��Դ����Ƶϵ��,64MHz
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_16);

	//ʹ��PLL
	RCC_PLLCmd(ENABLE);

	//���PLL׼���ñ�־�������
	while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLLRDY)){

	}

	//ѡ��PLL��Ϊϵͳʱ�ӣ�SYSCLK=(HSI/2)*16=64MHz
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	//�ж�PLL�Ƿ���ϵͳʱ��
	while(0x08 != RCC_GetSYSCLKSource()){

	}
	
	//����1���ж�
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


