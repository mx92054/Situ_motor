#ifndef __BSP_INNERFLASH_H__
#define	__BSP_INNERFLASH_H__

#if defined(STM32F10X_HD) || defined(STM32F10X_HD_VL) ||\
		defined(STM32F10X_CL) || defined(STM32F10X_XL)
#define FLASH_PAGE_SIZE ((uint16_t)0x800)			//2048
#else
#define FLASH_PAGE_SIZE ((uint16_t)0x400)			//1024
#endif

//Address and lenght of Data need write and read
#define  	WRITE_START_ADDR		((uint32_t)0x08008000)
#define 	WRITE_END_ADDR			((uint32_t)0x08008800)


int		InternalFlashWrite(short* pData, short len) ;
void 	InternalFlashRead(short* pData, short len) ;

#endif
//--------------end of file---------------------------------
