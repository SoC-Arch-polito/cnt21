#include "i2c_lcd.h"
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "stdio.h"

extern I2C_HandleTypeDef hi2c1;  // change your handler here accordingly

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

// https://maxpromer.github.io/LCD-Character-Creator/
char customCharPerson[]= {0x0E, 0x0E, 0x04, 0x0E, 0x15, 0x04, 0x0A, 0x0A};

void lcd_create_char(char customChar[], int location){
	location &= 0x7; // we only have 8 locations 0-7
	lcd_send_cmd(LCD_CREATE_CHAR | (location << 3));
	for (int i=0; i<8; i++) {
		lcd_send_data(customChar[i]);
	}
}

void lcd_send_custom_char(int pos) {
	lcd_send_data((char)pos);
}

void lcd_send_internal(char cmd, int mode){
  	char data_u, data_l;
	uint8_t data_t[4];
	data_u = cmd&0xf0;
	data_l = (cmd<<4)&0xf0;
	data_t[0] = data_u|mode|LCD_BACKLIGHT|EN;  	//en=1, rs=0
	data_t[1] = data_u|mode|LCD_BACKLIGHT;  	//en=0, rs=0 
	data_t[2] = data_l|mode|LCD_BACKLIGHT|EN;  	//en=1, rs=0
	data_t[3] = data_l|mode|LCD_BACKLIGHT;  	//en=0, rs=0 
	HAL_I2C_Master_Transmit (&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

void lcd_send_cmd (char cmd)
{
	lcd_send_internal(cmd, 0x00);
}

void lcd_send_data (char data)
{
	lcd_send_internal(data, RS);
}

void lcd_clear (void)
{
	lcd_send_cmd (LCD_CLEAR_DISPLAY);
}

void lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= LCD_SET_ROW1_COL1;
            break;
        case 1:
            col |= LCD_SET_ROW2_COL1;
            break;
    }

    lcd_send_cmd (col);
}

void lcd_go_home(void){
	lcd_put_cur(0,0);
}

void lcd_init (void)
{
	// 4 bit initialisation
	HAL_Delay(50);  // wait for >40ms
	lcd_send_cmd (LCD_FUNCTION_SET1);
	HAL_Delay(5);  // wait for >4.1ms
	lcd_send_cmd (LCD_FUNCTION_SET1);
	HAL_Delay(1);  // wait for >100us
	lcd_send_cmd (LCD_FUNCTION_SET1);
	HAL_Delay(10);
	lcd_send_cmd (LCD_FUNCTION_SET2);       // 4bit mode
	HAL_Delay(10);

    // dislay initialisation
	lcd_send_cmd (LCD_4BIT_2LINE_MODE);     // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_cmd (LCD_DISPLAY_OFF);         //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	lcd_send_cmd (LCD_CLEAR_DISPLAY);       // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	lcd_send_cmd (LCD_INCREMENT_CURSER);    //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcd_send_cmd (LCD_DISP_ON_CURS_OFF);    //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)

	// Send person custom char
 	lcd_create_char(customCharPerson, 1);
}

void lcd_send_string (char *str)
{
	while (*str){
		lcd_send_data (*str++);
	} 
}

void lcd_send_two_string (char *str1, char * str2)
{
    lcd_clear();
	lcd_go_home();
	lcd_send_string(str1);
	lcd_put_cur(1,0);
	lcd_send_string(str2);
}

void lcd_set_text_downloading(){
	lcd_go_home();
	lcd_send_string("Downloading...  ");
	lcd_put_cur(1,0);
	lcd_send_string("                ");
}

void lcd_set_number_people(int n_people){
	char num_char[15];
	sprintf(num_char, "%d", n_people);

	lcd_go_home();
	lcd_send_string("Number of people");
	lcd_put_cur(1,0);
	lcd_send_custom_char(1);
	lcd_send_string(" ");
	lcd_send_string(num_char);
	lcd_send_string("                ");
}