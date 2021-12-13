#include "stm32f4xx_hal.h"

#define GPIO_BANK_IR GPIOD
#define IR_1_PIN GPIO_PIN_8
#define IR_2_PIN GPIO_PIN_9
#define IR_EXT_HANDLE EXTI9_5_IRQn