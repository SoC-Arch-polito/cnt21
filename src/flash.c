// #include "flash.h"
// #include "stm32f4xx.h"

// //Private variables
// //1. sector start address
// static uint32_t MY_SectorAddrs;
// static uint8_t MY_SectorNum;

// //functions definitions
// //1. Erase Sector

// void flashEraseSector(uint8_t sector_num)
// {
// 	HAL_FLASH_Unlock();
// 	//Erase the required Flash sector
// 	FLASH_Erase_Sector(sector_num, FLASH_VOLTAGE_RANGE_3);
// 	HAL_FLASH_Lock();
// }

// //2. Set Sector Adress
// void flashSetSectorAddrs(uint8_t sector, uint32_t addrs)
// {
// 	MY_SectorNum = sector;
// 	MY_SectorAddrs = addrs;
// }

// //3. Write Flash
// void flashWrite(uint32_t idx, void *wrBuf, uint32_t Nsize, DataTypeDef dataType)
// {
// 	uint32_t flashAddress = MY_SectorAddrs + idx;
	
// 	//Erase sector before write
// 	// flashEraseSector(MY_SectorAddr);
	
// 	//Unlock Flash
// 	HAL_FLASH_Unlock();
// 	//Write to Flash
// 	switch(dataType)
// 	{
// 		case DATA_TYPE_8:
// 				for(uint32_t i=0; i<Nsize; i++)
// 				{
// 					HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, flashAddress , ((uint8_t *)wrBuf)[i]);
// 					flashAddress++;
// 				}
// 			break;
		
// 		case DATA_TYPE_16:
// 				for(uint32_t i=0; i<Nsize; i++)
// 				{
// 					HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, flashAddress , ((uint16_t *)wrBuf)[i]);
// 					flashAddress+=2;
// 				}
// 			break;
		
// 		case DATA_TYPE_32:
// 				for(uint32_t i=0; i<Nsize; i++)
// 				{
// 					HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddress , ((uint32_t *)wrBuf)[i]);
// 					flashAddress+=4;
// 				}
// 			break;
// 	}
// 	//Lock the Flash space
// 	HAL_FLASH_Lock();
// }
// //4. Read Flash
// void flashRead(uint32_t idx, void *rdBuf, uint32_t Nsize, DataTypeDef dataType)
// {
// 	uint32_t flashAddress = MY_SectorAddrs + idx;
	
// 	switch(dataType)
// 	{
// 		case DATA_TYPE_8:
// 				for(uint32_t i=0; i<Nsize; i++)
// 				{
// 					*((uint8_t *)rdBuf + i) = *(uint8_t *)flashAddress;
// 					flashAddress++;
// 				}
// 			break;
		
// 		case DATA_TYPE_16:
// 				for(uint32_t i=0; i<Nsize; i++)
// 				{
// 					*((uint16_t *)rdBuf + i) = *(uint16_t *)flashAddress;
// 					flashAddress+=2;
// 				}
// 			break;
		
// 		case DATA_TYPE_32:
// 				for(uint32_t i=0; i<Nsize; i++)
// 				{
// 					*((uint32_t *)rdBuf + i) = *(uint32_t *)flashAddress;
// 					flashAddress+=4;
// 				}
// 			break;
// 	}
// }
