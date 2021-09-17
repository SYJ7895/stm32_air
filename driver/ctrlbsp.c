/**
********************************************************************************
  * @file    ctrlbsp.c
  * @author  gjf
  * @version V1.0.0
  * @date    2017-3-15
  * @brief   
控制板板级支持，初始化，各种硬件接口配置等

	
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
 * 函数名：USART_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口2，即USART2
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART2_printf( USART2, "\r\n this is a demo \r\n" );
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

	while ( * Data != '\0' )     // 判断是否到达字符串结束符
	{				                          
		if ( * Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
				USART_SendData(USARTx, 0x0d);
				Data ++;
				break;

				case 'n':							          //换行符
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
				case 's':										  //字符串
				s = va_arg(ap, const char *);
				
				for ( ; *s; s++) 
				{
					USART_SendData(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				
				Data++;
				
				break;

				case 'd':			
					//十进制
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
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART2_printf()调用
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

   const char *ptr = str;  //ptr保存str字符串开头

   if(*str == '-' || *str == '+'){ //如果第一个字符是正负号，则移到下一个字符
      str++;
   }

   while(*str != 0){
   	  //如果当前字符不是数字,则退出循环
   	 if((*str < '0') || (*str > '9')){
	 	break;
	 }

	 temp = temp * 10 + (*str - '0'); //如果当前字符是数字则计算数值
	 str++;      //移到下一个字符

   } 
   
   //如果字符串是以“-”开头，则转换成其相反数
   if(*ptr == '-'){
   	   temp = -temp;
   }

   return temp;
}


/**********************************************/
void GPIO_Config(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	
    //使能GPIO时钟
	uint32_t rrc_apb2 =  RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD ;
	RCC_APB2PeriphClockCmd(rrc_apb2,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
	
	//======================按键GPIO初始化==========================//
	
	//设置GPIO引脚为输入引脚 ,PA2 --->  ON/OFF
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//设置GPIO引脚为输入引脚 ,PB8 --->  MODE
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	//设置GPIO引脚为输入引脚 ,PB9 --->  SPEED
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	//设置GPIO引脚为输入引脚 ,PC13 --->  UP
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	//设置GPIO引脚为输入引脚 ,PA15 --->  DOWN
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//======================HT1621 GPIO初始化==========================//
	
	//设置GPIO引脚为输出引脚 ,PB4 --->  LCD_CS  PB5---> LCD_BL  PB6 -->LCD_WR  PB7---->LCD_DATD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//======================ADC GPIO初始化==========================//
	// PA0 -->TMP_DQ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	//======================ESP8266 USART GPIO初始化==========================//
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
	//======================DEBUG USART3 GPIO初始化==========================//
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
    //====================== GPIO 模拟I2C 初始化==========================//
	// PA3 ---> SCL  PA4 ---> SDL
	GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_Out_OD ;
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
#endif
    //====================== GPIO 空调开关风速控制 初始化==========================//
	//PA1---->AC_DET PA11 ---> RELAY_LOW  PA12 ---> RELAY_OPEN  PB3----->RELAY_MID 
	GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_1 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_Out_OD ;
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_Out_OD ;
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
    //设置为低电平，关闭状态
	GPIO_ResetBits(GPIOA,GPIO_Pin_12);
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);


}

/**********************************************/
void ADC_Config(void)
{
	//定义ADC初始化结构体变量
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	////设置 ADC 分频因子 8
    //64M/8=8,ADC 最大时间不能超过 14M
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);  
	
	//初始化ADC寄存器
	ADC_DeInit(ADC1);
	
	//工作在独立模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    //单通道模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    //单次转换
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    //软件控制转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    //右对齐方式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    //通道数量为1
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	
	ADC_Init(ADC1, &ADC_InitStructure); 

    //ADC1选择信道0，音序器等级1，采样时间为239.5个周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_28Cycles5);
	
	//失能ADC1模块DMA
	ADC_DMACmd(ADC1, DISABLE);
    //使能ADC1
    ADC_Cmd(ADC1, ENABLE);
	//重置ADC1校准寄存器
	ADC_ResetCalibration(ADC1); 
    //等待ADC1校准重置完成
    while(ADC_GetResetCalibrationStatus(ADC1)){
	
	}
	//开始ADC1校准
	ADC_StartCalibration(ADC1);
	//等待ADC1校准完成
	while(ADC_GetCalibrationStatus(ADC1)){
	
	}
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能ADC1软件开始转换
}



void TIME_Config(void)
{

    //配置TIMER3
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;                   
	TIM_OCInitTypeDef       TIM_OCInitStructure;                   
	
	//开定时器3时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
	
	//初始化定时器3
	TIM_DeInit(TIM3); 
    //重装值 计数到最大100-1 然后从0开始	
	TIM_TimeBaseStructure.TIM_Period = (100 -1);
    //分频系数	
	TIM_TimeBaseStructure.TIM_Prescaler = (32-1); 
    //时钟分割	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ; 
    //向上计数	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;      
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;    
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
	
	//配置TIMER2PWM输出 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    //比较输出使能	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
    //输出极性高 输出高电平	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	
    //通道2  ---> PB5 ---> LCD_BL  背光设置
    //设置占空比	
	TIM_OCInitStructure.TIM_Pulse = 20; 
    //初始化外设TIMx 通道	
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);         
	//CH1预装载使能
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  
	
    //使能TIMx在ARR上的预装载寄存器
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    //使能定时器3	
	TIM_Cmd(TIM3, ENABLE);   
	
}


void USART3_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	
	/* 使能USART3的时钟 */
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




/*************************************指示灯控制开始********************************/			
void CTRLBSP_Init(void)
{
	SystemInit();
	
	//系统时钟设置到64M
	Systick_Init();

	//重定向中断向量表
	NVIC_SetVectorTable(SOFT_ADDR+512,0);
	
	//所有GPIO引脚功能定义初始化
	GPIO_Config();
	
	//温度传感器ADC控制
	ADC_Config();
	
	//LCD背光控制定时器
    TIME_Config();
	
	#ifdef __DEBUG
	 USART3_Init();
	#endif
		
}

