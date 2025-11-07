#include <system.h>

App_State_t state = IDLE;

//Input Variables
static uint8_t key_buffer;
static char string_buffer[30];

//Editor Variables
static const uint8_t cursor_map[6] = { 0, 1, 3, 4, 6, 7 };

static uint8_t exit_edit_mode = 0;
static uint8_t col_selection = 0;
static uint8_t input_buffer[6] = { 0 };

//Temperature reading variables
static float temp_buffer[10] = { 0 };
static uint8_t temp_index = 0;
static float temp = 0.0f;

//SHOPWINDOW range variables
static uint8_t hour_start = 21;
static uint8_t minutes_start = 30;
static uint8_t hour_end = 7;
static uint8_t minutes_end = 15;

//Programmed temperature
static uint8_t programmed_temp = 24;

//generic timer variable for timeout logic
static uint32_t timer;

//Function pairs for state logic
static StateFunction state_functions[] = {
		{idle_setup, idle_loop },
		{info_setup, info_loop },
		{clock_e_setup, clock_e_loop},
		{temp_e_setup, temp_e_loop },
		{shopwindow_e_setup, shopwindow_e_loop },
		{time_edit_setup, time_edit_loop},
		{date_edit_setup, date_edit_loop},
		{shopwindow_start_setup, shopwindow_start_loop},
		{shopwindow_end_setup, shopwindow_end_loop}
};

static uint8_t shopwindowState(void);
static void format_temp(float temp, char *buffer);
static void average_temp_sensor(void);

/*
 * @brief Updates system variables and states such as the
 * system's clock and temperature moving average
 * */
static void system_update(void) {
	//Moving Average Temperature reading logic
	average_temp_sensor();

	//Update system clock
	clock_update_datetime();

	if (shopwindowState()) {
		WRITE_PIN(SHOPWINDOW_LED, GPIO_PIN_SET);
		handle_alarm();
		reset_air_conditioning();
	} else {
		WRITE_PIN(SHOPWINDOW_LED, GPIO_PIN_RESET);
		handle_air_conditioning(datetime.season, programmed_temp, temp);
		reset_alarm();
	}
}

void state_entry_point(App_State_t state) {
	StateFunction *state_function = &state_functions[state];

	key_buffer = 0;
	state_function->setup();

	uint8_t exit = 0;
	while (!exit) {
		key_buffer = keypad_read();
		system_update();

		exit = state_function->loop();
	}

	system_update();
}

// IDLE STATE FUNCTIONS

void idle_setup() {
	lcd_send_cmd(CUR_OFF_BLINK_OFF);

	lcd_put_cur(0, 0);
	char *wd = week_day_to_string(datetime.week_day);
	sprintf(string_buffer, "%.7s", wd);
	lcd_send_string(string_buffer);
	lcd_send_data(' ');
	sprintf(string_buffer, "%02u/%02u/%02u (FECHA)", datetime.date, datetime.month, datetime.year);
	lcd_send_string(string_buffer);

	lcd_put_cur(1, 0);
	sprintf(string_buffer, "%02u:%02u", datetime.hours, datetime.minutes);
	lcd_send_string(string_buffer);
	lcd_send_string(" Temp00.0 ");
	lcd_send_data(datetime.season);
}

uint8_t idle_loop(void) {
	static uint8_t last_minute = 61;
	static uint8_t last_date = 32;
	static uint8_t blink_counter = 0;

	switch (key_buffer) {
	case 11:
		state = INFO;
		return 1;
	case 12:
		state = CLOCK_EDIT;
		return 1;
	case 13:
		state = TEMP_EDIT;
		return 1;
	case 14:
		state = SHOPWINDOW_EDIT;
		return 1;
	}

	//UPDATE TEMPERATURE
	lcd_put_cur(1, 10);
	format_temp(temp, string_buffer);
	lcd_send_string(string_buffer);

	//BLINKING COLON
	blink_counter = (blink_counter + 1) & 0x07;

	lcd_put_cur(1, 2);
	if (blink_counter < 4) {
		lcd_send_string(":");
	} else {
		lcd_send_string(" ");
	}

	//UPDATE DATE AND SEASON EVERY DAY
	if (last_date != datetime.date) {
		lcd_put_cur(0, 0);
		char *wd = week_day_to_string(datetime.week_day);
		sprintf(string_buffer, "%.7s", wd);
		lcd_send_string(string_buffer);
		lcd_send_data(' ');
		sprintf(string_buffer, "%02u/%02u/%02u (FECHA)", datetime.date, datetime.month, datetime.year);
		lcd_send_string(string_buffer);

		lcd_put_cur(1, 15);
		lcd_send_data(datetime.season);

		last_date = datetime.date;
	}

	//UPDATE TIME EVERY MINUTE
	if (last_minute != datetime.minutes) {
		lcd_put_cur(1, 0);
		sprintf(string_buffer, "%02u:%02u", datetime.hours, datetime.minutes);
		lcd_send_string(string_buffer);

		last_minute = datetime.minutes;
	}

	return 0;
}

//INFO STATE FUNCTIONS

void info_setup(void) {
	lcd_send_cmd(CUR_OFF_BLINK_OFF);

	lcd_put_cur(0, 0);
	sprintf(string_buffer, "Temp P:%02u M:00.0", programmed_temp);
	lcd_send_string(string_buffer);

	lcd_put_cur(1, 0);
	sprintf(string_buffer, "Vidr %02u:%02u-%02u:%02u", hour_start,
			minutes_start, hour_end, minutes_end);
	lcd_send_string(string_buffer);
}

uint8_t info_loop(void) {
	if (key_buffer == 15) {
		state = IDLE;
		return 1;
	}

	lcd_put_cur(0, 12);
	format_temp(temp, string_buffer);
	lcd_send_string(string_buffer);

	return 0;
}

// CLOCK EDIT

// CLOCK EDIT STATE VARIABLES
static App_State_t clock_e_selection = DATE_EDIT;

// CLOCK EDIT STATE FUNCTIONS
void clock_e_setup(void) {
	lcd_send_cmd(CUR_OFF_BLINK_OFF);
	timer = HAL_GetTick();

	lcd_put_cur(0, 0);
	lcd_send_string("EDIT RELOJ  B:  ");
	lcd_put_cur(0, 14);
	send_lcd_ASCII(0x7F); // <-
	lcd_put_cur(0, 15);
	send_lcd_ASCII(0x7E); // ->

	lcd_put_cur(1, 0);
	lcd_send_string("   FECHA   HORA ");

	if (clock_e_selection == TIME_EDIT) {
		lcd_put_cur(1, 9);
		lcd_send_data('*');
	} else if (clock_e_selection == DATE_EDIT) {
		lcd_put_cur(1, 1);
		lcd_send_data('*');
	}
}

uint8_t clock_e_loop(void) {

	//ESCAPE AND TIMEOUT
	if (key_buffer == 15 || HAL_GetTick() - timer > EXTRA_LONG_DELAY) {
		state = IDLE;
		return 1;
	}

	//ENTER
	if (key_buffer == 16) {
		state = clock_e_selection;
		return 1;
	}

	//SELECTION TOGGLE
	if (key_buffer == 12) {
		timer = HAL_GetTick();

		if (clock_e_selection == DATE_EDIT) {
			clock_e_selection = TIME_EDIT;
			lcd_put_cur(1, 9);
			lcd_send_data('*');
			lcd_put_cur(1, 1);
			lcd_send_data(' ');
		} else if (clock_e_selection == TIME_EDIT) {
			clock_e_selection = DATE_EDIT;
			lcd_put_cur(1, 9);
			lcd_send_data(' ');
			lcd_put_cur(1, 1);
			lcd_send_data('*');
		}
	}

	return 0;
}

static uint8_t edition_mode(uint8_t digits, const uint8_t *cursor_map);

void time_edit_setup(void) {
	exit_edit_mode = 0;
	timer = HAL_GetTick();
	col_selection = 0;

	input_buffer[0] = datetime.hours / 10;
	input_buffer[1] = datetime.hours % 10;
	input_buffer[2] = datetime.minutes / 10;
	input_buffer[3] = datetime.minutes % 10;
	input_buffer[4] = datetime.seconds / 10;
	input_buffer[5] = datetime.seconds % 10;

	lcd_put_cur(0, 0);
	lcd_send_string(" A:   B:   #:ENT");
	lcd_put_cur(0, 4);
	send_lcd_ASCII(0x7E); //->
	lcd_put_cur(0, 9);
	send_lcd_ASCII(0x7F); //<-

	lcd_put_cur(1, 0);
	sprintf(string_buffer, "%2u:%2u:%2u  (HORA)", datetime.hours, datetime.minutes, datetime.seconds);
	lcd_send_string(string_buffer);

	lcd_send_cmd(CUR_ON_BLINK_ON);
}

uint8_t time_edit_loop(void) {

	exit_edit_mode = edition_mode(6, cursor_map);
	state = CLOCK_EDIT;
	//ENTER
	if (key_buffer == 16) {

		status_buffer = clock_set_time(input_buffer[0] * 10 + input_buffer[1],
				input_buffer[2] * 10 + input_buffer[3],
				input_buffer[4] * 10 + input_buffer[5]);
		return 1;
	}

	return exit_edit_mode;
}

void date_edit_setup(void) {
	exit_edit_mode = 0;
	timer = HAL_GetTick();
	col_selection = 0;

	input_buffer[0] = datetime.date / 10;
	input_buffer[1] = datetime.date % 10;
	input_buffer[2] = datetime.month / 10;
	input_buffer[3] = datetime.month % 10;
	input_buffer[4] = datetime.year / 10;
	input_buffer[5] = datetime.year % 10;

	lcd_put_cur(0, 0);
	lcd_send_string(" A:   B:   #:ENT");
	lcd_put_cur(0, 4);
	send_lcd_ASCII(0x7E); //->
	lcd_put_cur(0, 9);
	send_lcd_ASCII(0x7F); //<-

	lcd_put_cur(1, 0);
	sprintf(string_buffer, "%02u/%02u/%02u (FECHA)", datetime.date, datetime.month, datetime.year);
	lcd_send_string(string_buffer);

	lcd_send_cmd(CUR_ON_BLINK_ON);
}

uint8_t date_edit_loop(void) {

	exit_edit_mode = edition_mode(6, cursor_map);
	state = CLOCK_EDIT;
	//ENTER
	if (key_buffer == 16) {

		status_buffer = clock_set_date(input_buffer[0] * 10 + input_buffer[1],
				input_buffer[2] * 10 + input_buffer[3],
				input_buffer[4] * 10 + input_buffer[5]);
		return 1;
	}

	return exit_edit_mode;
}

static uint8_t edition_mode(uint8_t digits, const uint8_t *cursor_map) {
	// ESCAPE AND TIMEOUT
	if (key_buffer == 15 || HAL_GetTick() - timer > EXTRA_LONG_DELAY) {
		return 1;
	}

	// MOVE CURSOR RIGHT AND LEFT WITH ROLLOVER
	if (key_buffer == 11) { // move right
		col_selection = (col_selection + 1) % digits;
		timer = HAL_GetTick();
	}
	if (key_buffer == 12) { // move left
		col_selection = (col_selection + digits - 1) % digits;
		timer = HAL_GetTick();
	}

	// POSITION THE CURSOR using map
	lcd_put_cur(1, cursor_map[col_selection]);

	// INSERT NUMBER KEY
	if (key_buffer > 0 && key_buffer <= 10) {
		timer = HAL_GetTick(); // reset timeout on input
		if (key_buffer == 10)
			key_buffer = 0; // '0' key

		input_buffer[col_selection] = key_buffer;
		sprintf(string_buffer, "%1d", key_buffer);
		lcd_send_string(string_buffer);
		col_selection = (col_selection + 1) % digits;
	}

	return 0;
}

// TEMPERATURE EDIT STATE FUNCTIONS

void temp_e_setup(void) {
	exit_edit_mode = 0;
	timer = HAL_GetTick();
	col_selection = 0;

	input_buffer[0] = programmed_temp / 10;
	input_buffer[1] = programmed_temp % 10;

	lcd_put_cur(0, 0);
	lcd_send_string(" A:   B:   #:ENT");
	lcd_put_cur(0, 4);
	send_lcd_ASCII(0x7E); //->
	lcd_put_cur(0, 9);
	send_lcd_ASCII(0x7F); //<-

	lcd_put_cur(1, 0);
	sprintf(string_buffer, "%02u TEMP C       ", programmed_temp);
	lcd_send_string(string_buffer);

	lcd_send_cmd(CUR_ON_BLINK_ON);
}

uint8_t temp_e_loop(void) {
	static uint8_t temp_temperature = 50;
	exit_edit_mode = edition_mode(2, cursor_map);
	state = IDLE;
	//ENTER
	if (key_buffer == 16) {

		temp_temperature = input_buffer[0] * 10 + input_buffer[1];

		if (temp_temperature <= 40) {
			programmed_temp = temp_temperature;
			status_buffer = CLOCK_OK;
			return 1;
		}

		status_buffer = CLOCK_CRITICAL_ERROR;
		return 1;
	}

	return exit_edit_mode;
}

//SHOPWINDOW EDIT STATE FUNCTIONS

static App_State_t shopwindow_e_selection = SHOPWINDOW_START_EDIT;

void shopwindow_e_setup(void) {
	lcd_send_cmd(CUR_OFF_BLINK_OFF);
	timer = HAL_GetTick();

	lcd_put_cur(0, 0);
	lcd_send_string(" VIDRIERA   D:  ");
	lcd_put_cur(0, 14);
	send_lcd_ASCII(0x7F); // <-
	lcd_put_cur(0, 15);
	send_lcd_ASCII(0x7E); // ->

	lcd_put_cur(1, 0);
	lcd_send_string("  INICIO   FINAL");

	if (shopwindow_e_selection == SHOPWINDOW_START_EDIT) {
		lcd_put_cur(1, 0);
		lcd_send_data('*');
	} else if (shopwindow_e_selection == SHOPWINDOW_END_EDIT) {
		lcd_put_cur(1, 9);
		lcd_send_data('*');
	}
}

uint8_t shopwindow_e_loop(void) {

	//ESCAPE AND TIMEOUT
	if (key_buffer == 15 || HAL_GetTick() - timer > EXTRA_LONG_DELAY) {
		state = IDLE;
		return 1;
	}

	//ENTER
	if (key_buffer == 16) {
		state = shopwindow_e_selection;
		return 1;
	}

	//SELECTION TOGGLE
	if (key_buffer == 14) {
		timer = HAL_GetTick();

		if (shopwindow_e_selection == SHOPWINDOW_START_EDIT) {
			shopwindow_e_selection = SHOPWINDOW_END_EDIT;
			lcd_put_cur(1, 9);
			lcd_send_data('*');
			lcd_put_cur(1, 0);
			lcd_send_data(' ');
		} else if (shopwindow_e_selection == SHOPWINDOW_END_EDIT) {
			shopwindow_e_selection = SHOPWINDOW_START_EDIT;
			lcd_put_cur(1, 9);
			lcd_send_data(' ');
			lcd_put_cur(1, 0);
			lcd_send_data('*');
		}
	}

	return 0;
}

void shopwindow_start_setup(void) {
	exit_edit_mode = 0;
	timer = HAL_GetTick();
	col_selection = 0;

	input_buffer[0] = hour_start / 10;
	input_buffer[1] = hour_start % 10;
	input_buffer[2] = minutes_start / 10;
	input_buffer[3] = minutes_start % 10;

	lcd_put_cur(0, 0);
	lcd_send_string(" A:   B:   #:ENT");
	lcd_put_cur(0, 4);
	send_lcd_ASCII(0x7E); //->
	lcd_put_cur(0, 9);
	send_lcd_ASCII(0x7F); //<-

	lcd_put_cur(1, 0);
	sprintf(string_buffer, "%02u:%02u   (INICIO)", hour_start, minutes_start);
	lcd_send_string(string_buffer);

	lcd_send_cmd(CUR_ON_BLINK_ON);
}

uint8_t shopwindow_start_loop(void) {
	static uint8_t temp_hour = 24;
	static uint8_t temp_minutes = 61;

	exit_edit_mode = edition_mode(4, cursor_map);
	state = SHOPWINDOW_EDIT;
	//ENTER
	if (key_buffer == 16) {

		temp_hour = input_buffer[0] * 10 + input_buffer[1];
		temp_minutes = input_buffer[2] * 10 + input_buffer[3];

		if (temp_hour < 24 && temp_minutes < 60) {
			hour_start = temp_hour;
			minutes_start = temp_minutes;
			status_buffer = CLOCK_OK;
			return 1;
		}
		if (temp_hour > 23)
			status_buffer = CLOCK_ERROR_INVALID_HOUR;
		if (temp_minutes > 59)
			status_buffer = CLOCK_ERROR_INVALID_MINUTE;
		return 1;
	}

	return exit_edit_mode;
}

void shopwindow_end_setup(void) {
	exit_edit_mode = 0;
	timer = HAL_GetTick();
	col_selection = 0;

	input_buffer[0] = hour_end / 10;
	input_buffer[1] = hour_end % 10;
	input_buffer[2] = minutes_end / 10;
	input_buffer[3] = minutes_end % 10;

	lcd_put_cur(0, 0);
	lcd_send_string(" A:   B:   #:ENT");
	lcd_put_cur(0, 4);
	send_lcd_ASCII(0x7E); //->
	lcd_put_cur(0, 9);
	send_lcd_ASCII(0x7F); //<-

	lcd_put_cur(1, 0);
	sprintf(string_buffer, "%02u:%02u    (FINAL)", hour_end, minutes_end);
	lcd_send_string(string_buffer);

	lcd_send_cmd(CUR_ON_BLINK_ON);
}

uint8_t shopwindow_end_loop(void) {
	static uint8_t temp_hour = 24;
	static uint8_t temp_minutes = 61;

	exit_edit_mode = edition_mode(4, cursor_map);
	state = SHOPWINDOW_EDIT;
	//ENTER
	if (key_buffer == 16) {

		temp_hour = input_buffer[0] * 10 + input_buffer[1];
		temp_minutes = input_buffer[2] * 10 + input_buffer[3];

		if (temp_hour < 24 && temp_minutes < 60) {
			hour_end = temp_hour;
			minutes_end = temp_minutes;
			status_buffer = CLOCK_OK;
			return 1;
		}
		if (temp_hour > 23)
			status_buffer = CLOCK_ERROR_INVALID_HOUR;
		if (temp_minutes > 59)
			status_buffer = CLOCK_ERROR_INVALID_MINUTE;
		return 1;
	}

	return exit_edit_mode;
}

// STATIC HELPER FUNCTIONS

static uint8_t shopwindowState(void) {
	uint16_t now = (uint16_t) datetime.hours * 60u
			+ (uint16_t) datetime.minutes;
	uint16_t start = (uint16_t) hour_start * 60u + (uint16_t) minutes_start;
	uint16_t end = (uint16_t) hour_end * 60u + (uint16_t) minutes_end;

	if (start == end) {
		// interpret equal start and end as "always on"
		return 1;
	}

	if (start < end) {
		// simple daytime range, e.g. 09:00 - 17:00
		if (now >= start && now < end)
			return 1;
		else
			return 0;
	} else {
		// overnight range, e.g. 23:30 - 07:30
		if (now >= start || now < end)
			return 1;
		else
			return 0;
	}
}

static void format_temp(float temp, char *buffer) {
	int whole = (int) temp;
	int decimal = (int) ((temp - whole) * 10);
	buffer[0] = (whole / 10) + '0';
	buffer[1] = (whole % 10) + '0';
	buffer[2] = '.';
	buffer[3] = decimal + '0';
	buffer[4] = '\0';
}

static void average_temp_sensor(void) {
	static float temp_sum = 0;
	float current = poll_sensor();
	temp_sum -= temp_buffer[temp_index];
	temp_buffer[temp_index] = current;
	temp_sum += current;
	temp_index = (temp_index + 1) % 10;
	temp = temp_sum / 10;
}
