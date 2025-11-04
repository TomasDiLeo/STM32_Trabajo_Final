#include <states.h>

App_State_t state = IDLE;

static uint8_t key_buffer;
static char string_buffer[30];
static uint8_t temp_index = 0;
static float temp_buffer[10] = {0};
static float temp = 0.0f;

static uint32_t timer;

static StateFunction state_functions[] = {
		{idle_setup, idle_loop},
		{info_setup, info_loop},
		{clock_e_setup, clock_e_loop},
		{temp_e_setup, temp_e_loop},
		{shopwindow_e_setup, shopwindow_e_loop},
		{time_edit_setup, time_edit_loop},
		{date_edit_setup, date_edit_loop}
};

static void format_temp(float temp, char* buffer) {
    int whole = (int)temp;
    int decimal = (int)((temp - whole) * 10);
    buffer[0] = (whole / 10) + '0';
    buffer[1] = (whole % 10) + '0';
    buffer[2] = '.';
    buffer[3] = decimal + '0';
    buffer[4] = '\0';
}

static void system_update(void){
	float current_temp = poll_sensor();

	temp_buffer[temp_index] = current_temp;
	temp_index = (temp_index + 1) % 10;

	float sum = 0.0f;
	for (int i = 0; i < 10; i++){
		sum += temp_buffer[i];
	}
	temp = sum / 10;

	clock_update_datetime();
}

void state_entry_point(App_State_t state){
	StateFunction* state_function = &state_functions[state];

	key_buffer = 0;
	state_function->setup();

	uint8_t exit = 0;
	while(!exit){
		key_buffer = keypad_read();
		system_update();

		exit = state_function->loop();
	}
}

// IDLE STATE FUNCTIONS

void idle_setup(){
    lcd_send_cmd(CUR_OFF_BLINK_OFF);

    lcd_put_cur(0, 0);
    char *wd = week_day_to_string(datetime.week_day);
    lcd_send_string(wd);
    lcd_send_data(' ');
    lcd_print_date(datetime.date, datetime.month, datetime.year);
    lcd_send_data(' ');

    lcd_put_cur(1, 0);
    lcd_print_time(datetime.hours, datetime.minutes);
    lcd_send_string(" Temp00.0 ");
    lcd_send_data(datetime.season);
}

uint8_t idle_loop(void){
	static uint8_t last_minute = 61;
	static uint8_t last_date = 32;
	static uint8_t blink_counter = 0;

	//UPDATE TEMPERATURE
	lcd_put_cur(1,10);
	format_temp(temp, string_buffer);
	lcd_send_string(string_buffer);

	//BLINKING COLON
	blink_counter = (blink_counter + 1) & 0x07; //BLINK IN RANGE 0-7

	lcd_put_cur(1, 2);
	if(blink_counter < 4){
		lcd_send_string(":");
	} else {
		lcd_send_string(" ");
	}

	//UPDATE DATE AND SEASON EVERY DAY
	if(last_date != datetime.date){
	    lcd_put_cur(0, 0);
	    char *wd = week_day_to_string(datetime.week_day);
	    lcd_send_string(wd);
	    lcd_send_data(' ');
	    lcd_print_date(datetime.date, datetime.month, datetime.year);

		lcd_put_cur(1, 15);
		lcd_send_data(datetime.season);

		last_date = datetime.date;
	}

	//UPDATE TIME EVERY MINUTE
	if(last_minute != datetime.minutes){
		lcd_put_cur(1, 0);
		lcd_print_time(datetime.hours, datetime.minutes);

		last_minute = datetime.minutes;
	}

	switch(key_buffer){
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

	return 0;
}

//INFO STATE FUNCTIONS

void info_setup(void){
	lcd_send_cmd(CUR_OFF_BLINK_OFF);

	lcd_put_cur(0, 0);
	lcd_send_string("Temp P:XX M:XX.X");
	lcd_put_cur(1, 0);
	lcd_send_string("Vidr XX:XX-XX:XX");
}

uint8_t info_loop(void){
	if(key_buffer == 15){
		state = IDLE;
		return 1;
	}

	return 0;
}

// CLOCK EDIT

	// CLOCK EDIT STATE VARIABLES
static uint8_t exit = 0;
static uint8_t col_selection = 0;
static uint8_t time_buffer[6] = {0};
static App_State_t selection = DATE_EDIT;

	// CLOCK EDIT STATE FUNCTIONS
void clock_e_setup(void){
	lcd_send_cmd(CUR_OFF_BLINK_OFF);
	timer = 0;

	lcd_put_cur(0, 0);
	lcd_send_string("EDITAR RELOJ (A)");
	lcd_put_cur(1, 0);
	lcd_send_string("   FECHA   HORA ");

	if(selection == TIME_EDIT){
		lcd_put_cur(1, 9);
		lcd_send_data('*');
	}else if(selection == DATE_EDIT){
		lcd_put_cur(1, 1);
		lcd_send_data('*');
	}
}

uint8_t clock_e_loop(void){

	//ESCAPE AND TIMEOUT
	if(key_buffer == 15 || timer > MID_DELAY){
		state = IDLE;
		return 1;
	}

	//ENTER
	if(key_buffer == 16){
		state = selection;
		return 1;
	}

	//SELECTION TOGGLE
	if(key_buffer == 11){
		timer = 0;

		if(selection == DATE_EDIT){
			selection = TIME_EDIT;
			lcd_put_cur(1, 9);
			lcd_send_data('*');
			lcd_put_cur(1, 1);
			lcd_send_data(' ');
		}else if(selection == TIME_EDIT){
			selection = DATE_EDIT;
			lcd_put_cur(1, 9);
			lcd_send_data(' ');
			lcd_put_cur(1, 1);
			lcd_send_data('*');
		}
	}

	timer += 1;
	return 0;
}

static uint8_t edition_mode(void);

void time_edit_setup(void){
	exit = 0;
	timer = 0;
	col_selection = 0;

	time_buffer[0] = datetime.hours / 10;
	time_buffer[1] = datetime.hours % 10;
	time_buffer[2] = datetime.minutes / 10;
	time_buffer[3] = datetime.minutes % 10;
	time_buffer[4] = datetime.seconds / 10;
	time_buffer[5] = datetime.seconds % 10;

	lcd_put_cur(0, 0);
	lcd_send_string(" A: DER | B: IZQ");
	lcd_put_cur(1, 0);
	lcd_print_time(datetime.hours, datetime.minutes);
	lcd_send_data(':');
	lcd_print2d(datetime.seconds);
	lcd_send_string("          ");

	lcd_send_cmd(CUR_ON_BLINK_ON);
}

uint8_t time_edit_loop(void){

	exit = edition_mode();

	//ENTER
	if(key_buffer == 16){

		status_buffer = clock_set_time(
				time_buffer[0] * 10 + time_buffer[1],
				time_buffer[2] * 10 + time_buffer[3],
				time_buffer[4] * 10 + time_buffer[5]
		);
		state = CLOCK_EDIT;
		return 1;
	}

	timer += 1;
	return exit;
}

void date_edit_setup(void){
	exit = 0;
	timer = 0;
	col_selection = 0;

	time_buffer[0] = datetime.date / 10;
	time_buffer[1] = datetime.date % 10;
	time_buffer[2] = datetime.month / 10;
	time_buffer[3] = datetime.month % 10;
	time_buffer[4] = datetime.year / 10;
	time_buffer[5] = datetime.year % 10;

	lcd_put_cur(0, 0);
	lcd_send_string(" A: DER | B: IZQ");
	lcd_put_cur(1, 0);
	lcd_print_date(datetime.date, datetime.month, datetime.year);
	lcd_send_string("          ");

	lcd_send_cmd(CUR_ON_BLINK_ON);
}

uint8_t date_edit_loop(void){

	exit = edition_mode();
	//ENTER
	if(key_buffer == 16){

		status_buffer = clock_set_date(
				time_buffer[0] * 10 + time_buffer[1],
				time_buffer[2] * 10 + time_buffer[3],
				time_buffer[4] * 10 + time_buffer[5]
		);
		state = CLOCK_EDIT;
		return 1;
	}

	timer += 1;
	return exit;
}

static uint8_t edition_mode(void){
	//ESCAPE AND TIMEOUT
	if(key_buffer == 15 || timer > MID_DELAY){
		state = CLOCK_EDIT;
		return 1;
	}

	//MOVE CURSOR RIGHT AND LEFT WITH ROLLOVER
	if(key_buffer == 11) {col_selection = (col_selection + 1) % 6; timer = 0;}
	if(key_buffer == 12) {col_selection = (col_selection - 1 + 6) % 6; timer = 0;}

	//POSITION THE CURSOR ON THE CORRESPONDING DISPLAY COLUMN
	if(col_selection == 2 || col_selection == 3) lcd_put_cur(1, col_selection + 1);
	else if (col_selection >= 4) lcd_put_cur(1, col_selection + 2);
	else lcd_put_cur(1, col_selection);

	//INSERT NUMBER KEY
	if(key_buffer > 0 && key_buffer <= 10){
		timer = 0;
		if(key_buffer == 10) key_buffer = 0;

		time_buffer[col_selection] = key_buffer;
		sprintf(string_buffer, "%1d", key_buffer);
		lcd_send_string(string_buffer);
		lcd_send_cmd(MOV_CUR_LEFT);
	}

	return 0;
}

// TEMPERATURE EDIT STATE FUNCTIONS

void temp_e_setup(void){

}

uint8_t temp_e_loop(void){

	return 0;
}

//SHOPWINDOW EDIT STATE FUNCTIONS

void shopwindow_e_setup(void){

}

uint8_t shopwindow_e_loop(void){

	return 0;
}


