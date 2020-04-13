
#include "ESP8266Modules.h"
#include "GPIOConf.h"
#include "usart.h"
//#include "stdlib.h"
//#include "stdio.h"
#include "malloc.h"
#include "string.h"
#include "bsp_internal_flash.h" 

#define FLASH_SAVE_ADDR            WRITE_START_ADDR

#define FLASH_HUBPASSWORD_ADDR         FLASH_SAVE_ADDR + 100
#define FLASH_SERVERURL_ADDR           FLASH_SAVE_ADDR + 200
#define FLASH_SERVERPORT_ADDR          FLASH_SAVE_ADDR + 300
#define FLASH_PINPASSWORD_ADDR         FLASH_SAVE_ADDR + 400
#define FLASH_DEVICEVERSION_ADDR       FLASH_SAVE_ADDR + 500


//u8   received_data[100];
//u16  receivedByte=0;

char  strHubName[20];
char 	strHubPass[20] ;
char 	serverURL[20];
char 	serverPort[20];
static char  PinPassword[10];
char 	strDeviceVersion[20];

char Pinword[5];
char macAddr[5];

static char myDefineDeviceVersion[20] = {"WiFiCigarGuard_V2.0"};

static char SendStringINFO[200];
static char SendStringUP[20];
static char RecvStringUpdata[200];

u8 HubnameLength;
u8 PasswordLength;
u8 URLLength;
u8 PortNumLength;
u8 PinPasswordLength;
u8 DeviceVersLength;
u8 tempIndex;

char* ptrCmp;
char* ptrHubname;
char* ptrPassword;
char* ptrURL;
char* ptrPinCode;
char* ptrPORT;

static char ATcmd[50];
static char ChipID[5];
static char SerialNumber[5];

//usmart支持部分
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART1_RX_STA;
//     1,清零USART1_RX_STA;
void esp_8266_at_response(u8 mode)
{
	if(USART1_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART1_RX_BUF[USART1_RX_STA&0X7FFF]=0;//添加结束符
		//printf("%s",USART1_RX_BUF);	//发送到串口
		if(mode)USART1_RX_STA=0;
	} 
}

u8 compareChars(char *a, char *b, u8 length)
{
  int i;
  for(i = 0; i < length; i++)
  {
    if(a[i] != b[i]) 
      return 1;
  }
  return 0; // equal
}

//ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* esp8266_check_cmd(u8 *str)
{	
	char *strx=0;
	if(USART1_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART1_RX_BUF[USART1_RX_STA&0X7FFF] = 0;//添加结束符

		strx=strstr((const char*)USART1_RX_BUF,(const char*)str);	
	} 
	return (u8*)strx;
}

u8 esp8266_check_data(u8 *checkdata,u16 waittime)
{
	u8 res=0; 
	USART1_RX_STA=0;
	
	if(checkdata&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			
			if(USART1_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(esp8266_check_cmd(checkdata))
				{
					res = 1;
					break;//得到有效数据 
				}
				USART1_RX_STA=0;
			} 
		}
		
		if(waittime==0)res=1; 
	}
	
	return res;
}

//向ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 esp8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART1_RX_STA=0;
	u1_printf("%s\r\n",cmd);	//发送命令
	
	if(ack && waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			
			if(USART1_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(esp8266_check_cmd(ack))
				{
//					printf("ack:%s\r\n",(u8*)ack);
					res = 1;
					break;//得到有效数据 
				}
				
				if((esp8266_check_cmd("FAIL")) || (esp8266_check_cmd("ERROR")))
				{
					res = 5;
					break;
				}
				
				USART1_RX_STA=0;
			}
		}
		
		if(waittime==0)
		{
			return 5;
		}
	}
	return res;
}

//向ATK-ESP8266发送指定数据
//data:发送的数据(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)luojian
u8 esp8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART1_RX_STA=0;
	u1_printf("%s",data);	//发送命令
	
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART1_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(esp8266_check_cmd(ack))break;//得到有效数据 
				USART1_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}

//ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
u8 esp8266_quit_trans(void)
{
	while((USART1->SR&0X40)==0);	//等待发送空
	USART1->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART1->SR&0X40)==0);	//等待发送空
	USART1->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART1->SR&0X40)==0);	//等待发送空
	USART1->DR='+';      
	delay_ms(500);					//等待500ms
	return esp8266_send_cmd("AT","OK",20);//退出透传判断.
}


static void ESP8266_Rst(void)
{
   esp8266_send_cmd ( "AT+RST", "OK", 2500 );   	
}

static u8 ESP8266_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode)
{
  switch ( enumMode )
  {
    case STA:
      return esp8266_send_cmd ("AT+CWMODE=1", "OK", 200); 

    case AP:
      return esp8266_send_cmd ("AT+CWMODE=2", "OK", 200); 

    case STA_AP:
      return esp8266_send_cmd ("AT+CWMODE=3", "OK", 200); 

    default:
      return false;
  }	
}

static void CharArray_NullInit(char* strPtr, u16 ArrayLength)
{
	u16 i;
	for(i = 0; i < ArrayLength; i++)
	{
		strPtr[i] = '\0';
	}
}

u8 ESP8266_SendString(char* SeStr, u8 MUXnum)
{
	unsigned int SendStrLen = 0;
	char sstr[200] = {0};
	char cCmd[20];
	u8 bRet = 0;
	
	sprintf(sstr,"%s\r\n", SeStr);

	SendStrLen = strlen(sstr);
	
	if(MUXnum == MUX0)
	{
		sprintf(cCmd, "AT+CIPSEND=%d", SendStrLen);
	}
	if(MUXnum == MUX1)
	{
		sprintf(cCmd, "AT+CIPSEND=0,%d", SendStrLen);
	}
   
	esp8266_send_cmd((u8*)cCmd, ">", 200);
	
	bRet = esp8266_send_cmd ((u8*)sstr, "SEND OK", 1000 );
	
	return bRet;
}

void ESP8266_SendByteArr(u8* ByteArrays, int SendStrLen, u8 MUXn)
{
	int i;
	char ByteCmd[20];
	
	if(MUXn == MUX0)
		{
			sprintf(ByteCmd, "AT+CIPSEND=%d", SendStrLen);
		}
		
	if(MUXn == MUX1)
		{
			sprintf(ByteCmd, "AT+CIPSEND=0,%d", SendStrLen);
		}
	
	if(esp8266_send_cmd((u8*)ByteCmd, ">", 50))
		{
			for (i = 0; i < SendStrLen; i++)
				{
					Usart1_SendByte(USART1, ByteArrays[i]);
//					LED1_ON;
				}
			delay_ms(10);	
		}	
}

char* ESP8266_ReceiveString (void)
{
	char* pRecStr = 0;
	
	USART1_RX_STA = 0;
		
	while (!(USART1_RX_STA & 0x8000))
	
//////	USART1_RX_BUF[USART1_RX_STA] = '\0';

	if (strstr((const char*)USART1_RX_BUF, ">"))
		pRecStr = (char*)USART1_RX_BUF;

	if (strstr ((const char*)USART1_RX_BUF, "+IPD"))
		pRecStr = (char*)USART1_RX_BUF;
	else
		pRecStr = 0;

	return pRecStr;	
}

u8 DeviceStartMode_Decide(void)
{
	u8 DataLength;
	DataLength = STMFLASH_Read(FLASH_HUBPASSWORD_ADDR,(u32*)strHubPass);
	if(DataLength < 3)
		return 1;
	else
		return 0;
}

void DeviceInfo_ReadtoFLASH(void)
{
	HubnameLength = STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)strHubName);
	delay_ms(1);
	PasswordLength = STMFLASH_Read(FLASH_HUBPASSWORD_ADDR,(u32*)strHubPass);
	delay_ms(1);
	URLLength = STMFLASH_Read(FLASH_SERVERURL_ADDR,(u32*)serverURL);
	delay_ms(1);
	PortNumLength = STMFLASH_Read(FLASH_SERVERPORT_ADDR,(u32*)serverPort);
	delay_ms(1);
	PinPasswordLength = STMFLASH_Read(FLASH_PINPASSWORD_ADDR,(u32*)PinPassword);
	delay_ms(1);
	DeviceVersLength = STMFLASH_Read(FLASH_DEVICEVERSION_ADDR,(u32*)strDeviceVersion);
	delay_ms(100);	
}

void InitDevice_WithApp(void)
{
	int k;
	u8* Rp = 0;	
	char* ReSt = 0; 
	int RecvStrLen = 0;	
	char  tempString[20];
	
	HubnameLength = 0;
	PasswordLength = 0;
	URLLength = 0;
	PortNumLength = 0;
	PinPasswordLength = 0;
	tempIndex = 0;

	SerialNumber[0] = ChipID[0];
	SerialNumber[1] = ChipID[1];
	SerialNumber[2] = ChipID[2];
	SerialNumber[3] = ChipID[3];
	SerialNumber[4] = ChipID[4];
						
	DeviceVersLength = strlen(myDefineDeviceVersion);
	STMFLASH_Write(FLASH_DEVICEVERSION_ADDR,(u32*)myDefineDeviceVersion,DeviceVersLength);	
	delay_ms(100);
	DeviceInfo_ReadtoFLASH();
	
	CharArray_NullInit(tempString, 20);
	CharArray_NullInit(SendStringINFO, 200);
	
//	sprintf(SendStringINFO, "@DEVICEINFO*@HUBNAME*\"%s\"@PASSWORD*\"%s\"@URL*\"%s\"@PORT*\"%s\"@PINCODE*\"%s\"@VERSION*\"%s\"@END*", 
//													strHubName, strHubPass, serverURL, serverPort, PinPassword, strDeviceVersion);

	sprintf(SendStringINFO, "Pin#: %s\n Serial#: %s\n", 
													PinPassword, SerialNumber);
	
	sprintf(SendStringUP, "@Update*");
	
  USART1_RX_STA=0;
	
	do
	{
    if(USART1_RX_STA&0X8000)
		{
			Rp = esp8266_check_cmd("CONNECT");
		}
	}	while(!Rp);
	
	LED1_ON;
	Rp = 0;
	
//	USART1_RX_STA=0;
//	ReSt = ESP8266_ReceiveString();
	
	if(esp8266_check_cmd("@VER?*"))
		{
			ESP8266_SendString(SendStringINFO, MUX1);

			ReSt = 0;
			LED1_ON;
			
			Repeat:
			
			do
			{
				ReSt = ESP8266_ReceiveString();
			}while(!ReSt);
			
			if(esp8266_check_cmd("@VER?*"))
					goto Repeat;
			
			if(esp8266_check_cmd("@UPDATE*@HUBNAME*"))
			{
				strncpy(RecvStringUpdata,ReSt,200);
				delay_ms(10);	
				RecvStrLen = strlen(RecvStringUpdata);
//////				delay_ms(10);	
//////				ESP8266_SendString(RecvStringUpdata, MUX1);
				delay_ms(50);	
				ESP8266_SendString(SendStringUP, MUX1); // Response "@Update"
				LED1_ON;
			}
		}
	else if(esp8266_check_cmd("@UPDATE*@HUBNAME*"))
		{
				strncpy(RecvStringUpdata,ReSt,200);
				delay_ms(10);	
				RecvStrLen = strlen(RecvStringUpdata);
//////				delay_ms(10);	
//////				ESP8266_SendString(RecvStringUpdata, MUX1);
				delay_ms(50);	
				ESP8266_SendString(SendStringUP, MUX1); // Response "@Update"
				LED1_ON;
		}
		
	for(k = 0; k < RecvStrLen; k++)
	{
		if((RecvStringUpdata[k]=='A')&&(RecvStringUpdata[k+1]=='M')&&
			 (RecvStringUpdata[k+2]=='E'))
			{
				tempIndex = k + 4; // K is Index of "NAME"
				ptrHubname = RecvStringUpdata + tempIndex;
			}
		else if((RecvStringUpdata[k]=='O')&&(RecvStringUpdata[k+1]=='R')&&
			 (RecvStringUpdata[k+2]=='D'))
			{
				HubnameLength = k - (tempIndex + 6); // K is Index of "WORD"
				tempIndex = k + 4;
				ptrPassword = RecvStringUpdata + tempIndex;
				strncpy(tempString,ptrHubname,HubnameLength);
				STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)tempString,HubnameLength);
			}
		else if((RecvStringUpdata[k]=='U')&&(RecvStringUpdata[k+1]=='R')&&
		   (RecvStringUpdata[k+2]=='L'))
			{
				PasswordLength = k - (tempIndex + 1); // K is Index of "@URL"
				tempIndex = k + 4;
				ptrURL = RecvStringUpdata + tempIndex;
				CharArray_NullInit(tempString, 20);
				strncpy(tempString,ptrPassword,PasswordLength);
				STMFLASH_Write(FLASH_HUBPASSWORD_ADDR,(u32*)tempString,PasswordLength);
			}
		else if((RecvStringUpdata[k]=='O')&&(RecvStringUpdata[k+1]=='R')&&
		   (RecvStringUpdata[k+2]=='T'))
			{
				URLLength = k - (tempIndex + 2); // K is Index of "PORT"
				CharArray_NullInit(tempString, 20);
				strncpy(tempString,ptrURL,URLLength);
				STMFLASH_Write(FLASH_SERVERURL_ADDR,(u32*)tempString,URLLength);
				
				tempIndex = k + 4;
				PortNumLength = 4;
				ptrPORT = RecvStringUpdata + tempIndex;
				CharArray_NullInit(tempString, 20);
				strncpy(tempString,ptrPORT,PortNumLength);
				STMFLASH_Write(FLASH_SERVERPORT_ADDR,(u32*)tempString,PortNumLength);
			}
		else if((RecvStringUpdata[k]=='I')&&(RecvStringUpdata[k+1]=='N')&&
		   (RecvStringUpdata[k+2]=='C'))
			{
				tempIndex = k + 7; // K is Index of "PinCode"
				ptrPinCode = RecvStringUpdata + tempIndex;
			}
		else if((RecvStringUpdata[k]=='E')&&(RecvStringUpdata[k+1]=='N')&&
		   (RecvStringUpdata[k+2]=='D'))
			{
				PinPasswordLength = k - (tempIndex + 1);
				CharArray_NullInit(tempString, 20);
				strncpy(tempString,ptrPinCode,PinPasswordLength);
				STMFLASH_Write(FLASH_PINPASSWORD_ADDR,(u32*)tempString,PinPasswordLength);
			}
		else{};
	}

	delay_ms(50);	

	LED1_OFF;
}

void ESP8266_GetChipID(void)
{
	int ii;
	
	esp8266_send_cmd("AT+CIPSTAMAC?","OK",200);
	
//	ChipID[0] = USART1_RX_BUF[37];
//	ChipID[1] = USART1_RX_BUF[38];
//	ChipID[2] = USART1_RX_BUF[40];
//	ChipID[3] = USART1_RX_BUF[41];
//	ChipID[4] = USART1_RX_BUF[43];
//	ChipID[5] = USART1_RX_BUF[44];
	
	ChipID[0] = USART1_RX_BUF[38];
	ChipID[1] = USART1_RX_BUF[40];
	ChipID[2] = USART1_RX_BUF[41];
	ChipID[3] = USART1_RX_BUF[43];
	ChipID[4] = USART1_RX_BUF[44];
	
	for(ii = 0; ii < 5; ii++)
		macAddr[ii] = ChipID[ii];
}

void ESP8266_STAClient_Build(char* NetHubName, char* HubPassWord, char* SeveURL, char* SevePortNum)
{
	u8 ConIs;
	
	char cmd01[50];
	char cmd02[50];
	
	char url[20] = {"192.168.0.109"};
	char portn[10] = {"8080"};	
		
	sprintf(cmd01,"AT+CWJAP=\"%s\",\"%s\"",NetHubName,HubPassWord);
//	sprintf(cmd02, "AT+CIPSTART=\"TCP\",\"%s\",%s",SeveURL,SevePortNum);
	sprintf(cmd02, "AT+CIPSTART=\"TCP\",\"%s\",%s",url,portn);
	
	LED1_ON;
		
  do
	{
//			CharArray_NullInit(ATcmd, 50);	
		
			ConIs = 0;
			esp8266_send_cmd("AT","OK",200);	
			ESP8266_Net_Mode_Choose(STA);
			ESP8266_Rst();
			delay_ms(300);
			esp8266_send_cmd("AT+CWLAP","OK",1000);
			
//			sprintf(ATcmd,"AT+CWJAP=\"%s\",\"%s\"",NetHubName,HubPassWord);
			esp8266_send_cmd((u8*)cmd01,"OK",5000);
		
			ESP8266_Net_Mode_Choose(STA);
			esp8266_send_cmd("AT+CIPMUX=0","OK",1000);  // Single Mode.......
			esp8266_send_cmd("AT+CIFSR","OK",1000);    // Get IP Address.......
			
//			CharArray_NullInit(ATcmd, 50);
		
//			sprintf(ATcmd, "AT+CIPSTART=\"TCP\",\"%s\",%s",SeveURL,SevePortNum);
//			sprintf(ATcmd, "AT+CIPSTART=\"TCP\",\"%s\",%s",url,portn);
			delay_ms(3);
			ConIs = esp8266_send_cmd((u8*)cmd02,"OK",1500);
		
			delay_ms(20);
	}while(ConIs > 3);
}

void ESP8266_APServer_Build(void)
{
	esp8266_send_cmd("AT","OK",200);	
	ESP8266_Net_Mode_Choose(AP);
	ESP8266_Rst();
	delay_ms(300);
	
//	CharArray_NullInit((u8*)USART1_RX_BUF, USART1_MAX_RECV_LEN);
	ESP8266_GetChipID();
	
	CharArray_NullInit(ATcmd, 50);
	sprintf(ATcmd,"AT+CWSAP=\"CigarSN-%s\",\"123456789\",1,0",ChipID);
	esp8266_send_cmd((u8*)ATcmd,"OK",100);
	esp8266_send_cmd("AT+CIPMUX=1","OK",100);
	esp8266_send_cmd("AT+CIPSERVER=1,3333","OK",100);
}

void Set_InitMode_APSever(void)
{
	LED1_ON;
	ESP8266_APServer_Build();
	LED1_OFF;	
	InitDevice_WithApp();
}

void Set_FixMode_STAClient(void)
{
	int kk;
////	if((strHubName[0] == '\0') && (strHubName[1] == '\0') && (strHubName[3] == '\0'))
	DeviceInfo_ReadtoFLASH();
	
	for(kk = 0; kk < 4; kk++)
		Pinword[kk] = PinPassword[kk];
	
	LED1_ON;
	delay_ms(100);
	ESP8266_STAClient_Build(strHubName, strHubPass, serverURL, serverPort);
	delay_ms(100);	
	ESP8266_SendString("STAClient OK!", MUX0);
	LED1_OFF;
}

//void Connecting_router(void)
//{
//	u16 txByte=0;
//	u16 i=0;
//	txByte = receivedByte+9;
//	received_data[txByte]=0x0D;
//	received_data[txByte+1]=0x0A;
//	USART1_RX_STA=0;

//		do
//		{
//		
//			for(i=0;i<txByte+2;i++)
//			{
//				Usart1_SendByte(USART1,received_data[i]);
//			}
//		}while(esp8266_check_data("OK",1000));
//	
//}
