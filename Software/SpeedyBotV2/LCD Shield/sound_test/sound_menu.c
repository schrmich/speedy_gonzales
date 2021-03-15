/*
 * menu.c
 *
 * Created: 24.01.2021 21:13:16
 *  Author: mschr
 */ 

#include "sound_menu.h"
#include "../lcd_shield.h"
#include "log.h"
#include "scheduler51.h"
#include "sound.h"
#include "notes.h"
#include "../menu.h"
#include <avr/pgmspace.h>

#define MENU_DISP_NUM	6	// Anzahl der Zeilen des Displays
#define MENU_MAX		10	// Anzahl der Menueinträge

// Startposition des Cursors 
// 1 wenn das Menu eine Überschrift hat, 0 wenn nein
int8_t sound_cursor = 1;

// Generischer Funktionszeiger
typedef void ( *generic_func_ptr )( void );


typedef struct MENU {
	const char *text;
	generic_func_ptr fp;
} MENU_ENTRY_T;

const char PROGMEM menu_string11[] = "*Sound Test*";
const char PROGMEM menu_string12[] = ">Indiana J.";
const char PROGMEM menu_string13[] = ">Tetris";
const char PROGMEM menu_string14[] = ">Beethoven";
const char PROGMEM menu_string15[] = ">Monkey Isl.";
const char PROGMEM menu_string16[] = ">Knight R.";
const char PROGMEM menu_string17[] = ">Mario Brot.";
const char PROGMEM menu_string18[] = ">Pacman";
const char PROGMEM menu_string19[] = ">Beep Sound";
const char PROGMEM menu_string20[] = ">Back to Main";


void menu_function11(void);
void menu_function12(void);
void menu_function13(void);
void menu_function14(void);
void menu_function15(void);
void menu_function16(void);
void menu_function17(void);
void menu_function18(void);
void menu_function19(void);
void menu_function20(void);

const MENU_ENTRY_T sound_menu[] PROGMEM = {
	{ menu_string11, NULL },
	{ menu_string12, menu_function12 },
	{ menu_string13, menu_function13 },
	{ menu_string14, menu_function14 },
	{ menu_string15, menu_function15 },
	{ menu_string16, menu_function16 },
	{ menu_string17, menu_function17 },
	{ menu_string18, menu_function18 },
	{ menu_string19, menu_function19 },
	{ menu_string20, menu_function20 }
};
		

void sound_menu_render(void)
{
	char buffer[16];
	int8_t menu_offset = 0;
	
	if (sound_cursor >= MENU_DISP_NUM) menu_offset = sound_cursor + 1 - MENU_DISP_NUM;
	
	lcd_clear();
	
	for (uint8_t menuPointer=0; menuPointer<MENU_DISP_NUM; menuPointer++)
	{
		dispSetCusor(0, menuPointer, 1);
		strcpy_P(buffer, (char*)pgm_read_word(&(sound_menu[menuPointer + menu_offset].text)));
		
		if (sound_cursor == menuPointer + menu_offset)	 lcd_write_string(buffer, 1, LCD_WHITE);
		else											 lcd_write_string(buffer, 1, LCD_BLACK);
	}
	
	lcd_render();
}

void sound_menu_key_update(void)
{
	static bool select_sw = true, start_sw = true, up_sw = true, down_sw = true;
	generic_func_ptr entryCall;
	
	if (!PCF8574_data.select && select_sw)
	{
		// NOP
	}
	
	if (!PCF8574_data.start && start_sw)
	{
		entryCall = (generic_func_ptr) pgm_read_word(&sound_menu[sound_cursor].fp);
		//LOG_INFO("Cursor: %i", sound_cursor);
		
		entryCall();
	}
	
	if (!PCF8574_data.up && up_sw)
	{
		sound_cursor++;
	}
	
	if (!PCF8574_data.down && down_sw)
	{
		sound_cursor--;
	}
	
	if (sound_cursor >= MENU_MAX)	sound_cursor = 1;
	
	if (sound_cursor < 1)					sound_cursor = MENU_MAX - 1;
	
	select_sw = PCF8574_data.select;
	start_sw = PCF8574_data.start;
	up_sw = PCF8574_data.up;
	down_sw = PCF8574_data.down;
}


void menu_function12(void)
{
	//LOG_INFO("menu_string01");
	playSound(indiaJones);
	
}
void menu_function13(void)
{
	playSound(tetris1);
}
void menu_function14(void)
{
	playSound(beethoven);
}
void menu_function15(void)
{
	playSound(monkey_theme);
}
void menu_function16(void)
{
	playSound(knightraider);
}
void menu_function17(void)
{
	playSound(mario);
}
void menu_function18(void)
{
	playSound(pacman1);
}
void menu_function19(void)
{
	playSound(beep);
}
void menu_function20(void)
{
	menu_scheduler_emty();
	menu_scheduler_restore();
}

