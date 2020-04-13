#ifndef __USART2_H
#define __USART2_H	 

#include "sys.h"  
#include <stdio.h>
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

#define USART2_MAX_RECV_LEN		50			//�����ջ����ֽ���
#define USART2_MAX_SEND_LEN		1024				//����ͻ����ֽ���
#define USART2_RX_EN 			1					//0,������;1,����.
	 	 
extern u8 RecvFinishFlag;

extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern u16 USART2_RX_STA;   						//��������״̬

void USART2_Init(u32 pclk1,u32 bound);				//����2��ʼ�� 

void Usart2_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart2_SendString(USART_TypeDef * pUSARTx, char *str);

void TIM3_Set(u8 sta);
void TIM3_Init(u16 arr,u16 psc);

void Usart2_SendByteArr( USART_TypeDef * pUSARTx, uint8_t *ByteArr, uint8_t len);

void USART2_FirstConfig(void);

void Usart2_RxIT_Enable(void);
void Usart2_RxIT_Disable(void);
#endif













