#include "main.h"
#include "i2c_lcd.h"
#include "led.h"
#include "comm.h"
#include "ir.h"

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

// Static reference for I2C
I2C_HandleTypeDef hi2c1;

void SystemClock_Config(void);
static void MX_I2C1_Init(void);
static void MX_GPIO_Init(void);

int main(void) {
    struct COMM_Handle hcomm;

    SystemInit();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    SystemClock_Config();
    HAL_Init();

    // Init LED
    MX_GPIO_Init();
	// Setup I2C
	MX_I2C1_Init();

	lcd_init();


	// Init COMM
	hcomm.SrcMemory.basePtr = (uint8_t *)0x08000000;
	hcomm.SrcMemory.size = 0x10000;
	hcomm.Callback.newValueSet = NULL;
	hcomm.Callback.onNewSysDateTime = NULL;
	hcomm.Callback.onUARTDownload = NULL;

	COMM_Init(&hcomm);
	COMM_StartListen();

    int i = 0;
    while (1) {
		lcd_set_number_people(10);
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


static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
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
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  //pin 6 as pull up
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIO_BANK_LED, &GPIO_InitStruct); 

  GPIO_InitStruct.Pin = GREEN_LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIO_BANK_LED, &GPIO_InitStruct); 

  /*Configure GPIO pin : PA9 */
  /*Configure GPIO pins : PA8 PA9 */
  GPIO_InitStruct.Pin = IR_1_PIN|IR_2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIO_BANK_IR, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(IR_EXT_HANDLE, 0, 0);
  HAL_NVIC_EnableIRQ(IR_EXT_HANDLE);
}

void EXTI9_5_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(IR_1_PIN); // Reset the PIN8 Interrupt
  HAL_GPIO_EXTI_IRQHandler(IR_2_PIN); // Reset the PIN9 Interrupt
}

/**
 * @brief Handler to manage the interrupt coming from the two IR sensors
 * 
 * @param GPIO_Pin the IR sensor pin
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin)
		{
		case IR_1_PIN:
      // TODO: manage counter, increase
      turn_on_red_led();
			break;
		case IR_2_PIN:
      // TODO: manage counter, decrease
      turn_off_red_led();
      break;
		}
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1) {}
}


void MemManage_Handler(void)
{
  while (1) {}
}

void BusFault_Handler(void)
{
  while (1) {}
}

void UsageFault_Handler(void)
{
  while (1) {}
}

void SVC_Handler(void)
{
}


void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
