#include "log.h"
#include "stdio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "time.h"

static uint16_t number;
static uint16_t i = 0;

static uint32_t MY_SectorAddrs;
static uint8_t MY_SectorNum;

int log_unix_timestamp(RTC_TimeTypeDef *gTime, RTC_DateTypeDef *gDate) {
    struct tm t;
    int t_of_day;

    t.tm_year = (int)gDate->Year + 2000 - 1900; // Year - 1900
    t.tm_mon = (int)gDate->Month - 1;   // Month, where 0 = jan
    t.tm_mday = (int)gDate->Date;       // Day of the month
    t.tm_hour = (int)gTime->Hours + (gTime->TimeFormat == RTC_HOURFORMAT12_PM ? 12 : 0);
    t.tm_min = (int)gTime->Minutes;
    t.tm_sec = (int)gTime->Seconds;
    t.tm_isdst = -1; // Is DST on? 1 = yes, 0 = no, -1 = unknown
    t_of_day = (int)mktime(&t);
    return t_of_day;
}

void log_rtc_setup(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *gTime, RTC_DateTypeDef *gDate, const char *start) {
    int hour, min, sec, day, month, year;
    flashSetSectorAddrs(11, 0x080E0000);
    // https://community.st.com/s/question/0D53W000004KJtn/cannot-set-time-with-halrtcsettime-on-l412kb-nucleo32-after-init
    // HAL_RTC_GetDate(hrtc, gDate, RTC_FORMAT_BIN);
    // HAL_RTC_GetTime(hrtc, gTime, RTC_FORMAT_BIN);
    sscanf((char *)start, "%d/%d/%d %d/%d/%d", &day, &month, &year, &hour, &min, &sec);
    gTime->Hours = hour > 12 ? hour % 12 : hour;
    gTime->Minutes = min;
    gTime->Seconds = sec;
    gTime->TimeFormat = hour >= 12 ? RTC_HOURFORMAT12_PM : RTC_HOURFORMAT12_AM;
    HAL_RTC_SetTime(hrtc, gTime, RTC_FORMAT_BIN);
    gDate->Date = day;
    gDate->Month = month;
    gDate->Year = year - 2000;
    HAL_RTC_SetDate(hrtc, gDate, RTC_FORMAT_BIN);
}

void log_set_start_number(uint16_t this_number){
    number = this_number;
}

int log_update_number(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *gTime, RTC_DateTypeDef *gDate, uint16_t this_number, struct COMM_Handle *hcomm) {
    int timestamp;

    number = this_number;
	HAL_RTC_GetTime(hrtc, gTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(hrtc, gDate, RTC_FORMAT_BIN);
	timestamp = log_unix_timestamp(gTime, gDate);
    flashWrite(i, &timestamp, 1, DATA_TYPE_32);
    flashWrite(i + 4, &this_number, 1, DATA_TYPE_16);
    hcomm->SrcMemory.size = hcomm->SrcMemory.size + 6;
    i = i + 6;

    return timestamp;
}

//functions definitions
//1. Erase Sector

void flashEraseSector(uint8_t sector_num) {
    HAL_FLASH_Unlock();
    //Erase the required Flash sector
    FLASH_Erase_Sector(sector_num, FLASH_VOLTAGE_RANGE_3);
    HAL_FLASH_Lock();
}

//2. Set Sector Adress
void flashSetSectorAddrs(uint8_t sector, uint32_t addrs) {
    MY_SectorNum = sector;
    MY_SectorAddrs = addrs;
}

//3. Write Flash
void flashWrite(uint32_t idx, void *wrBuf, uint32_t Nsize, DataTypeDef dataType) {
    uint32_t flashAddress = MY_SectorAddrs + idx;

    //Erase sector before write
    // flashEraseSector(MY_SectorAddr);

    //Unlock Flash
    HAL_FLASH_Unlock();
    //Write to Flash
    switch (dataType) {
    case DATA_TYPE_8:
        for (uint32_t i = 0; i < Nsize; i++) {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, flashAddress, ((uint8_t *)wrBuf)[i]);
            flashAddress++;
        }
        break;

    case DATA_TYPE_16:
        for (uint32_t i = 0; i < Nsize; i++) {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, flashAddress, ((uint16_t *)wrBuf)[i]);
            flashAddress += 2;
        }
        break;

    case DATA_TYPE_32:
        for (uint32_t i = 0; i < Nsize; i++) {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddress, ((uint32_t *)wrBuf)[i]);
            flashAddress += 4;
        }
        break;
    }
    //Lock the Flash space
    HAL_FLASH_Lock();
}
//4. Read Flash
void flashRead(uint32_t idx, void *rdBuf, uint32_t Nsize, DataTypeDef dataType) {
    uint32_t flashAddress = MY_SectorAddrs + idx;

    switch (dataType) {
    case DATA_TYPE_8:
        for (uint32_t i = 0; i < Nsize; i++) {
            *((uint8_t *)rdBuf + i) = *(uint8_t *)flashAddress;
            flashAddress++;
        }
        break;

    case DATA_TYPE_16:
        for (uint32_t i = 0; i < Nsize; i++) {
            *((uint16_t *)rdBuf + i) = *(uint16_t *)flashAddress;
            flashAddress += 2;
        }
        break;

    case DATA_TYPE_32:
        for (uint32_t i = 0; i < Nsize; i++) {
            *((uint32_t *)rdBuf + i) = *(uint32_t *)flashAddress;
            flashAddress += 4;
        }
        break;
    }
}
