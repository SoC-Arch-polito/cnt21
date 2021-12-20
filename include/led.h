#include "stm32f4xx_hal.h"

#define GPIO_BANK_LED GPIOD
#define RED_LED_PIN GPIO_PIN_6
#define GREEN_LED_PIN GPIO_PIN_7

void turn_on_red_led();
void turn_off_red_led();
void turn_on_green_led();
void turn_off_green_led();