/*
 * menu.c
 *
 * Created: 24.01.2021 21:13:16
 *  Author: mschr
 */ 

#include "line_menu.h"
#include "line_follower.h"
#include "..\menu.h"
#include "..\lcd_shield.h"
#include "log.h"
#include "scheduler51.h"
#include <avr/pgmspace.h>
#include <util/delay.h>

#define MENU_DISP_NUM	6	// Anzahl der Zeilen des Displays
#define MENU_MAX		4	// Anzahl der Menueinträge

// Startposition des Cursors 
// 1 wenn das Menu eine Überschrift hat, 0 wenn nein
int8_t line_cursor = 1;

// Generischer Funktionszeiger
typedef void ( *generic_func_ptr )( void );


typedef struct MENU {
	const char *text;
	generic_func_ptr fp;
} MENU_ENTRY_T;

const char PROGMEM menu_string31[] = "*PID Line Fol*";
const char PROGMEM menu_string32[] = ">Start";
const char PROGMEM menu_string33[] = ">Config Param.";
const char PROGMEM menu_string34[] = ">Back to Main";

void menu_function32(void);
void menu_function33(void);
void menu_function34(void);

const MENU_ENTRY_T line_menu[] PROGMEM = {
	{ menu_string31, NULL },
	{ menu_string32, menu_function32 },
	{ menu_string33, menu_function33 },
	{ menu_string34, menu_function34 }
};

const char PROGMEM menu_string131[] = "*PID Config*";
const char PROGMEM menu_string132[] = "SPEED:";
const char PROGMEM menu_string133[] = "KP	:";
const char PROGMEM menu_string134[] = "KI	:";
const char PROGMEM menu_string135[] = "KD	:";
const char PROGMEM menu_string136[] = ">Back to Main";

void menu_function132(void);
void menu_function133(void);
void menu_function134(void);
void menu_function135(void);
void menu_function136(void);

const MENU_ENTRY_T pid_menu[] PROGMEM = {
	{ menu_string131, NULL },
	{ menu_string132, menu_function132 },
	{ menu_string133, menu_function133 },
	{ menu_string134, menu_function134 },
	{ menu_string135, menu_function135 },
	{ menu_string136, menu_function136 }
};
		
void task_pid_line_menu_render(void)
{
	char buffer[16];
	int8_t menu_offset = 0;
	
	
	if (line_cursor >= MENU_DISP_NUM) menu_offset = line_cursor + 1 - MENU_DISP_NUM;
	
	lcd_clear();
	
	for (uint8_t menuPointer=0; menuPointer<MENU_MAX; menuPointer++)
	{
		dispSetCusor(0, menuPointer, 1);
		strcpy_P(buffer, (char*)pgm_read_word(&(line_menu[menuPointer + menu_offset].text)));
		
		if (line_cursor == menuPointer + menu_offset)	 lcd_write_string(buffer, 1, LCD_WHITE);
		else											 lcd_write_string(buffer, 1, LCD_BLACK);
	}
	
	lcd_render();
	
}

void task_pid_line_keyHandler(void)
{
	static bool select_sw = true, start_sw = true, up_sw = true, down_sw = true;
	generic_func_ptr entryCall;
	
	if (!PCF8574_data.select && select_sw)
	{
		// NOP
	}
	
	if (!PCF8574_data.start && start_sw)
	{
		entryCall = (generic_func_ptr) pgm_read_word(&line_menu[line_cursor].fp);
		
		entryCall();
	}
	
	if (!PCF8574_data.up && up_sw)
	{
		line_cursor++;
	}
	
	if (!PCF8574_data.down && down_sw)
	{
		line_cursor--;
	}
	
	if (line_cursor >= MENU_MAX)	line_cursor = 1;
	
	if (line_cursor < 1)	line_cursor = MENU_MAX - 1;
	
	select_sw = PCF8574_data.select;
	start_sw = PCF8574_data.start;
	up_sw = PCF8574_data.up;
	down_sw = PCF8574_data.down;
	
}

// LCD Render Submenu PID Config
void task_pid_submenu_render(void);

void menu_function32(void)
{	
	menu_scheduler_emty();
	taskID[4] = schedulerAddTask(task_pid_line_follow_main, 10,20);
	lcd_clear();
	lcd_render();
	
	_delay_ms(500);
}

void menu_function33(void)
{
	schedulerDeleteTask(taskID[2]);
	schedulerDeleteTask(taskID[3]);
	taskID[2] = schedulerAddTask(task_pid_submenu_render, 5, 400);
	//taskID[3] = schedulerAddTask(menu_keyHandler_133, 5, 200);
}

void menu_function34(void)
{
	menu_scheduler_emty();
	schedulerDeleteTask(taskID[4]);
	menu_scheduler_restore();
}


void menu_function132(void)
{
	lcd_clear();
	
	char buffer[20];
	
	
	dispSetCusor(0, 0, 1);
	lcd_write_string_P("* LINE PID *", 1, LCD_WHITE);
	
	dispSetCusor(0, 1, 1);
	sprintf_P(buffer, PSTR("SPEED: %d"), baseSpeed);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(0, 2, 1);
	sprintf_P(buffer, PSTR("K_P: %2.2f"), k_p);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(0, 3, 1);
	sprintf_P(buffer, PSTR("K_I: %2.2f"), k_i);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(0, 4, 1);
	sprintf_P(buffer, PSTR("K_D: %2.2f"), k_d);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	lcd_render();
}

// Key Handler Submenu PID Config
void menu_function133(void)
{
	
}

// LCD Render Submenu PID Config
void task_pid_submenu_render(void)
{
	char buffer[16];
	int8_t menu_offset = 0;
	
	if (line_cursor >= MENU_DISP_NUM) menu_offset = line_cursor + 1 - MENU_DISP_NUM;
	
	lcd_clear();
	
	for (uint8_t menuPointer=0; menuPointer<MENU_MAX; menuPointer++)
	{
		dispSetCusor(0, menuPointer, 1);
		strcpy_P(buffer, (char*)pgm_read_word(&(pid_menu[menuPointer + menu_offset].text)));
		
		if (line_cursor == menuPointer + menu_offset)	 lcd_write_string(buffer, 1, LCD_WHITE);
		else											 lcd_write_string(buffer, 1, LCD_BLACK);
	}
	
	// Dislpay Speed value
	dispSetCusor(5, 0, 1);
	itoa(baseSpeed, buffer, 10);
	if (line_cursor == menuPointer + menu_offset)	 lcd_write_string(buffer, 1, LCD_WHITE);
	else											 lcd_write_string(buffer, 1, LCD_BLACK);
	
	// Display PID Values
	dispSetCusor(5, 1, 1);
	dtostrf(k_p, 4, 2,buffer);
	if (line_cursor == menuPointer + menu_offset)	lcd_write_string(buffer, 1, LCD_WHITE);
	else											lcd_write_string(buffer, 1, LCD_BLACK);
	dispSetCusor(5, 2, 1);
	dtostrf(k_i, 4, 2,buffer);
	if (line_cursor == menuPointer + menu_offset)	lcd_write_string(buffer, 1, LCD_WHITE);
	else											lcd_write_string(buffer, 1, LCD_BLACK);
	dispSetCusor(5, 3, 1);
	dtostrf(k_d, 4, 2,buffer);
	if (line_cursor == menuPointer + menu_offset)	lcd_write_string(buffer, 1, LCD_WHITE);
	else											lcd_write_string(buffer, 1, LCD_BLACK);
	
	lcd_render();
	
}