#include "usart.h"	
#include "usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "delay.h"
#include "sys.h"
#include "GPIOConf.h"
#include "ESP8266Modules.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ�����ʺ�STM32F10xϵ�У�		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2010/1/1
//�汾��V1.7
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
//V1.6�޸�˵�� 20150109
//uart_init����ȥ���˿���PE�ж�
//V1.7�޸�˵�� 20150322
//�޸�OS_CRITICAL_METHOD���ж�Ϊ��SYSTEM_SUPPORT_OS
////////////////////////////////////////////////////////////////////////////////// 	  
 
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc����
//printf�������ָ��fputc����fputc���������
//����ʹ�ô���1(USART1)���printf��Ϣ
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//�ȴ���һ�δ������ݷ������  
	USART1->DR = (u8) ch;      	//дDR,����1����������
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////
__align(8) u8 USART1_TX_BUF[USART1_MAX_SEND_LEN]; 	//���ͻ���,���USART1_MAX_SEND_LEN�ֽ� 	 
									 
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������ 
void usart1_init(u32 pclk2,u32 bound)
{  	 	
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRH&=0XFFFFF00F;//IO״̬����
	GPIOA->CRH|=0X000008B0;//IO״̬���� 
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
	//����������
 	USART1->BRR=mantissa; // ����������	 
	USART1->CR1|=0X200C;  //1λֹͣ,��У��λ.
#if EN_USART1_RX		  //���ʹ���˽���
	//ʹ�ܽ����ж� 
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	
	MY_NVIC_Init(2,3,USART1_IRQn,2);//��2��������ȼ� 
	TIM3_Init(999,7199);		//100ms�ж�
	USART1_RX_STA=0;		//����
	TIM3_Set(0);			//�رն�ʱ��3	
#endif
}

void Usart1_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

uint8_t Usart1_RecvByte(USART_TypeDef * pUSARTx)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (uint8_t)USART_ReceiveData(USART1);
}

void Usart1_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart1_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

void u1_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART1_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART1_TX_BUF);		//�˴η������ݵĳ��� 
	
	for(j=0;j<i;j++)							//ѭ����������
	{
		while((USART1->SR&0X40)==0);			//ѭ������,ֱ���������   
		USART1->DR=USART1_TX_BUF[j];  
	
	}
}

void Usart1_RxIT_Enable(void)
{
	/* ʹ�ܴ���1�����ж� */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void Usart1_RxIT_Disable(void)
{
	/* ʹ�ܴ���1�����ж� */
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
}


void TIM3_IRQHandler(void)
{ 	
	if(TIM3->SR&0X01)//�Ǹ����ж�
	{	 			   
		USART1_RX_STA|=1<<15;	//��ǽ������
		TIM3->SR&=~(1<<0);		//����жϱ�־λ		   
		TIM3_Set(0);			//�ر�TIM3
 
	}	    
}

//����TIM3�Ŀ���
//sta:0���ر�;1,����;
void TIM3_Set(u8 sta)
{
	if(sta)
	{
    TIM3->CNT=0;         //���������
		TIM3->CR1|=1<<0;     //ʹ�ܶ�ʱ��3
	}else TIM3->CR1&=~(1<<0);//�رն�ʱ��3	   
}

////ͨ�ö�ʱ���жϳ�ʼ��
////����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
////arr���Զ���װֵ��
////psc��ʱ��Ԥ��Ƶ��		 
void TIM3_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��    
 	TIM3->ARR=arr;  	//�趨�������Զ���װֵ   
	TIM3->PSC=psc;  	//Ԥ��Ƶ��
 	TIM3->DIER|=1<<0;   //��������ж�				
 	TIM3->CR1|=0x01;  	//ʹ�ܶ�ʱ��3	  	   
  MY_NVIC_Init(1,3,TIM3_IRQn,2);//��ռ2�������ȼ�3����2	��2�����ȼ����								 
}



