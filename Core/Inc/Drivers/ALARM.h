#ifndef ALARM_H
#define ALARM_H

#include "stm32f1xx_hal.h"

#define HIGH_PRIORITY_TIME 125
#define LOW_PRIORITY_TIME 500

#define ALARM_LED_Pin GPIO_PIN_13
#define ALARM_LED_GPIO_Port GPIOB

typedef struct {
	uint16_t pin;
	GPIO_TypeDef *port;
} Sensor;

typedef struct {
	Sensor sensor;
	uint16_t toggle_time;
} Alarm;

#define READ_BUTTON(port, pin) HAL_GPIO_ReadPin(port, pin)
#define TOGGLE_PIN(name) HAL_GPIO_TogglePin(name##_GPIO_Port, name##_Pin)
#define WRITE_PIN(name, state) HAL_GPIO_WritePin(name##_GPIO_Port, name##_Pin, state)

void alarm_init(void);
void handle_alarm(void);
void reset_alarm(void);

#endif /* ALARM_H */
