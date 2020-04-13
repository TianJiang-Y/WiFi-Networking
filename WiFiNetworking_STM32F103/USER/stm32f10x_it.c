/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "delay.h"
#include "usart.h"
#include "usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
#include "GPIOConf.h"
#include "ESP8266Modules.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
//}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	

u8 USART1_RX_BUF[USART1_MAX_RECV_LEN];     //接收缓冲,最大USART1_MAX_RECV_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA=0;       //接收状态标记	  

  
void USART1_IRQHandler(void)
{
	u8 res1;	
//	int i=0;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	
	if(USART1->SR&(1<<5))	//接收到数据
	{	 
		res1=USART1->DR; 			 
		if((USART1_RX_STA&(1<<15))==0)		//还可以接收数据
		{
			TIM3->CNT=0;         					//计数器清空
			if(USART1_RX_STA==0)
				TIM3_Set(1);	 	//使能定时器3的中断 
			USART1_RX_BUF[USART1_RX_STA++]=res1;		//记录接收到的值	 
		}
		else 
		{
			USART1_RX_STA|=1<<15;					//强制标记接收完成
		} 
	}
	
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
#endif	



#if USART2_RX_EN

//////u8  USART2_RX_BUF[USART2_MAX_RECV_LEN];
////////接收状态
////////bit15，	接收完成标志
////////bit14，	接收到0x0d
////////bit13~0，	接收到的有效字节数目

//////u16 USART2_RX_STA;   						//接收数据状态

u8 RecvStart;

//void USART2_IRQHandler(void)
//{
//	u8 res;	    
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
//	{	 
//		res = USART_ReceiveData(USART2); 			 
//		 
//		USART2_RX_BUF[USART2_RX_STA++] = res;
//		
//		if(USART2_RX_STA == 45)
//		{		
//			RecvFinishFlag = 1;			
//			LED1_TOGGLE;
//		}
//	}
//	
//	if ( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )                                         //???????
//	{
//		USART_ClearFlag(USART2,USART_FLAG_ORE);  //Read SR 
//		USART_ReceiveData(USART2); //Read DR 
//        RecvFinishFlag = 1;
//		
//		res = USART_ReceiveData( USART2 );                      
//  }
//}

void USART2_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
	{
		res = USART_ReceiveData(USART2);

		if(RecvStart)
			{
					USART2_RX_BUF[USART2_RX_STA] = res;
					USART2_RX_STA++;
					
//					if((USART2_RX_BUF[41] == 0xFF)&&(USART2_RX_BUF[42] == 0xFF)&&
//						 (USART2_RX_BUF[43] == 0xFF)&&(USART2_RX_BUF[44] == 0xFF))
						if(USART2_RX_STA > 38)
						{
								RecvFinishFlag = 1;
								USART2_RX_STA = 0;
//								LED1_TOGGLE;
						}
			}

		if((RecvStart == 0)&&(res == 0x23))
			{
				RecvStart = 1;
//				USART2_RX_BUF[0] = res;
				USART2_RX_STA = 0;
			}
	}
	
	if(USART_GetITStatus(USART2, USART_IT_IDLE) == SET)
	{
		USART_ClearFlag(USART2,USART_FLAG_ORE);  //Read SR 
		USART_ReceiveData(USART2); //Read DR 
		
		RecvFinishFlag = 1;
		
//		res = USART_ReceiveData( USART2 );
  }
}

#endif

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
