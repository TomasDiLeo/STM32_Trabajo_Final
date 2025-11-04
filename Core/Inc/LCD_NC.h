#ifndef LCD_NC_H
#define LCD_NC_H

#include "stm32f1xx_hal.h" // Ajusta esto según tu serie STM32 si no es F1

// Define the GPIO ports and pins for your LCD
#define RS_Pin 			GPIO_PIN_15
#define RS_GPIO_Port 		GPIOA
//#define RW_Pin 			GPIO_PIN_2
//#define RW_GPIO_Port 	GPIOA
#define EN_Pin 			GPIO_PIN_5
#define EN_GPIO_Port 		GPIOA
#define D4_Pin 			GPIO_PIN_3
#define D4_GPIO_Port 		GPIOB
#define D5_Pin 			GPIO_PIN_4
#define D5_GPIO_Port 		GPIOA
#define D6_Pin 			GPIO_PIN_5
#define D6_GPIO_Port 		GPIOB
#define D7_Pin 			GPIO_PIN_3
#define D7_GPIO_Port 		GPIOA

typedef enum {
	HOME = 0x02,
	DISP_OFF = 0x00,
	DISP_ON = 0x08,
	CUR_OFF_BLINK_OFF = 0x0C,
	CUR_ON_BLINK_OFF = 0x0E,
	CUR_OFF_BLINK_ON = 0x0D,
	CUR_ON_BLINK_ON = 0x0F,
	POS_R_DFIX = 0x06,
	POS_L_DFIX = 0x04,
	POS_R_CFIX = 0x07,
	POS_L_CFIX = 0x05,
	MOV_CUR_LEFT = 0x10,
	MOV_CUR_RIGHT = 0x14,
	MOV_DISP_LEFT = 0x18,
	MOV_DISP_RIGHT = 0x1C
} Lcd_Command_t;

void lcd_init(void);
void lcd_send_cmd(Lcd_Command_t cmd);
void lcd_send_data(char data);
void lcd_send_string(char *str);
void lcd_put_cur(int row, int col);
void lcd_clear(void);
void send_lcd_ASCII(char data);
void send_to_lcd(char data, int rs);
void lcd_print2d(uint8_t val);
void lcd_print_date(uint8_t d, uint8_t m, uint8_t y);
void lcd_print_time(uint8_t h, uint8_t m);

#endif // LCD_NC_H
