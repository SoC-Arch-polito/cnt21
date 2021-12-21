#include "main.h"

#include "comm.h"
#include "i2c_lcd.h"
#include "ir.h"
#include "led.h"
#include "log.h"
#include "stdio.h"
#include "string.h"
#include "time.h"
#include <stdbool.h>
#include "stm32f4xx_hal.h"


// Static reference for I2C
static I2C_HandleTypeDef hi2c1;
static RTC_HandleTypeDef hrtc;

void SystemClock_Config(void);
static void MX_I2C1_Init(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void update_interface();
static void disable_IRQ();
static void enable_IRQ();
ONNEW_SYSDTTIME_CB(onNewSysDateTime, newTime);
ONNEW_VALUE_SET_CB(onNewValueSet, newMax);
ONUART_DOWNLOAD_CB(onUartDownload, xferDone);


static int number_people;
static int number_people_max;
static uint8_t setup_phase;
static RTC_DateTypeDef gDate;
static RTC_TimeTypeDef gTime;
static struct COMM_Handle hcomm;

int main(void) {

    SystemInit();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    SystemClock_Config();
    HAL_Init();
    // Init LED
    MX_GPIO_Init();
    // Setup I2C
    MX_I2C1_Init();
    MX_RTC_Init();

    // Initilization of the counters
    number_people = 0;
    number_people_max = 0;
    setup_phase = 0;
    lcd_init(&hi2c1);

    // Init COMM
    hcomm.SrcMemory.basePtr = (uint8_t *)0x080E0000;
    hcomm.SrcMemory.size = 0x0;
    hcomm.Callback.newValueSet = onNewValueSet;
    hcomm.Callback.onNewSysDateTime = onNewSysDateTime;
    hcomm.Callback.onUARTDownload = onUartDownload;

    COMM_Init(&hcomm);
    COMM_StartListen();

    update_interface();

    while (1) {
        HAL_Delay(100);
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

ONNEW_SYSDTTIME_CB(onNewSysDateTime, newTime) {
    log_rtc_setup(&hrtc, &gTime, &gDate, newTime);
	setup_phase++;
}

ONNEW_VALUE_SET_CB(onNewValueSet, newMax) {
	number_people_max = newMax;
	setup_phase++;
	update_interface();
}

ONUART_DOWNLOAD_CB(onUartDownload, xferDone) {

	if (!hcomm.SrcMemory.size) return;

	if (xferDone) {
        update_interface();
        enable_IRQ();
    } else {
        disable_IRQ();
        lcd_set_text_downloading();
    }
}

static void MX_I2C1_Init(void) {
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = RED_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // pin 6 as pull up
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIO_BANK_LED, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GREEN_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIO_BANK_LED, &GPIO_InitStruct);

    /*Configure GPIO pin : PA9 */
    /*Configure GPIO pins : PA8 PA9 */
    GPIO_InitStruct.Pin = IR_1_PIN | IR_2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIO_BANK_IR, &GPIO_InitStruct);

    enable_IRQ();
}

static void disable_IRQ(){
    HAL_NVIC_SetPriority(IR_EXT_HANDLE, 0, 0);
    HAL_NVIC_DisableIRQ(IR_EXT_HANDLE);
}

static void enable_IRQ(){
    HAL_NVIC_SetPriority(IR_EXT_HANDLE, 0, 0);
    HAL_NVIC_EnableIRQ(IR_EXT_HANDLE);
}

void EXTI9_5_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(IR_1_PIN);  // Reset the PIN8 Interrupt
    HAL_GPIO_EXTI_IRQHandler(IR_2_PIN);  // Reset the PIN9 Interrupt
}

static void update_interface(){
    if (number_people < number_people_max) {
        turn_off_red_led();
        turn_on_green_led();
    } else {
        turn_on_red_led();
        turn_off_green_led();
    }

    lcd_set_number_people(number_people, number_people_max);
}

/**
 * @brief Handler to manage the interrupt coming from the two IR sensors
 *
 * @param GPIO_Pin the IR sensor pin
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case IR_1_PIN:
            // Manage counter, increase
            if(setup_phase > 1 && number_people < number_people_max)
                log_update_number(&hrtc, &gTime, &gDate, ++number_people, &hcomm);
        break;
        case IR_2_PIN:
            // Manage counter, decrease
            if(setup_phase > 1 && number_people > 0)
                log_update_number(&hrtc, &gTime, &gDate, --number_people, &hcomm);
        break;
    }

    update_interface();
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
    hrtc.Init.HourFormat = RTC_HOURFORMAT_12;
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

void NMI_Handler(void) {}

void HardFault_Handler(void) {
    while (1);
}

void MemManage_Handler(void) {
    while (1);
}

void BusFault_Handler(void) {
    while (1);
}

void UsageFault_Handler(void) {
    while (1);
}

void SVC_Handler(void) {}

void DebugMon_Handler(void) {}

void PendSV_Handler(void) {}

void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state
     */
    __disable_irq();
    while (1);
    /* USER CODE END Error_Handler_Debug */
}