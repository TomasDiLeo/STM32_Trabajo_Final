#ifndef STATES_H
#define STATES_H

//#define PROTEUS //COMENTAR PARA HARDWARE REAL
#ifdef PROTEUS
	#define SHORT_DELAY 100
	#define MID_DELAY 200
	#define LONG_DELAY 500
	#define EXTRA_LONG_DELAY 1200
#else
	#define SHORT_DELAY 1000
	#define MID_DELAY 2000
	#define LONG_DELAY 5000
	#define EXTRA_LONG_DELAY 15000
#endif


#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "Drivers/CLOCK.h"
#include "Drivers/KEYPAD.h"
#include "Drivers/LCD_NC.h"
#include "Drivers/TEMPERATURE_SENSOR.h"
#include "Drivers/ALARM.h"
#include "Drivers/AIR_CONDITIONING.h"
#include "editor.h"

#define SHOPWINDOW_LED_Pin GPIO_PIN_15
#define SHOPWINDOW_LED_GPIO_Port GPIOB

typedef enum {
	IDLE = 0,
	INFO,
	CLOCK_EDIT,
	TEMP_EDIT,
	SHOPWINDOW_EDIT,
	TIME_EDIT,
	DATE_EDIT,
	SHOPWINDOW_START_EDIT,
	SHOPWINDOW_END_EDIT
} App_State_t;

typedef void (*StateSetup)(void);
typedef uint8_t (*StateLoop)(void);

typedef struct {
	StateSetup setup;
	StateLoop loop;
} StateFunction;

extern App_State_t state;

void state_entry_point(App_State_t state);

// IDLE STATE FUNCTIONS
void idle_setup();
uint8_t idle_loop(void);

//INFO STATE FUNCTIONS
void info_setup(void);
uint8_t info_loop(void);

// CLOCK EDIT STATE FUNCTIONS
void clock_e_setup(void);
uint8_t clock_e_loop(void);

void time_edit_setup(void);
uint8_t time_edit_loop(void);
void date_edit_setup(void);
uint8_t date_edit_loop(void);

// TEMPERATURE EDIT STATE FUNCTIONS
void temp_e_setup(void);
uint8_t temp_e_loop(void);

//SHOPWINDOW EDIT STATE FUNCTIONS
void shopwindow_e_setup(void);
uint8_t shopwindow_e_loop(void);

void shopwindow_start_setup(void);
uint8_t shopwindow_start_loop(void);
void shopwindow_end_setup(void);
uint8_t shopwindow_end_loop(void);

#endif /* MENU_H */
