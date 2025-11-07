#ifndef EDITOR_H
#define EDITOR_H

#include "stm32f1xx_hal.h"
#include "Drivers/LCD_NC.h"

#define EDITOR_TIMEOUT 15000

typedef void (*OnUpdateCallback)(uint8_t position, uint8_t value);
typedef uint8_t (*OnSaveCallback)(uint8_t *buffer);
typedef void (*OnSetupCallback)();

typedef struct{
	OnSetupCallback message;
	OnUpdateCallback on_update;
} EditorDisplay;

typedef struct{
	uint8_t buffer[6];
	uint8_t column_selection;
	uint8_t max_digits;

	uint8_t exit;
	uint32_t timer;
	uint32_t timeout_ms;

	OnSaveCallback on_save;
	EditorDisplay display;
} Editor;

extern const uint8_t cursor_map[6];

void editor_setup(uint8_t *initial_values, uint8_t size, Editor *editor);
uint8_t editor_loop(uint8_t pressed_key, Editor *editor);




#endif /* EDITOR_H */
