#include "stm32f10x.h"   // 相当于51单片机中的  #include <reg51.h>
#include "sys.h"
#include "my_clkconfig.h"
#include "delay.h"
#include "usart2.h"
#include "usart.h"
#include "GPIOConf.h"
#include "ESP8266Modules.h"
#include "malloc.h" 
#include "string.h"

u8 UpTouchDown[11] =      {0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x01, 0xA1, 0xFF, 0xFF, 0xFF, 0xFF};
u8 UpTouchRelease[11] =   {0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x01, 0xA4, 0xFF, 0xFF, 0xFF, 0xFF};

u8 DownTouchDown[11] =    {0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x01, 0xA2, 0xFF, 0xFF, 0xFF, 0xFF};
u8 DownTouchRelease[11] = {0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x01, 0xA5, 0xFF, 0xFF, 0xFF, 0xFF};

u8 SetTouchDown[11] =     {0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x01, 0xA3, 0xFF, 0xFF, 0xFF, 0xFF};
u8 SetTouchRelease[11] =  {0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x01, 0xA6, 0xFF, 0xFF, 0xFF, 0xFF};

char DataHead[8] = {"fdfdfdfd"};
u8 DataSendtoServer[100] = {0};
extern u8 RecvStart;

int main(void)
{
	unsigned char j,k;

	Systick_Init();
	
//	HSI_SetSysClock(RCC_PLLMul_9);	//set up internal clock as 36MHz
//	delay_init(72);
	GPIO_Config();								//define GPIO pin and iniiate, (led control, and humidifir transmition and receive enable/disable)	
	
	LED1_OFF;
	usart1_init(36,115200);		  	// communication between cpu and esp03 module
	USART2_Init(36,9600);			// communication between cpu and humidifier
////////	NVIC_Configuration();

////////	if(DeviceStartMode_Decide())
	
	Set_InitMode_APSever();			 // set up AP mode

	Set_FixMode_STAClient();
	
//	sprintf(ConnectDemand, "AT+CIPSTART=\"TCP\",\"%s\",%s",serverURL,serverPort);
//	esp8266_send_cmd("AT+CIPCLOSE","OK",200);
			
	USART1_RX_STA = 0;
	RecvStart = 0;
	
	DataSendtoServer[0] = (u8)DataHead[0];
	DataSendtoServer[1] = (u8)DataHead[1];
	DataSendtoServer[2] = (u8)DataHead[2];
	DataSendtoServer[3] = (u8)DataHead[3];
	DataSendtoServer[4] = (u8)DataHead[4];
	DataSendtoServer[5] = (u8)DataHead[5];
	DataSendtoServer[6] = (u8)DataHead[6];
	DataSendtoServer[7] = (u8)DataHead[7];
	
	DataSendtoServer[8] = (u8)macAddr[0];
	DataSendtoServer[9] = (u8)macAddr[1];
	DataSendtoServer[10] = (u8)macAddr[2];
	DataSendtoServer[11] = (u8)macAddr[3];
	DataSendtoServer[12] = (u8)macAddr[4];
	
	DataSendtoServer[13] = (u8)Pinword[0];
	DataSendtoServer[14] = (u8)Pinword[1];
	DataSendtoServer[15] = (u8)Pinword[2];
	DataSendtoServer[16] = (u8)Pinword[3];
	
	USART2_FirstConfig();
	
//	esp8266_send_cmd((u8*)ConnectDemand, "OK", 200);
	
	ESP8266_SendString("Server Connect OK!", MUX0);

//	esp8266_send_cmd("AT+CIPCLOSE","OK",200);
	
	LED1_OFF; 
	
	while(1)
	{
		if(RecvFinishFlag)
			{
				Usart2_RxIT_Disable();
				
				for(j = 0; j < 39; j++)
						DataSendtoServer[j + 17] = USART2_RX_BUF[j];
				
//				esp8266_send_cmd((u8*)ConnectDemand, "OK", 200);  // 1-stage

//////////////////				ESP8266_SendByteArr(USART2_RX_BUF, 45, MUX0);     // 2-stage
				ESP8266_SendByteArr(DataSendtoServer, 62, MUX0);
				
				if(esp8266_check_data("+IPD", 50))
				{
						Usart1_RxIT_Disable();
						if(esp8266_check_cmd("serverA1")) // Up-Key
						{
							Usart2_SendByteArr(USART2, UpTouchDown, 11);
							delay_ms(20);
							Usart2_SendByteArr(USART2, UpTouchRelease, 11);
						}
						else if(esp8266_check_cmd("serverA2")) // Down-Key
						{
							Usart2_SendByteArr(USART2, DownTouchDown, 11);
							delay_ms(20);
							Usart2_SendByteArr(USART2, DownTouchRelease, 11);
						}
						else if(esp8266_check_cmd("serverA3")) // Set-Short-key
						{
							Usart2_SendByteArr(USART2, SetTouchDown, 11);
							delay_ms(20);
							Usart2_SendByteArr(USART2, SetTouchRelease, 11);
						}
						else if(esp8266_check_cmd("serverA7")) // Set-Long-Key
						{
							Usart2_SendByteArr(USART2, SetTouchDown, 11);
							delay_ms(500);
							Usart2_SendByteArr(USART2, SetTouchRelease, 11);
						};
						Usart1_RxIT_Enable();
				};

				USART1_RX_STA = 0;								
				RecvFinishFlag = 0;				
				USART2_RX_STA = 0;
				RecvStart = 0;	
				
//				esp8266_send_cmd("AT+CIPCLOSE","OK",200);  // 4-stage
				
				for(k = 0; k < 50; k++)
					USART2_RX_BUF[k] = 0;
				
				Usart2_RxIT_Enable();
				delay_ms(1);
			}
//		delay_ms(2);
	}
}
