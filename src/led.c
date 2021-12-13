#include "led.h"
#include "stm32f4xx_hal.h"

/**
 * @brief Used to turn on the red led placed at GPIO_BANK->RED_LED_PIN
 * 
 */
void turn_on_red_led(){
    HAL_GPIO_WritePin(GPIO_BANK, RED_LED_PIN, GPIO_PIN_SET);

}

/**
 * @brief Used to turn off the red led placed at GPIO_BANK->RED_LED_PIN
 * 
 */
void turn_off_red_led(){
    HAL_GPIO_WritePin(GPIO_BANK, RED_LED_PIN, GPIO_PIN_RESET);
}

/**
 * @brief Used to turn on the green led placed at GPIO_BANK->GREEN_LED_PIN
 * 
 */
void turn_on_green_led(){
    HAL_GPIO_WritePin(GPIO_BANK, GREEN_LED_PIN, GPIO_PIN_SET);
}

/**
 * @brief Used to turn off the green led placed at GPIO_BANK->GREEN_LED_PIN
 * 
 */
void turn_off_green_led(){
    HAL_GPIO_WritePin(GPIO_BANK, GREEN_LED_PIN, GPIO_PIN_RESET);
}