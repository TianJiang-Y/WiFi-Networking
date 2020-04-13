#include "delay.h"
#include "usart.h"
#include "usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
#include "GPIOConf.h"
#include "ESP8266Modules.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口2驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//串口发送缓存区 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节 	  
//串口接收缓存区 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.

u16 USART2_RX_STA=0;   	 

u8 RecvFinishFlag; 
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void USART2_Init(u32 pclk1,u32 bound)
{  	 		 
	RCC->APB2ENR|=1<<2;   	//使能PORTA口时钟  
	GPIOA->CRL&=0XFFFF00FF;	//IO状态设置
	GPIOA->CRL|=0X00008B00;	//IO状态设置	 
	RCC->APB1ENR|=1<<17;  	//使能串口时钟 	 
	RCC->APB1RSTR|=1<<17;   //复位串口2
	RCC->APB1RSTR&=~(1<<17);//停止复位	   	   
	//波特率设置
 	USART2->BRR=(pclk1*1000000)/(bound);// 波特率设置	 

	USART2->CR1|=0X200C;  	//1位停止,无校验位.
	//使能接收中断 
	USART2->CR1|=1<<5;    	//接收缓冲区非空中断使能	
	MY_NVIC_Init(3,3,USART2_IRQn,2);//组2
	
	USART2_RX_STA=0;		//清零							
	USART_Cmd(USART2, DISABLE);
	
	Hum_TX_Disable;
	STM_TX_Disable;
}

void Usart2_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

void Usart2_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart2_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

void Usart2_SendByteArr( USART_TypeDef * pUSARTx, uint8_t *ByteArr, uint8_t len)
{
	unsigned int k=0;
	
	for(k = 0; k < len; k++)
	{
      Usart2_SendByte(pUSARTx, *(ByteArr++));
	}
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

void USART2_FirstConfig(void)
{
	Hum_TX_Enable;
	STM_TX_Enable;
	delay_ms(20);
	
	USART_Cmd(USART2, ENABLE);		
	RecvFinishFlag = 0;
	USART2_RX_STA = 0;	
	delay_ms(10);
}

void Usart2_RxIT_Enable(void)
{
	/* 使能串口1接收中断 */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void Usart2_RxIT_Disable(void)
{
	/* 使能串口1接收中断 */
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
}
	   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 									 





















