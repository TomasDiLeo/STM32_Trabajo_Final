#ifndef CLOCK_H
#define CLOCK_H

#include "stm32f1xx_hal.h"

typedef enum {
	CLOCK_OK = 0,
    CLOCK_ERROR_INVALID_HOUR,
    CLOCK_ERROR_INVALID_MINUTE,
    CLOCK_ERROR_INVALID_SECOND,
    CLOCK_ERROR_INVALID_DATE,
    CLOCK_ERROR_INVALID_MONTH,
    CLOCK_ERROR_INVALID_YEAR,
	CLOCK_CRITICAL_ERROR,
	REST
} Clock_Status_t;

typedef struct {
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;

	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t week_day;

	char season;
} DateTime;

extern DateTime datetime;
extern Clock_Status_t status_buffer;

// Function declarations
void clock_init(RTC_HandleTypeDef *hrtc);
Clock_Status_t clock_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);
Clock_Status_t clock_set_date(uint8_t date, uint8_t month, uint8_t year);
void clock_update_datetime(void);
char * clock_error_string(Clock_Status_t code);
char * week_day_to_string(uint8_t week_day);
char season();

#endif // CLOCK_H
