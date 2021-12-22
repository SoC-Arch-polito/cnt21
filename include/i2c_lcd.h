#include "stm32f4xx_hal.h"

/*LCD Commands*/
#define LCD_FUNCTION_SET1      0x31
#define LCD_FUNCTION_SET2      0x22
#define LCD_4BIT_2LINE_MODE    0x28
#define LCD_DISP_CURS_ON       0x0E
#define LCD_DISP_ON_CURS_OFF   0x0C  //Display on, cursor off
#define LCD_DISPLAY_OFF        0x08
#define LCD_DISPLAY_ON         0x04
#define LCD_CLEAR_DISPLAY      0x01
#define LCD_ENTRY_MODE_SET     0x04
#define LCD_INCREMENT_CURSER   0x06
#define LCD_SET_ROW1_COL1      0x80  //Force cursor to beginning ( 1st line)
#define LCD_SET_ROW2_COL1      0xC0  //Force cursor to beginning ( 2nd line)
#define LCD_MOVE_DISPLAY_LEFT  0x18 
#define LCD_MOVE_DISPLAY_RIGHT 0x1C 
#define LCD_CREATE_CHAR		   0x40 

#define SLAVE_ADDRESS_LCD 0x34<<1 // change this according to ur setup
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00
#define EN 0x04
#define RS 0x01


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

void lcd_set_number_people(int n_people, int max_people);
