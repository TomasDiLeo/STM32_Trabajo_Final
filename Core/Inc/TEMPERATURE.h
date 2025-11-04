#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "stm32f1xx_hal.h"

#define TEMP_SENSOR_pin GPIO_PIN_0
#define TEMP_SENSOR_GPIO_Port B

void temperature_sensor_init(ADC_HandleTypeDef *hadc1);
float poll_sensor(void);

#endif // TEMPERATURE_H
