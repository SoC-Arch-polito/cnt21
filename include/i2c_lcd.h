#include "stm32f4xx_hal.h"

void lcd_init (I2C_HandleTypeDef * i2c_ref);   // initialize lcd

void lcd_create_char(char customChar[], int location);

void lcd_send_custom_char(int pos);

void lcd_go_home(void);

void lcd_send_cmd (char cmd);  // send command to the lcd

void lcd_send_data (char data);  // send data to the lcd

void lcd_send_string (char *str);  // send string to the lcd

void lcd_put_cur(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);

void lcd_clear (void);

void lcd_send_two_string (char *str1, char * str2);

void lcd_set_text_downloading();

void lcd_set_number_people(int n_people);