/**
********************************************************************************
  * @file    ctrlbsp.c
  * @author  gjf
  * @version V1.0.0
  * @date    2017-3-15
  * @brief   
���ư�弶֧�֣���ʼ��������Ӳ���ӿ����õ�

	
  ******************************************************************************

  ******************************************************************************
  */


	/* Includes ------------------------------------------------------------------*/
#include "ctrlbsp.h"

#include "stm32f10x_exti.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_pwr.h"
#include <stdarg.h>
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "partition.h"




static char *                 itoa                                ( int value, char * string, int radix );

/*
 * ��������USART_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-USARTx ����ͨ��������ֻ�õ��˴���2����USART2
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 *         ����Ӧ��USART2_printf( USART2, "\r\n this is a demo \r\n" );
 *            		 USART2_printf( USART2, "\r\n %d \r\n", i );
 *            		 USART2_printf( USART2, "\r\n %s \r\n", j );
 */
void USART_printf ( USART_TypeDef * USARTx, char * Data, ... )
{
	const char *s;
	int d;   
	char buf[16];

	
	va_list ap;
	va_start(ap, Data);

	while ( * Data != '\0' )     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( * Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //�س���
				USART_SendData(USARTx, 0x0d);
				Data ++;
				break;

				case 'n':							          //���з�
				USART_SendData(USARTx, 0x0a);	
				Data ++;
				break;

				default:
				Data ++;
				break;
			}			 
		}
		
		else if ( * Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //�ַ���
				s = va_arg(ap, const char *);
				
				for ( ; *s; s++) 
				{
					USART_SendData(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				
				Data++;
				
				break;

				case 'd':			
					//ʮ����
				d = va_arg(ap, int);
				
				itoa(d, buf, 10);
				
				for (s = buf; *s; s++) 
				{
					USART_SendData(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				
				Data++;
				
				break;
				
				default:
				Data++;
				
				break;
				
			}		 
		}
		
		else USART_SendData(USARTx, *Data++);
		
		while ( USART_GetFlagStatus ( USARTx, USART_FLAG_TXE ) == RESET );
		
	}
}


/*
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����USART2_printf()����
 */
static char * itoa( int value, char *string, int radix )
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;

	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
		*ptr = 0;
		return string;
	}

	if (!value)
	{
		*ptr++ = 0x30;
		*ptr = 0;
		return string;
	}

	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
		*ptr++ = '-';

		/* Make the value positive. */
		value *= -1;
		
	}

	for (i = 1000000000; i > 0; i /= 10)
	{
		d = value / i;

		if (d || flag)
		{
			*ptr++ = (char)(d + 0x30);
			value -= (d * i);
			flag = 1;
		}
	}

	/* Null terminate the string. */
	*ptr = 0;

	return string;

} /* NCL_Itoa */


int str2int(const char *str)
{
   int temp = 0;

   const char *ptr = str;  //ptr����str�ַ�����ͷ

   if(*str == '-' || *str == '+'){ //�����һ���ַ��������ţ����Ƶ���һ���ַ�
      str++;
   }

   while(*str != 0){
   	  //�����ǰ�ַ���������,���˳�ѭ��
   	 if((*str < '0') || (*str > '9')){
	 	break;
	 }

	 temp = temp * 10 + (*str - '0'); //�����ǰ�ַ��������������ֵ
	 str++;      //�Ƶ���һ���ַ�

   } 
   
   //����ַ������ԡ�-����ͷ����ת�������෴��
   if(*ptr == '-'){
   	   temp = -temp;
   }

   return temp;
}


/**********************************************/
void GPIO_Config(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	
    //ʹ��GPIOʱ��
	uint32_t rrc_apb2 =  RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD ;
	RCC_APB2PeriphClockCmd(rrc_apb2,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
	
	//======================����GPIO��ʼ��==========================//
	
	//����GPIO����Ϊ�������� ,PA2 --->  ON/OFF
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//����GPIO����Ϊ�������� ,PB8 --->  MODE
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	//����GPIO����Ϊ�������� ,PB9 --->  SPEED
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	//����GPIO����Ϊ�������� ,PC13 --->  UP
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	//����GPIO����Ϊ�������� ,PA15 --->  DOWN
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//======================HT1621 GPIO��ʼ��==========================//
	
	//����GPIO����Ϊ������� ,PB4 --->  LCD_CS  PB5---> LCD_BL  PB6 -->LCD_WR  PB7---->LCD_DATD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//======================ADC GPIO��ʼ��==========================//
	// PA0 -->TMP_DQ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	//======================ESP8266 USART GPIO��ʼ��==========================//
	// PA9 ---> USART1_TX  PA10 ---> USART_RX
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
	/* Configure USART1 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

#ifdef __DEBUG
	//======================DEBUG USART3 GPIO��ʼ��==========================//
	// PB10 ---> USART3_TX  PA10 ---> USART_RX
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
  
	/* Configure USART3 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
#endif

#ifdef USE_EEPROM
    //====================== GPIO ģ��I2C ��ʼ��==========================//
	// PA3 ---> SCL  PA4 ---> SDL
	GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_Out_OD ;
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
#endif
    //====================== GPIO �յ����ط��ٿ��� ��ʼ��==========================//
	//PA1---->AC_DET PA11 ---> RELAY_LOW  PA12 ---> RELAY_OPEN  PB3----->RELAY_MID 
	GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_1 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_Out_OD ;
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_Out_OD ;
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
    //����Ϊ�͵�ƽ���ر�״̬
	GPIO_ResetBits(GPIOA,GPIO_Pin_12);
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);


}

/**********************************************/
void ADC_Config(void)
{
	//����ADC��ʼ���ṹ�����
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	////���� ADC ��Ƶ���� 8
    //64M/8=8,ADC ���ʱ�䲻�ܳ��� 14M
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);  
	
	//��ʼ��ADC�Ĵ���
	ADC_DeInit(ADC1);
	
	//�����ڶ���ģʽ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    //��ͨ��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    //����ת��
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    //�������ת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    //�Ҷ��뷽ʽ
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    //ͨ������Ϊ1
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	
	ADC_Init(ADC1, &ADC_InitStructure); 

    //ADC1ѡ���ŵ�0���������ȼ�1������ʱ��Ϊ239.5������
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_28Cycles5);
	
	//ʧ��ADC1ģ��DMA
	ADC_DMACmd(ADC1, DISABLE);
    //ʹ��ADC1
    ADC_Cmd(ADC1, ENABLE);
	//����ADC1У׼�Ĵ���
	ADC_ResetCalibration(ADC1); 
    //�ȴ�ADC1У׼�������
    while(ADC_GetResetCalibrationStatus(ADC1)){
	
	}
	//��ʼADC1У׼
	ADC_StartCalibration(ADC1);
	//�ȴ�ADC1У׼���
	while(ADC_GetCalibrationStatus(ADC1)){
	
	}
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ʹ��ADC1�����ʼת��
}



void TIME_Config(void)
{

    //����TIMER3
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;                   
	TIM_OCInitTypeDef       TIM_OCInitStructure;                   
	
	//����ʱ��3ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
	
	//��ʼ����ʱ��3
	TIM_DeInit(TIM3); 
    //��װֵ ���������100-1 Ȼ���0��ʼ	
	TIM_TimeBaseStructure.TIM_Period = (100 -1);
    //��Ƶϵ��	
	TIM_TimeBaseStructure.TIM_Prescaler = (32-1); 
    //ʱ�ӷָ�	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ; 
    //���ϼ���	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;      
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;    
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
	
	//����TIMER2PWM��� 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    //�Ƚ����ʹ��	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
    //������Ը� ����ߵ�ƽ	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	
    //ͨ��2  ---> PB5 ---> LCD_BL  ��������
    //����ռ�ձ�	
	TIM_OCInitStructure.TIM_Pulse = 20; 
    //��ʼ������TIMx ͨ��	
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);         
	//CH1Ԥװ��ʹ��
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  
	
    //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    //ʹ�ܶ�ʱ��3	
	TIM_Cmd(TIM3, ENABLE);   
	
}


void USART3_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	
	/* ʹ��USART3��ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO ,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	USART_DeInit(USART3);

	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 57600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	
	USART_Init(USART3, &USART_InitStructure);
		
	
	USART_Cmd(USART3, ENABLE);
	USART_ClearFlag(USART3, USART_FLAG_TC);
	
	DEBUG("1");

}




/*************************************ָʾ�ƿ��ƿ�ʼ********************************/			
void CTRLBSP_Init(void)
{
	SystemInit();
	
	//ϵͳʱ�����õ�64M
	Systick_Init();

	//�ض����ж�������
	NVIC_SetVectorTable(SOFT_ADDR+512,0);
	
	//����GPIO���Ź��ܶ����ʼ��
	GPIO_Config();
	
	//�¶ȴ�����ADC����
	ADC_Config();
	
	//LCD������ƶ�ʱ��
    TIME_Config();
	
	#ifdef __DEBUG
	 USART3_Init();
	#endif
		
}

