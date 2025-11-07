#ifndef KEYPAD_H
#define KEYPAD_H

#include "stm32f1xx_hal.h"

#define ROW_1_Pin GPIO_PIN_1
#define ROW_1_GPIO_Port GPIOA
#define ROW_3_Pin GPIO_PIN_2
#define ROW_3_GPIO_Port GPIOA
#define COL_2_Pin GPIO_PIN_6
#define COL_2_GPIO_Port GPIOA
#define ROW_2_Pin GPIO_PIN_7
#define ROW_2_GPIO_Port GPIOA
#define ROW_4_Pin GPIO_PIN_8
#define ROW_4_GPIO_Port GPIOA
#define COL_1_Pin GPIO_PIN_4
#define COL_1_GPIO_Port GPIOB
#define COL_3_Pin GPIO_PIN_8
#define COL_3_GPIO_Port GPIOB
#define COL_4_Pin GPIO_PIN_9
#define COL_4_GPIO_Port GPIOB

#define READ_PIN(name) HAL_GPIO_ReadPin(name##_GPIO_Port, name##_Pin)
#define WRITE_PIN(name, state) HAL_GPIO_WritePin(name##_GPIO_Port, name##_Pin, state)

static const uint8_t keymap[4][4] = {
    { 1,  2,  3, 11},
    { 4,  5,  6, 12},
    { 7,  8,  9, 13},
    {15, 10, 16, 14}
};

uint8_t keypad_read(void);

#endif /* KEYPAD_H */
