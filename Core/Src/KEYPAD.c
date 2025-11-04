#include "KEYPAD.h"

static void select_row(uint8_t row);

static uint8_t last_key = 0;

uint8_t keypad_read(void){
	uint8_t key = 0;
	for(uint8_t row = 0; row < 4; row++){
		HAL_Delay(1);
		select_row(row + 1);

		if (READ_PIN(COL_1) == GPIO_PIN_SET) {key = keymap[row][0];break;};
		if (READ_PIN(COL_2) == GPIO_PIN_SET) {key = keymap[row][1];break;};
		if (READ_PIN(COL_3) == GPIO_PIN_SET) {key = keymap[row][2];break;};
		if (READ_PIN(COL_4) == GPIO_PIN_SET) {key = keymap[row][3];break;};
	}

	select_row(0); //RESET ALL ROWS

	if(key == last_key) return 0;
	last_key = key;
	return key;
}

static void select_row(uint8_t row){
	WRITE_PIN(ROW_1, (row == 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	WRITE_PIN(ROW_2, (row == 2) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	WRITE_PIN(ROW_3, (row == 3) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	WRITE_PIN(ROW_4, (row == 4) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
