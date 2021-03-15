/*
 * menu.c
 *
 * Created: 24.01.2021 21:13:16
 *  Author: mschr
 */ 

#include "menu.h"
#include "lcd_shield.h"
#include "log.h"
#include "scheduler51.h"
#include "remote/remote.h"
#include "sound_test/sound_menu.h"
#include "line_follower/line_menu.h"
#include <avr/pgmspace.h>
#include <util/delay.h>

#define MENU_DISP_NUM	6	// Anzahl der Zeilen des Displays
#define MENU_MAX		8	// Anzahl der Menueinträge

// Startposition des Cursors 
// 1 wenn das Menu eine Überschrift hat, 0 wenn nein
int8_t cursor = 1;

// Generischer Funktionszeiger
typedef void ( *generic_func_ptr )( void );


typedef struct MENU {
	const char *text;
	generic_func_ptr fp;
} MENU_ENTRY_T;

const char PROGMEM menu_string01[] = "*SpeedyBotV2*";
const char PROGMEM menu_string02[] = ">Remote Cont.";
const char PROGMEM menu_string03[] = ">Sound Test";
const char PROGMEM menu_string04[] = ">Line Follwer";
const char PROGMEM menu_string05[] = ">Maze Solv. I";
const char PROGMEM menu_string06[] = ">Maze Solv.II";
const char PROGMEM menu_string07[] = ">Config Sys.";
const char PROGMEM menu_string08[] = ">Test Program";

void menu_function02(void);
void menu_function03(void);
void menu_function04(void);
void menu_function05(void);
void menu_function06(void);
void menu_function07(void);
void menu_function08(void);

const MENU_ENTRY_T menu[] PROGMEM = {
	{ menu_string01, NULL },
	{ menu_string02, menu_function02 },
	{ menu_string03, menu_function03 },
	{ menu_string04, menu_function04 },
	{ menu_string05, menu_function05 },
	{ menu_string06, menu_function06 },
	{ menu_string07, menu_function07 },
	{ menu_string08, menu_function08 }
};
		

void main_menu_render(void)
{
	char buffer[16];
	int8_t menu_offset = 0;
	
	if (cursor >= MENU_DISP_NUM) menu_offset = cursor + 1 - MENU_DISP_NUM;
	
	lcd_clear();
	
	for (uint8_t menuPointer=0; menuPointer<MENU_DISP_NUM; menuPointer++)
	{
		dispSetCusor(0, menuPointer, 1);
		strcpy_P(buffer, (char*)pgm_read_word(&(menu[menuPointer + menu_offset].text)));
		
		if (cursor == menuPointer + menu_offset) lcd_write_string(buffer, 1, LCD_WHITE);
		else									 lcd_write_string(buffer, 1, LCD_BLACK);
	}
	
	lcd_render();
}

void main_menu_key_update(void)
{
	static bool select_sw = true, start_sw = true, up_sw = true, down_sw = true;
	generic_func_ptr entryCall;
	
	if (!PCF8574_data.select && select_sw)
	{
		// NOP
	}
	
	if (!PCF8574_data.start && start_sw)
	{
		entryCall = (generic_func_ptr) pgm_read_word(&menu[cursor].fp);
		
		entryCall();
	}
	
	if (!PCF8574_data.up && up_sw)
	{
		cursor++;
	}
	
	if (!PCF8574_data.down && down_sw)
	{
		cursor--;
	}
	
	if (cursor >= MENU_MAX)	cursor = 1;
	
	if (cursor < 1)					cursor = MENU_MAX - 1;
	
	select_sw = PCF8574_data.select;
	start_sw = PCF8574_data.start;
	up_sw = PCF8574_data.up;
	down_sw = PCF8574_data.down;
}


void menu_function02(void)
{	
	menu_scheduler_emty();
	taskID[4] = schedulerAddTask(task_remote_program, 0, 100);
}

void menu_function03(void)
{
	menu_scheduler_emty();
	_delay_ms(500);
	taskID[2] = schedulerAddTask(sound_menu_render, 0, 400);
	taskID[3] = schedulerAddTask(sound_menu_key_update, 0, 100);
}

void menu_function04(void)
{
	menu_scheduler_emty();
	_delay_ms(500);
	taskID[2] = schedulerAddTask(task_pid_line_menu_render, 0, 400);
	taskID[3] = schedulerAddTask(task_pid_line_keyHandler, 0, 100);
	//taskID[4] = schedulerAddTask(task_pid_line_follow_main, 10,20);
}

void menu_function05(void)
{
	
}

void menu_function06(void)
{
	
}

void menu_function07(void)
{
	
}

void menu_function08(void)
{
	
}

void menu_scheduler_restore(void)
{
	taskID[2] = schedulerAddTask(main_menu_render, 20, 500);
	taskID[3] = schedulerAddTask(main_menu_key_update, 10, 100);
}

void menu_scheduler_emty(void)
{
	schedulerDeleteTask(taskID[2]);
	schedulerDeleteTask(taskID[3]);
}