
#ifndef __ESP8266Modules_H__
#define __ESP8266Modules_H__	 

#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "usart2.h" 
#include "string.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-ESP8266 WIFI模块 公用驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/4/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

typedef enum{
  STA,
  AP,
  STA_AP  
} ENUM_Net_ModeTypeDef;

#ifndef false
  #define false 0
#endif

#ifndef true
  #define true 1
#endif
		
#define MUX0            0
#define MUX1            1
	
extern char strHubName[20];
extern char	strHubPass[20];
extern char	serverURL[20];
extern char	serverPort[20];
extern char Pinword[5];
extern char	strDeviceVersion[20];

extern char macAddr[5];
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void esp_8266_at_response(u8 mode);
u8* esp8266_check_cmd(u8 *str);
	
u8 esp8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 esp8266_send_data(u8 *data,u8 *ack,u16 waittime);
	
u8 esp8266_quit_trans(void);
void Set_FixMode_STAClient(void);
void Set_InitMode_APSever(void);
u8 esp8266_check_data(u8 *checkdata,u16 waittime);
void Connecting_router(void);
	
void Set_AP_Mode_STAClient(void);
	
u8 compareChars(char *a, char *b, u8 length);
u8 ESP8266_SendString(char* SeStr, u8 MUXnum);
char* ESP8266_ReceiveString (void);
u8 DeviceStartMode_Decide(void);
void DeviceInfo_ReadtoFLASH(void);

void ESP8266_SendByteArr(u8* ByteArrays, int SendStrLen, u8 MUXn);

#endif
