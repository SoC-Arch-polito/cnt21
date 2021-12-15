#include "stdio.h"
#include "main.h"
#include "flash.h"
#include "rtc.h"
#include "string.h"
#include "time.h"
#define F4 1

#if F0
#include "stm32f0xx_hal.h"
#elif F1
#include "stm32f1xx_hal.h"
#elif F2
#include "stm32f2xx_hal.h"
#elif F3
#include "stm32f3xx_hal.h"
#elif F4
#include "stm32f4xx_hal.h"
#elif F7
#include "stm32f7xx_hal.h"
#elif L0
#include "stm32l0xx_hal.h"
#elif L1
#include "stm32l1xx_hal.h"
#elif L4
#include "stm32l4xx_hal.h"
#else
#error "Unsupported STM32 Family"
#endif

#define LED_PIN GPIO_PIN_5
#define LED_GPIO_PORT GPIOA
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

uint8_t test_write[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
uint8_t test_read[5];

static void MX_USART1_UART_Init(void);
static void MX_RTC_Init(void);

UART_HandleTypeDef huart1;
RTC_HandleTypeDef hrtc;


/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    uint8_t start[23], datestamp[23], timestamp[23];
    RTC_DateTypeDef gDate;
    RTC_TimeTypeDef gTime;
    strcpy((char *)start, "11:11:11 11/11/2020 10");
    SystemInit();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    SystemClock_Config();
    HAL_Init();
    MX_USART1_UART_Init();
    MX_RTC_Init();

    //HAL_UART_Receive(&huart1, data, 21, HAL_MAX_DELAY);
    readStart(&hrtc, &gTime, &gDate, start);
    int timestamp_func;
    timestamp_func = unix_timestamp(&gTime, &gDate);

    LED_GPIO_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);

    flashSetSectorAddrs(11, 0x080E0000);
    flashWrite(0, test_write, 5, DATA_TYPE_8);

    while (1) {
        HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);

        flashRead(0, test_read, 5, DATA_TYPE_8);
        flashWrite(10, test_write, 5, DATA_TYPE_8);
        flashEraseSector(11);
        flashRead(0, test_read, 5, DATA_TYPE_8);

        HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

        snprintf((char *)timestamp, sizeof(timestamp), "%02d:%02d:%02d\n%d", gTime.Hours, gTime.Minutes, gTime.Seconds, timestamp_func);
        snprintf((char *)datestamp, sizeof(datestamp), "%02d-%02d-%2d\n", gDate.Date, gDate.Month, 2000 + gDate.Year);
        HAL_UART_Transmit(&huart1, datestamp, strlen((char *)datestamp), HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart1, timestamp, strlen((char *)timestamp), HAL_MAX_DELAY);
        HAL_Delay(1000);
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 12;
    RCC_OscInitStruct.PLL.PLLN = 96;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        // Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
        // Error_Handler();
    }
    /** Enables the Clock Security System
     */
    HAL_RCC_EnableCSS();
}

static void MX_USART1_UART_Init(void) {

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_RTC_Init(void) {

    /* USER CODE BEGIN RTC_Init 0 */

    /* USER CODE END RTC_Init 0 */

    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    RTC_AlarmTypeDef sAlarm = {0};

    /* USER CODE BEGIN RTC_Init 1 */

    /* USER CODE END RTC_Init 1 */
    /** Initialize RTC Only
     */
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&hrtc) != HAL_OK) {
        Error_Handler();
    }

    /* USER CODE BEGIN Check_RTC_BKUP */

    /* USER CODE END Check_RTC_BKUP */

    /** Initialize RTC and set the Time and Date
     */
    sTime.Hours = 0x20;
    sTime.Minutes = 0x10;
    sTime.Seconds = 0x07;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler();
    }

    sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
    sDate.Month = RTC_MONTH_DECEMBER;
    sDate.Date = 0x11;
    sDate.Year = 0x05;

    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler();
    }
    /** Enable the Alarm A
     */
    sTime.Hours = 0x03;
    sTime.Minutes = 0x10;
    sTime.Seconds = 0x07;
    sAlarm.AlarmTime.SubSeconds = 0x0;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmDateWeekDay = 0x3;
    sAlarm.Alarm = RTC_ALARM_A;
    if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN RTC_Init 2 */

    /* USER CODE END RTC_Init 2 */
}

void SysTick_Handler(void) {
    HAL_IncTick();
}

void NMI_Handler(void) {
}

void HardFault_Handler(void) {
    while (1) {
    }
}

void MemManage_Handler(void) {
    while (1) {
    }
}

void BusFault_Handler(void) {
    while (1) {
    }
}

void UsageFault_Handler(void) {
    while (1) {
    }
}

void SVC_Handler(void) {
}

void DebugMon_Handler(void) {
}

void PendSV_Handler(void) {
}

void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}
