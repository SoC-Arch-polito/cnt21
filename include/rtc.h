#include "stm32f4xx_hal.h"

int unix_timestamp(RTC_TimeTypeDef *gTime, RTC_DateTypeDef *gDate);
void readStart(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *gTime, RTC_DateTypeDef *gDate, uint8_t start[]);
int incrementNumber(RTC_TimeTypeDef *gTime, RTC_DateTypeDef *gDate, uint16_t *number);
int decrementNumber(RTC_TimeTypeDef *gTime, RTC_DateTypeDef *gDate, uint16_t *number);