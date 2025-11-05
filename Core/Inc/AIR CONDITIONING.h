#ifndef CONDITIONING_H
#define CONDITIONING_H

#include "stm32f1xx_hal.h"

#define HEATING_LED_Pin GPIO_PIN_11
#define HEATING_LED_GPIO_Port GPIOA
#define COOLING_LED_Pin GPIO_PIN_10
#define COOLING_LED_GPIO_Port GPIOB

#define TEMP_COOLING_THRESHOLD 2.0f
#define TEMP_HEATING_THRESHOLD 3.0f

void handle_air_conditioning(char season, int8_t target_temp, float current_temp);
void reset_air_conditioning(void);

#endif /* CONDITIONING_H */
