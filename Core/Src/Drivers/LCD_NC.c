#include "Drivers/LCD_NC.h"

void lcd_init(void) {
    // 4 bit initialisation
    HAL_Delay(50);            // wait for >40ms
    lcd_send_cmd(0x0F);       // BLINK ON
    HAL_Delay(5);             // wait for >4.1ms
    lcd_send_cmd(0b10000010); // 4bit mode
    HAL_Delay(5);             // wait for >4.1ms
    lcd_send_cmd(0x01);       // clear display
    HAL_Delay(5);             // wait for >4.1ms
    lcd_send_cmd(0b00000010); // Position home
    HAL_Delay(5);             // wait for >4.1ms
    lcd_send_cmd(0b00001100); // Cursor->off Blinking->off
    HAL_Delay(5);             // wait for >4.1ms
    lcd_send_cmd(0x28);       // 4 BIT 2 LINES 5X7
    HAL_Delay(5);
}

void lcd_send_cmd(Lcd_Command_t cmd) {
    char datatosend;

    // send upper nibble first
    datatosend = ((cmd >> 4) & 0x0f);
    send_to_lcd(datatosend, 0); // RS must be 0 while sending command

    // send Lower Nibble
    datatosend = (cmd & 0x0f);
    send_to_lcd(datatosend, 0);
}

void send_lcd_ASCII(char data) {
    char datatosend;

    // send higher nibble
    datatosend = ((data >> 4) & 0x0f);
    send_to_lcd(datatosend, 1); // rs =1 for sending data

    // send Lower nibble
    datatosend = (data & 0x0f);
    send_to_lcd(datatosend, 1);
}

void send_to_lcd(char data, int rs) {
    HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, rs); // rs = 1 for data, rs=0 for command

    // write the data to the respective pin
    HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, ((data >> 3) & 0x01));
    HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, ((data >> 2) & 0x01));
    HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, ((data >> 1) & 0x01));
    HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, ((data >> 0) & 0x01));

    // Toggle EN PIN to send the data
    // if the HCLK > 100 MHz, use the 20 us delay
    // if the LCD still doesn't work, increase the delay to 50, 80 or 100..
    HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 1);
    HAL_Delay(1);
    HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 0);
    HAL_Delay(1);
}

void lcd_send_data(char data) {
    char datatosend;

    // send higher nibble
    datatosend = ((data >> 4) & 0x0f);
    send_to_lcd(datatosend, 1); // rs =1 for sending data

    // send Lower nibble
    datatosend = (data & 0x0f);
    send_to_lcd(datatosend, 1);
}

void lcd_send_string(char *str) {
    while (*str) lcd_send_data(*str++);
}

void lcd_put_cur(int row, int col) {
    char Fil_Col = 128;
    if (col > 15) {
        col = 15;
    }
    switch (row) {
        case 0:
            Fil_Col = 128 + col;
            break;
        case 1:
            Fil_Col = 192 + col;
            break;
        case 2:
            Fil_Col = 148 + col;
            break;
        case 3:
            Fil_Col = 212 + col;
            break;
        default:
            break;
    }
    lcd_send_cmd(Fil_Col);
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void lcd_print2d(uint8_t val) {
    lcd_send_data((val / 10) + '0');
    lcd_send_data((val % 10) + '0');
}

void lcd_print_date(uint8_t d, uint8_t m, uint8_t y) {
    lcd_print2d(d);
    lcd_send_data('/');
    lcd_print2d(m);
    lcd_send_data('/');
    lcd_print2d(y);
}

void lcd_print_time(uint8_t h, uint8_t m) {
    lcd_print2d(h);
    lcd_send_data(':');
    lcd_print2d(m);
}

