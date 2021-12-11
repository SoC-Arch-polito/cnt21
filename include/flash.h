#include "stm32f4xx_hal.h"

//Typedefs
//1. data size
typedef enum
{
	DATA_TYPE_8=0,
	DATA_TYPE_16,
	DATA_TYPE_32,
}DataTypeDef;

//functions prototypes
//1. Erase Sector
// static void flashEraseSector(void);

void flashEraseSector(uint8_t sector_num);

//2. Set Sector Adress
void flashSetSectorAddrs(uint8_t sector, uint32_t addrs);
//3. Write Flash
void flashWrite(uint32_t idx, void *wrBuf, uint32_t Nsize, DataTypeDef dataType);
//4. Read Flash
void flashRead(uint32_t idx, void *rdBuf, uint32_t Nsize, DataTypeDef dataType);

