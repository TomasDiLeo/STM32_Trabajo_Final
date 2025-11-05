#include "AIR CONDITIONING.h"

void handle_air_conditioning(char season, int8_t target_temp, float current_temp){
	if(season == 'V' || season == 'P'){
		HAL_GPIO_WritePin(HEATING_LED_GPIO_Port, HEATING_LED_Pin, GPIO_PIN_RESET);

		if(current_temp  - target_temp >= TEMP_COOLING_THRESHOLD){
			HAL_GPIO_WritePin(COOLING_LED_GPIO_Port, COOLING_LED_Pin, GPIO_PIN_SET);
		} else if(current_temp - target_temp <= 0.0f){
			HAL_GPIO_WritePin(COOLING_LED_GPIO_Port, COOLING_LED_Pin, GPIO_PIN_RESET);
		}
	} else {
		HAL_GPIO_WritePin(COOLING_LED_GPIO_Port, COOLING_LED_Pin, GPIO_PIN_RESET);

		if(target_temp  - current_temp >= TEMP_HEATING_THRESHOLD){
			HAL_GPIO_WritePin(HEATING_LED_GPIO_Port, HEATING_LED_Pin, GPIO_PIN_SET);
		} else if(target_temp - current_temp <= 0.0f){
			HAL_GPIO_WritePin(HEATING_LED_GPIO_Port, HEATING_LED_Pin, GPIO_PIN_RESET);
		}
	}
}

void reset_air_conditioning(void){
	HAL_GPIO_WritePin(HEATING_LED_GPIO_Port, HEATING_LED_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(COOLING_LED_GPIO_Port, COOLING_LED_Pin, GPIO_PIN_RESET);
}
