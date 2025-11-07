#include "editor.h"

static void move_with_rollover(uint8_t *variable, uint8_t move_by, uint8_t rollover_digit);

const uint8_t cursor_map[6] = { 0, 1, 3, 4, 6, 7 };

void editor_setup(uint8_t *initial_values, uint8_t size, Editor *editor){
	for(int i = 0; i < size; i++){
		editor->buffer[i] = initial_values[i];
	}

	editor->column_selection = 0;
	editor->exit = 0;
	editor->timer = HAL_GetTick();
	editor->display.message();
}

uint8_t editor_loop(uint8_t pressed_key, Editor *editor){
	// MOVE CURSOR RIGHT AND LEFT WITH ROLLOVER
	if (pressed_key == 11) { // move right
		move_with_rollover(&editor->column_selection, 1, editor->max_digits);
		editor->timer = HAL_GetTick();
	}
	if (pressed_key == 12) { // move left
		move_with_rollover(&editor->column_selection, editor->max_digits - 1, editor->max_digits);
		editor->timer = HAL_GetTick();
	}

	// INSERT NUMBER KEY
	if (pressed_key > 0 && pressed_key <= 10) {
		editor->timer = HAL_GetTick(); // reset timeout on input
		if (pressed_key == 10) pressed_key = 0; // '0' key

		editor->buffer[editor->column_selection] = pressed_key;
		editor->display.on_update(editor->column_selection, pressed_key);
		move_with_rollover(&editor->column_selection, 1, editor->max_digits);
	}

	// ESCAPE AND TIMEOUT
	if (pressed_key == 15 || HAL_GetTick() - editor->timer > editor->timeout_ms) {
		editor->exit = 1;
	}

	//ENTER
	if (pressed_key == 16){
		editor->exit = 1;
		return editor->on_save(editor->buffer);
	}

	return 0;
}

uint8_t get_number(uint8_t position, Editor *editor){
	return editor->buffer[position] * 10 + editor->buffer[position + 1];
}

static void move_with_rollover(uint8_t *variable, uint8_t move_by, uint8_t rollover_digit){
	*variable = (*variable + move_by) % rollover_digit;
}
