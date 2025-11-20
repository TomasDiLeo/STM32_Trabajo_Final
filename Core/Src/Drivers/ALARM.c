#include <Drivers/ALARM.h>

static Sensor high_priority_sensor;
static Sensor low_priority_sensor;
static Alarm high_priority_alarm;
static Alarm low_priority_alarm;

static uint32_t last_toggle = 0;
static GPIO_PinState read_sensor(Sensor sensor);
static Alarm get_active_alarm(void);

void alarm_init(){
	low_priority_sensor.pin = GPIO_PIN_11;
	low_priority_sensor.port = GPIOB;

	high_priority_sensor.pin = GPIO_PIN_12;
	high_priority_sensor.port = GPIOB;

	high_priority_alarm.sensor = high_priority_sensor;
	high_priority_alarm.toggle_time = HIGH_PRIORITY_TIME;

	low_priority_alarm.sensor = low_priority_sensor;
	low_priority_alarm.toggle_time = LOW_PRIORITY_TIME;
}

void handle_alarm(void) {
    Alarm active = get_active_alarm();

    if (active.sensor.pin != 0) {
        uint32_t current_time = HAL_GetTick();

        if (current_time - last_toggle >= active.toggle_time) {
            TOGGLE_PIN(ALARM_LED);
            last_toggle = current_time;
        }
    } else {
        WRITE_PIN(ALARM_LED, GPIO_PIN_RESET);
        last_toggle = 0;
    }
}

void reset_alarm(void){
	WRITE_PIN(ALARM_LED, GPIO_PIN_RESET);
}

static GPIO_PinState read_sensor(Sensor sensor) {
    return HAL_GPIO_ReadPin(sensor.port, sensor.pin);
}

static Alarm get_active_alarm(void) {
    if (read_sensor(high_priority_sensor) == GPIO_PIN_RESET) {
        return high_priority_alarm;
    }
    if (read_sensor(low_priority_sensor) == GPIO_PIN_RESET) {
        return low_priority_alarm;
    }

    // Return a default alarm when no sensors are active
    static Alarm no_alarm = {0};
    return no_alarm;
}
