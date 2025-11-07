#include <Drivers/TEMPERATURE_SENSOR.h>

static ADC_HandleTypeDef *hadc1_pointer;
volatile uint32_t adc_value = 0;
volatile uint8_t adc_conversion_complete = 0;

static void prepare_poll(void){
	if (HAL_ADC_Start_IT(hadc1_pointer) != HAL_OK)
	{
		Error_Handler();
	}
}

void temperature_sensor_init(ADC_HandleTypeDef*hadc1){
	hadc1_pointer = hadc1;

	prepare_poll();
}


float poll_sensor(void){
	static float output;
	if(adc_conversion_complete){
		adc_conversion_complete = 0;

		output = (adc_value * 40.0f) / 4095.0f;
		prepare_poll();
	}

	return output;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  if (hadc->Instance == ADC1)
  {
    // Read converted value
    adc_value = HAL_ADC_GetValue(hadc);
    adc_conversion_complete = 1;

    // Optionally start next conversion
    // HAL_ADC_Start_IT(&hadc1);
  }
}
