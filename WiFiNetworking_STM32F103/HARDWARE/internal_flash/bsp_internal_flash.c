/**
  ******************************************************************************
  * @file    bsp_internalFlash.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   �ڲ�FLASH��д���Է���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 MINI ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_internal_flash.h"   

/**
  * @brief  InternalFlash_Test,���ڲ�FLASH���ж�д����
  * @param  None
  * @retval None
  */
	
typedef volatile u32  vu32;

int InternalFlash_Test(void)
{
	uint32_t EraseCounter = 0x00; 	//��¼Ҫ��������ҳ
	uint32_t Address = 0x00;				//��¼д��ĵ�ַ
	uint32_t Data = 0x3210ABCD;			//��¼д�������
	uint32_t NbrOfPage = 0x00;			//��¼д�����ҳ
	
	FLASH_Status FLASHStatus = FLASH_COMPLETE; //��¼ÿ�β����Ľ��	
	TestStatus MemoryProgramStatus = PASSED;//��¼�������Խ��
	

  /* ���� */
  FLASH_Unlock();

  /* ����Ҫ��������ҳ */
  NbrOfPage = (WRITE_END_ADDR - WRITE_START_ADDR) / FLASH_PAGE_SIZE;


  /* ������б�־λ */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  /* ��ҳ����*/
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
    FLASHStatus = FLASH_ErasePage(WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
  }
  
  /* ���ڲ�FLASHд������ */
  Address = WRITE_START_ADDR;

  while((Address < WRITE_END_ADDR) && (FLASHStatus == FLASH_COMPLETE))
  {
    FLASHStatus = FLASH_ProgramWord(Address, Data);
    Address = Address + 4;
  }

  FLASH_Lock();
  
  /* ���д��������Ƿ���ȷ */
  Address = WRITE_START_ADDR;

  while((Address < WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
  {
    if((*(__IO uint32_t*) Address) != Data)
    {
      MemoryProgramStatus = FAILED;
    }
    Address += 4;
  }
	return MemoryProgramStatus;
}

void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
	u32 endaddr=0;	

	FLASH_Unlock();	//���� 	

	endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
//	  /* ����Ҫ��������ҳ */
//  NbrOfPage = (endaddr - addrx) / FLASH_PAGE_SIZE;	
  /* ������б�־λ */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
	  /* ��ҳ����*/
  if(WriteAddr == WRITE_START_ADDR)
  {
			FLASH_ErasePage(WriteAddr);//FLASH_PAGE_SIZE
  }  
	
  while(WriteAddr < endaddr)
  {
    if(!FLASH_ProgramWord(WriteAddr, *pBuffer))
		{
			break;
		}
    WriteAddr+=4;
		pBuffer++;
  }
	
	FLASH_Lock();//����
} 

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToRead:��(32λ)��
u8 STMFLASH_Read(u32 ReadAddr,u32 *pBuffer)
{
	u8 NumToRead = 30;
	u8 NumLength = 0;
	
	while(NumToRead--)
	{
		*pBuffer = *(vu32*)ReadAddr;//��ȡ4���ֽ�.
		if(*pBuffer == 0xFFFFFFFF)
			return NumLength;
		ReadAddr+=4;//ƫ��4���ֽ�.	
		pBuffer++;
		NumLength++;
	}
	return NumToRead;
}
