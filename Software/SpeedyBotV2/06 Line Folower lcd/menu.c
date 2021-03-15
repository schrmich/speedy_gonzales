/*
 * menu.c
 *
 * Created: 29.04.2020 15:04:36
 *  Author: mschr
 */ 

#include "speedy.h"
#include "lcd_shield.h"

#define MENU_DISP_NUM 5

typedef void (*MenuFnct) (void);

typedef struct MENU
{
	const char* text;
	MenuFnct Function;
}menu_t;


typedef enum {	STATE_MAIN_MENU,		// 0
				STATE_RUN,				// 1
				STATE_CONFIG_PID_MENU,	// 2
				STATE_MAIN_MENU_NUM }menuState_t;

menuState_t menuState = STATE_MAIN_MENU;

int8_t menuPointer = 1;

uint8_t menuMax;


/************************************************************************/
/* Main Menu                                                            */
/************************************************************************/
	
enum main_menu_en {	MENU_TITLE=0,		// 0
					MENU_RUN,			// 1
					MENU_CONFIG,		// 2
					MENU_MAIN_NUM};		// 3

const char _00title_str[]	PROGMEM = "PID Line Fol.";
const char _01run_str[]	PROGMEM = ">Run Prog.";
const char _02config_str[]	PROGMEM = ">Config Param.";


void _00title(void)
{
	//menuPointer = 1;
}

void _01run(void)
{
	//taskID[4] = schedulerAddTask(remote_program, 0, 100);
	
	//while(uart_getc() != UART_NO_DATA);	// Flush UART Buffer
	//menuState = REMOTE_MENU_STATE;
	//encoderReset();
	//menuPointer = REMOTE_MENU_STATE;
}

void _02config(void)
{
	menuState = SOUND_MENU_STATE;
	menuPointer = 0;
}

void task_lcdMainMenu(void)
{
	
	bool _color = LCD_BLACK;
		
	int8_t cursor_offset = 0;
		
	if(menuPointer > MENU_DISP_NUM) cursor_offset = menuPointer - MENU_DISP_NUM;
		
	int8_t pointerMax = MENU_DISP_NUM + cursor_offset +1;
		
	lcd_clear();
	
	switch(menuState)
	{
		case STATE_MAIN_MENU:
			if(menuPointer == 0) menuPointer = 1;
			
			for (int8_t i=cursor_offset; i<pointerMax; i++)
			{
				if (i >= STATE_MAIN_MENU_NUM) break;
				
				if (i == menuPointer) _color = LCD_WHITE;
				else _color = LCD_BLACK;
				
				dispSetCusor(0, i-cursor_offset, 1);
				
				char buffer[20];
				
				strcpy_P(buffer, (char*)pgm_read_word(&(mainMenu[i].text)));
				
				lcd_write_string(buffer, 1, _color);
			}
		break;
		
		case STATE_RUN:
			// NOP()
		break;
		
		case STATE_CONFIG_PID_MENU:
			for (int8_t i=cursor_offset; i<pointerMax; i++)
			{
				if (i >= SOUND_MENU_NUM) break;
				
				if (i == menuPointer) _color = LCD_WHITE;
				else _color = LCD_BLACK;
				
				dispSetCusor(0, i-cursor_offset, 1);
				
				char buffer[20];
				
				strcpy_P(buffer, (char*)pgm_read_word(&(soundMenu[i].text)));
				
				lcd_write_string(buffer, 1, _color);
			}
		break;
		
	}
		
	lcd_render();
	
}

void task_keyUpdate(void)
{
	static bool select_sw = true, start_sw = true, up_sw = true, down_sw = true;;
	
	MenuFnct _startProgram;
	
	readSwitch();
	
	switch(menuState)
	{
		case STATE_MAIN_MENU:
			menuMax = MENU_MAIN_NUM;
			_startProgram = (MenuFnct)pgm_read_word(&(mainMenu[menuPointer].Function));
		break;
		
		case REMOTE_MENU_STATE:
			menuMax = REMOTE_MENU_NUM;
			_startProgram = (MenuFnct)pgm_read_word(&(remoteMenu[menuPointer].Function));
		break;
		
		case SOUND_MENU_STATE:
			menuMax = SOUND_MENU_NUM;
			_startProgram = (MenuFnct)pgm_read_word(&(soundMenu[menuPointer].Function));
		break;
		
		case LINE_FOL_STATE:
		menuMax = LINE_MENU_NUM;
		_startProgram = (MenuFnct)pgm_read_word(&(line_menu[menuPointer].Function));
		break;
		
		case MAZE_SOLV_1_STATE:
			menuMax = MAZE_1_MENU_NUM;
			_startProgram = (MenuFnct)pgm_read_word(&(maze_1_menu[menuPointer].Function));
		break;
		
		case MAZE_SOLV_2_STATE:
			menuMax = MAZE_2_MENU_NUM;
			_startProgram = (MenuFnct)pgm_read_word(&(maze_2_menu[menuPointer].Function));
		break;
		
		case MAZE_CONFIG_STATE:
			menuMax = CONFIG_MENU_NUM;
			_startProgram = (MenuFnct)pgm_read_word(&(config_menu[menuPointer].Function));
		break;
		
		default:
			menuMax = MAIN_MENU_NUM;
			_startProgram = (MenuFnct)pgm_read_word(&(mainMenu[menuPointer].Function));
		break;
	}
	
	if (!PCF8574_data.select && select_sw)
	{
		menuPointer++;
		if(menuPointer >= menuMax) menuPointer = 0;
	}
	
	if (!PCF8574_data.start && start_sw)
	{
		_startProgram();
		
	}
	
	if (!PCF8574_data.up && up_sw)
	{
		writeLeds(LED_BLUE);
	}
	
	if (!PCF8574_data.down && down_sw)
	{
		writeLeds(RED);
	}
	
	select_sw = PCF8574_data.select;
	start_sw = PCF8574_data.start;
	up_sw = PCF8574_data.up;
	down_sw = PCF8574_data.down;
	
}
