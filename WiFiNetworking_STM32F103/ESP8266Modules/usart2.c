#include "delay.h"
#include "usart.h"
#include "usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
#include "GPIOConf.h"
#include "ESP8266Modules.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����2��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//���ڷ��ͻ����� 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ� 	  
//���ڽ��ջ����� 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.

u16 USART2_RX_STA=0;   	 

u8 RecvFinishFlag; 
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void USART2_Init(u32 pclk1,u32 bound)
{  	 		 
	RCC->APB2ENR|=1<<2;   	//ʹ��PORTA��ʱ��  
	GPIOA->CRL&=0XFFFF00FF;	//IO״̬����
	GPIOA->CRL|=0X00008B00;	//IO״̬����	 
	RCC->APB1ENR|=1<<17;  	//ʹ�ܴ���ʱ�� 	 
	RCC->APB1RSTR|=1<<17;   //��λ����2
	RCC->APB1RSTR&=~(1<<17);//ֹͣ��λ	   	   
	//����������
 	USART2->BRR=(pclk1*1000000)/(bound);// ����������	 

	USART2->CR1|=0X200C;  	//1λֹͣ,��У��λ.
	//ʹ�ܽ����ж� 
	USART2->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	
	MY_NVIC_Init(3,3,USART2_IRQn,2);//��2
	
	USART2_RX_STA=0;		//����							
	USART_Cmd(USART2, DISABLE);
	
	Hum_TX_Disable;
	STM_TX_Disable;
}

void Usart2_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
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
  
  /* �ȴ�������� */
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
  
  /* �ȴ�������� */
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
	/* ʹ�ܴ���1�����ж� */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void Usart2_RxIT_Disable(void)
{
	/* ʹ�ܴ���1�����ж� */
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
}
	   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 									 





















