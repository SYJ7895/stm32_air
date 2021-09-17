/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : stm32f10x_it.c
* Author             : MCD Application Team
* Version            : V2.0.0
* Date               : 04/27/2009
* Description        : Main Interrupt Service Routines.
*                      This file provides template for all exceptions handler
*                      and peripherals interrupt service routine.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rtc.h"
#include "misc.h"
#include "systick.h"
#include "ctrlbsp.h"

unsigned char USART2_ReceiveBuffer[200];
unsigned char U2_Package_Start=0;
unsigned char U2_Package_End=0;
unsigned char Package_ReceivedOk[20];
unsigned char Package_Num=0;
unsigned char Package_ByteNum[20];
unsigned char U2_B0x10_Flag=0;
unsigned char U2_Received_ByteNum=0;
unsigned char Received_BufferNum=0;

unsigned char Send_CheckSum;

unsigned char USART2_SendNum=0;
unsigned char U2_Received_Ok=0;
unsigned char U2_Received_CheckSum;

unsigned char U1_0x10Flag=0;
unsigned char U1_Package_Start=0;
unsigned char U1_Package_End=0;
unsigned char USART1_Received[50];
unsigned char U1_Received_Ok=0;
unsigned char U1_Received_ByteNum=0;
unsigned char U1_Received_CheckSum;
unsigned char USART1_SendNum=0;

volatile unsigned char TIM2_Timeup=0;

unsigned char USART_Case=0;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Index = 0;
static __IO uint32_t AlarmStatus = 0;
static __IO uint32_t LedCounter = 0;


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void NMI_Handler(void)
{
	  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void HardFault_Handler(void)
//{
//  /* Go to infinite loop when Hard Fault exception occurs */
//  while (1)
//  {
//  }
//}

/*******************************************************************************
* Function Name  : MemManage_Handler
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : BusFault_Handler
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : UsageFault_Handler
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : SVC_Handler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void)
{
}

/*******************************************************************************
* Function Name  : DebugMon_Handler
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMon_Handler(void)
{
}

/*******************************************************************************
* Function Name  : PendSV_Handler
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void PendSV_Handler(void)
//{
//}

///*******************************************************************************
//* Function Name  : SysTick_Handler
//* Description    : This function handles SysTick Handler.
//* Input          : None
//* Output         : None
//* Return         : None
//*******************************************************************************/
//void SysTick_Handler(void)
//{
//  /* Decrement the TimingDelay variable */
// // Decrement_TimingDelay();
//}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void)
{
 
}


/*******************************************************************************
* Function Name  : USB_HP_CAN1_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_CAN1_TX_IRQHandler(void)
{
 // CTR_HP();
}

/*******************************************************************************
* Function Name  : USB_LP_CAN1_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
 // USB_Istr();
}
/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{}
	
	
void EXTI9_5_IRQHandler(void)
{    if (EXTI_GetITStatus(EXTI_Line7) != RESET)
	      {//I2C_SendData(I2C1,0x00);
				}

}
void EXTI0_IRQHandler(void)
{
	/*uint8_t i;
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	      {Delay_ms(5) ;
					 if (GPIO_ReadInputDataBit(GPIOE ,GPIO_Pin_0) == 0)
					    {
							 for(i=0 ;i<8 ;i++)  
                  { 
										GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15) ;
										GPIO_ResetBits(GPIOE,(GPIO_Pin_8<<i)) ; Delay(10) ;
                    if (GPIO_ReadInputDataBit(GPIOE ,GPIO_Pin_0) == 0)
                      {   Keyboard_Val = i ;
                          Keyboard_Change_Flag = 1 ;
                          break ;
                      } 
			            }
					    }
				GPIO_ResetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15) ;
				while(GPIO_ReadInputDataBit(GPIOE ,GPIO_Pin_0) == 0){};			
				EXTI_ClearFlag(EXTI_Line0); 
				}*/

}
void EXTI1_IRQHandler(void)
{
	/*uint8_t i;
  	if (EXTI_GetITStatus(EXTI_Line1) != RESET)
	      {Delay_ms(5) ;
					 if (GPIO_ReadInputDataBit(GPIOE ,GPIO_Pin_1) == 0)
					    {EXTI_ClearFlag(EXTI_Line1);
							 for(i=0 ;i<8 ;i++)  
                  { 
										GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15) ;
										GPIO_ResetBits(GPIOE,(GPIO_Pin_8<<i)) ; Delay(10) ;
                    if (GPIO_ReadInputDataBit(GPIOE ,GPIO_Pin_1) == 0)
                      {   Keyboard_Val = 8+i ;
                          Keyboard_Change_Flag = 1 ;
                          break ;
                      } 
			            }
					    }
				GPIO_ResetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15) ;
				while(GPIO_ReadInputDataBit(GPIOE ,GPIO_Pin_1) == 0){};			
				EXTI_ClearFlag(EXTI_Line1);
				}*/

}
void EXTI2_IRQHandler(void)
{
	/*uint8_t i;
  	if (EXTI_GetITStatus(EXTI_Line2) != RESET)
	      {Delay_ms(5) ;
					 if (GPIO_ReadInputDataBit(GPIOE ,GPIO_Pin_2) == 0)
					    {
							 for(i=0 ;i<8 ;i++)  
                  { 
										GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15) ;
										GPIO_ResetBits(GPIOE,(GPIO_Pin_8<<i)) ; Delay(50) ;
                    if (GPIO_ReadInputDataBit(GPIOE ,GPIO_Pin_2) == 0)
                      {   Keyboard_Val = 16+i ;
                          Keyboard_Change_Flag = 1 ;
                          break ;
                      } 
			            }
					    }
				GPIO_ResetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15) ;
				while(GPIO_ReadInputDataBit(GPIOE ,GPIO_Pin_2) == 0){};			
				EXTI_ClearFlag(EXTI_Line2); 
				}*/

}
void EXTI3_IRQHandler(void)
{
	/*uint8_t i;
  	if (EXTI_GetITStatus(EXTI_Line3) != RESET)
	      {Delay_ms(5) ;
					 if (GPIO_ReadInputDataBit(GPIOE ,GPIO_Pin_3) == 0)
					    {
							 for(i=0 ;i<8 ;i++)  
                  { 
										GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15) ;
										GPIO_ResetBits(GPIOE,(GPIO_Pin_8<<i)) ; Delay(50) ;
                    if (GPIO_ReadInputDataBit(GPIOE ,GPIO_Pin_3) == 0)
                      {   Keyboard_Val = 24+i ;
                          Keyboard_Change_Flag = 1 ;
                          break ;
                      } 
			            }
					    }
				GPIO_ResetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15) ;
				while(GPIO_ReadInputDataBit(GPIOE ,GPIO_Pin_3) == 0){};			
				EXTI_ClearFlag(EXTI_Line3); 
				}*/
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
	
	/*if(TIM_GetITStatus(TIM1,TIM_IT_Update)!=RESET)//判断中断是否产生
   {
     MusicSend_On=1;			
     TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);//清除中断标志位
   }*/
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
   if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)//判断中断是否产生
   {
		 TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);//清除中断标志位
		TIM2_Timeup=1;
   }
	 else{}
}
/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)//判断中断是否产生
   {
		 TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);//清除中断标志位
    			
     
   }else{}
}





/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_IRQHandler(void)
{

}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{
	
}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{

} 
///*******************************************************************************
//* Function Name  : USART1_IRQHandler
//* Description    : This function handles USART1 global interrupt request.
//* Input          : None
//* Output         : None
//* Return         : None
//*******************************************************************************/
//void USART1_IRQHandler(void)
//{
//	
//	
//}



/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTCAlarm_IRQHandler(void)
{


}

/*******************************************************************************
* Function Name  : SDIO_IRQHandler
* Description    : This function handles SDIO global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SDIO_IRQHandler(void)
{
  /* Process All SDIO Interrupt Sources */
 // SD_ProcessIRQSrc();
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/*******************************************************************************
* Function Name  : PPP_IRQHandler
* Description    : This function handles PPP interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*void PPP_IRQHandler(void)
{
}*/

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
