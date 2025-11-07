#include "Drivers/CLOCK.h"

DateTime datetime = {
		.hours = 0,
		.minutes = 0,
		.seconds = 0,
		.date = 20,
		.month = 10,
		.year = 25,
		.season = 'P'
};
Clock_Status_t status_buffer = REST;

static RTC_HandleTypeDef *hrtc_ptr = NULL;

static int is_leap_year(uint8_t year);
static Clock_Status_t validate_date(uint8_t date, uint8_t month, uint8_t year);
static Clock_Status_t validate_time(uint8_t hours, uint8_t minutes, uint8_t seconds);

void clock_init(RTC_HandleTypeDef *hrtc) {
    hrtc_ptr = hrtc;

    clock_set_time(datetime.hours, datetime.minutes, datetime.seconds);
    clock_set_date(datetime.date, datetime.month, datetime.year);

    clock_update_datetime();
}

static uint8_t clock_int_to_bcd(uint8_t integer) {
    return ((integer / 10) << 4) | (integer % 10);
}

Clock_Status_t clock_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    if (!hrtc_ptr) return CLOCK_CRITICAL_ERROR;
    Clock_Status_t status = validate_time(hours, minutes, seconds);
    if (status != CLOCK_OK) return status;

    RTC_TimeTypeDef sTime = {0};
    sTime.Hours = clock_int_to_bcd(hours);
    sTime.Minutes = clock_int_to_bcd(minutes);
    sTime.Seconds = clock_int_to_bcd(seconds);

    if (HAL_RTC_SetTime(hrtc_ptr, &sTime, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler();
    }

    return CLOCK_OK;
}

Clock_Status_t clock_set_date(uint8_t date, uint8_t month, uint8_t year) {
    if (!hrtc_ptr) return CLOCK_CRITICAL_ERROR;
    Clock_Status_t status = validate_date(date, month, year);
    if (status != CLOCK_OK) return status;

    RTC_DateTypeDef sDate = {0};
    sDate.Date = clock_int_to_bcd(date);
    sDate.Month = clock_int_to_bcd(month);
    sDate.Year = clock_int_to_bcd(year);

    if (HAL_RTC_SetDate(hrtc_ptr, &sDate, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler();
    }

    return CLOCK_OK;
}

void clock_update_datetime(void){
	if (!hrtc_ptr) return;

	RTC_TimeTypeDef gTime;
    RTC_DateTypeDef gDate;

    HAL_RTC_GetTime(hrtc_ptr, &gTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(hrtc_ptr, &gDate, RTC_FORMAT_BIN);

    datetime.hours = gTime.Hours;
    datetime.minutes = gTime.Minutes;
    datetime.seconds = gTime.Seconds;

    datetime.date = gDate.Date;
    datetime.month = gDate.Month;
    datetime.year = gDate.Year;

    datetime.week_day = gDate.WeekDay;
    datetime.season = season();
}

static int is_leap_year(uint8_t year){
	return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

static Clock_Status_t validate_date(uint8_t date, uint8_t month, uint8_t year){
	if (date < 1 || date > 31) return CLOCK_ERROR_INVALID_DATE;
	if (month < 1 || month > 12) return CLOCK_ERROR_INVALID_MONTH;
	if (year > 99) return CLOCK_ERROR_INVALID_YEAR;

	if ((month == 4 || month == 6 || month == 9 || month == 11) && date > 30) return CLOCK_ERROR_INVALID_DATE;
	if ((month == 2) && (!is_leap_year(year)) && date > 28) return CLOCK_ERROR_INVALID_DATE;

	return CLOCK_OK;
}

static Clock_Status_t validate_time(uint8_t hours, uint8_t minutes, uint8_t seconds){
	if ((hours < 0) || (hours > 23)) return CLOCK_ERROR_INVALID_HOUR;
	if ((minutes < 0) || (minutes > 59)) return CLOCK_ERROR_INVALID_MINUTE;
	if ((seconds < 0) || (seconds > 59)) return CLOCK_ERROR_INVALID_SECOND;

	return CLOCK_OK;
}

char * clock_error_string(Clock_Status_t code){
	switch(code){
	case CLOCK_OK: 						return "CAMBIO GUARDADO ";
	case CLOCK_ERROR_INVALID_HOUR: 		return "HORA INVALIDA   ";
	case CLOCK_ERROR_INVALID_MINUTE: 	return "MINUTO INVALIDO ";
	case CLOCK_ERROR_INVALID_SECOND:	return "SEGUNDO INVALIDO";
	case CLOCK_ERROR_INVALID_DATE:		return "FECHA INVALIDA  ";
	case CLOCK_ERROR_INVALID_MONTH:		return "MES INVALIDO    ";
	case CLOCK_ERROR_INVALID_YEAR:		return "ANIO INVALIDO   ";
	case CLOCK_CRITICAL_ERROR:			return "CAMBIO INVALIDO ";
	case REST:
	}
	return "";
}

char * week_day_to_string(uint8_t week_day)
{
	switch (week_day) {
		case 6: return "Sabado";
		case 0: return "Domingo";
		case 1: return "Lunes";
		case 2: return "Martes";
		case 3: return "Miercoles";
		case 4: return "Jueves";
		case 5: return "Viernes";
		default:
			return "ERROR";
	}
}

char season(void) {
	uint8_t m = datetime.month;
	uint8_t d = datetime.date;
	return (m==3&&d>=21)||(m>3&&m<6)||(m==6&&d<=20) ? 'O' :
	       (m==6&&d>=21)||(m>6&&m<9)||(m==9&&d<=20) ? 'I' :
	       (m==9&&d>=21)||(m>9&&m<12)||(m==12&&d<=20) ? 'P' : 'V';
}

