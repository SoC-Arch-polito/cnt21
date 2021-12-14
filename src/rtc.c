#include "rtc.h"
#include "time.h"
#include "stdio.h"

int unix_timestamp(RTC_TimeTypeDef *gTime, RTC_DateTypeDef *gDate) {
    struct tm t;
    int t_of_day;

    t.tm_year = (int)gDate->Year + 100; // Year - 1900
    t.tm_mon = (int)gDate->Month - 1;   // Month, where 0 = jan
    t.tm_mday = (int)gDate->Date;       // Day of the month
    t.tm_hour = (int)gTime->Hours;
    t.tm_min = (int)gTime->Minutes;
    t.tm_sec = (int)gTime->Seconds;
    t.tm_isdst = -1; // Is DST on? 1 = yes, 0 = no, -1 = unknown
    t_of_day = (int)mktime(&t);
    return t_of_day;
}

void readStart(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *gTime, RTC_DateTypeDef *gDate, uint8_t start[]) {
    int hour, min, sec, day, month, year, number;
    sscanf((char *)start, "%d:%d:%d %d/%d/%d %d", &hour, &min, &sec, &day, &month, &year, &number);
    gTime->Hours = hour;
    gTime->Minutes = min;
    gTime->Seconds = sec;
    HAL_RTC_SetTime(hrtc, gTime, RTC_FORMAT_BIN);
    gDate->Date = day;
    gDate->Month = month;
    gDate->Year = year - 2000;
    HAL_RTC_SetDate(hrtc, gDate, RTC_FORMAT_BIN);
}