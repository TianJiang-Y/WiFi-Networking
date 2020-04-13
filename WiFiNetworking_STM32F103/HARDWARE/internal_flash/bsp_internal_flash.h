#ifndef __INTERNAL_FLASH_H
#define	__INTERNAL_FLASH_H

#include "stm32f10x.h"

/* STM32大容量产品每页大小2KByte，中、小容量产品每页大小1KByte */
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)	//2048
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)	//1024
#endif

//写入的起始地址与结束地址
#define WRITE_START_ADDR  ((uint32_t)0x08007800)
#define WRITE_END_ADDR    ((uint32_t)0x08008000)



typedef enum 
{
	FAILED = 0, 
  PASSED = !FAILED
} TestStatus;


int InternalFlash_Test(void);
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite);
u8 STMFLASH_Read(u32 ReadAddr,u32 *pBuffer);



#endif /* __INTERNAL_FLASH_H */

