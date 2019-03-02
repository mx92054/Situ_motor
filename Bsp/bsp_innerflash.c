#include "bsp_innerflash.h"
#include "stm32f10x.h"

//------------------------------------------------------------
//	@brief:  	write data to internal FLASH
//	@param: 	pData - data need write
//						len - data length need write
// 	@retval: 	0 - success 
//-------------------------------------------------------------
int	InternalFlashWrite(short* pData, short len) 
{
	uint32_t StartAddr = 0 ;
	uint32_t EndAddr = 0 ;
	FLASH_Status status = FLASH_COMPLETE ;
	
	FLASH_Unlock() ;
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR) ;
	status = FLASH_ErasePage(WRITE_START_ADDR) ;
	
	StartAddr = WRITE_START_ADDR ;
	EndAddr = StartAddr + len*4 ;
	while( StartAddr < EndAddr && status == FLASH_COMPLETE )
	{
		status = FLASH_ProgramWord(StartAddr,(uint32_t)(*pData)) ;
		pData++ ;
		StartAddr += 4 ;
	}
	
	FLASH_Lock() ;
	
	return status ;	
}

//----------------------------------------------------------
//	@brief:  	write data to internal FLASH
//	@param: 	pData - data need write
//						len - data length need write
// 	@retval: 	None
//-------------------------------------------------------------
void	InternalFlashRead(short* pData, short len) 
{
	uint32_t StartAddr = 0 ;
	uint32_t EndAddr = 0 ;
	
	StartAddr = WRITE_START_ADDR ;
	EndAddr = StartAddr + len*4 ;
	while( StartAddr < EndAddr )
	{
		*pData = (short)(*(__IO uint32_t*)StartAddr) ;
		pData++ ;
		StartAddr += 4 ;
	}	
}
