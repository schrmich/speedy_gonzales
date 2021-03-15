/*
 * menu.c
 *
 * Created: 29.04.2020 15:04:36
 *  Author: mschr
 */ 

#include "menu.h"
#include "speedy.h"
#include "lcd_shield.h"
#include "remote/remote.h"
#include "line_follower/line_follower.h"
#include "maze_solver_I/fsm.h"

#define MENU_DISP_NUM 5

typedef void (*MenuFnct) (void);

typedef struct MENU
{
	const char* text;
	MenuFnct Function;
}menu_t;


typedef enum {	MAIN_MENU_STATE,	// 0
				REMOTE_MENU_STATE,	// 1
				SOUND_MENU_STATE,	// 2
				LINE_FOL_STATE,		// 3
				MAZE_SOLV_1_STATE,	// 4
				MAZE_SOLV_2_STATE,	// 5
				MAZE_CONFIG_STATE,	// 6
				MAIN_MENU_STATE_NUM }menuState_t;

menuState_t menuState = MAIN_MENU_STATE;

int8_t menuPointer = 1;

uint8_t menuMax;


/************************************************************************/
/* Main Menu                                                            */
/************************************************************************/
	
enum main_menu_en {	TITLE_MENU=0,		// 0
					REMOTE_MENU,		// 1
					SOUND_TEST_MENU,	// 2
					LINE_FOL_MENU,		// 3
					MAZE_SOLV_1_MENU,	// 4
					MAZE_SOLV_2_MENU,	// 5
					CONFIG_MENU,		// 6
					MAIN_MENU_NUM};		// 7

const char _00title_str[]	PROGMEM = "*SpeedyBotV2*";
const char _01remote_str[]	PROGMEM = ">Remote Cont.";
const char _02sound_str[]	PROGMEM = ">Sound Test";
const char _03line_str[]	PROGMEM = ">Line Follwer";
const char _04maze1_str[]	PROGMEM = ">Maze Solv. I";
const char _05maze2_str[]	PROGMEM = ">Maze Solv.II";
const char _06config_str[]	PROGMEM = ">Config Sys.";

void _00title(void)
{
	//menuPointer = 1;
}

void _01remote(void)
{
	taskID[4] = schedulerAddTask(remote_program, 0, 100);
	
	//while(uart_getc() != UART_NO_DATA);	// Flush UART Buffer
	//menuState = REMOTE_MENU_STATE;
	//encoderReset();
	menuPointer = REMOTE_MENU_STATE;
}

void _02sound_test(void)
{
	menuState = SOUND_MENU_STATE;
	menuPointer = 0;
}

void _03line_follower(void)
{
	menuState = LINE_FOL_STATE;
	menuPointer = 1;
}

void _04maze_solver_1(void)
{
	menuState = MAZE_SOLV_1_STATE;
	menuPointer = 1;
}

void _05maze_solver_2(void)
{
	
}

void _06configuration(void)
{
	
}

const menu_t mainMenu[MAIN_MENU_NUM] PROGMEM =
{
	{ _00title_str,		_00title			},
	{ _01remote_str,	_01remote			},
	{ _02sound_str,		_02sound_test		},
	{ _03line_str,		_03line_follower	},
	{ _04maze1_str,		_04maze_solver_1	},
	{ _05maze2_str,		_05maze_solver_2	},
	{ _06config_str,	_06configuration	}
};

// End Main Menu

/************************************************************************/
/* Remote Control	                                                    */
/************************************************************************/

enum remote_menu_en{ REMOTE_MENU_BACK=0, REMOTE_MENU_NUM };

const char _10back_str[]	PROGMEM = ">Back to Main";

void _10backMain(void)
{
	menuState = MAIN_MENU_STATE;
	
	menuPointer = 1;
}

const menu_t remoteMenu[REMOTE_MENU_NUM] PROGMEM =
{
	{ _10back_str,		_10backMain		}
};


// End Reote Control

/************************************************************************/
/* Sound Menu                                                           */
/************************************************************************/

enum sound_menu_en{	SOUND_BACK_MEU = 0, 
					INDIANA_MENU, 
					TETRIS_MENU, 
					BEETHOVEN_MENU, 
					MONKEY_MENU, 
					KNIGHT_MENU, 
					MARIO_MENU, 
					BEEP_MENU, 
					PACMAN_MENU, 
					SOUND_MENU_NUM};
	
const char _20back_str[]		PROGMEM = ">Back to Main";
const char _21indiana_str[]		PROGMEM = ">Indiana J.";
const char _22tetris_str[]		PROGMEM = ">Tetris";
const char _23beethoven_str[]	PROGMEM = ">Beethoven";
const char _24monkey_str[]		PROGMEM = ">Monkey Isl.";
const char _25knight_str[]		PROGMEM = ">Knight R.";
const char _26mario_str[]		PROGMEM = ">Mario Brot.";
const char _27beep_str[]		PROGMEM = ">Beep Sound";
const char _28pacman_str[]		PROGMEM = ">Pacman";

void _20backMain(void)
{
	menuState = MAIN_MENU_STATE;
	menuPointer = 1;
}

void _21sound_indiana(void)
{
	playSound(indiaJones);
}

void _22sound_tetris(void)
{
	playSound(tetris1);
}

void _23sound_beethoven(void)
{
	playSound(beethoven);
}

void _24sound_monkey(void)
{
	playSound(monkey_theme);
}

void _25sound_knight(void)
{
	playSound(knightraider);
}

void _26mario_brothers(void)
{
	playSound(mario);
}

void _27beep(void)
{
	playSound(beep);
}

void _28sound_pacman(void)
{
	playSound(pacman1);
}

const menu_t soundMenu[SOUND_MENU_NUM] PROGMEM =
{
	{ _20back_str,			_20backMain			},
	{ _21indiana_str,		_21sound_indiana	},
	{ _22tetris_str,		_22sound_tetris		},
	{ _23beethoven_str,		_23sound_beethoven	},
	{ _24monkey_str,		_24sound_monkey		},
	{ _25knight_str,		_25sound_knight		},
	{ _26mario_str,			_26mario_brothers	},
	{ _27beep_str,			_27beep				},
	{ _28pacman_str,		_28sound_pacman		}
};

// End Sound Menu

/************************************************************************/
/* Line Solver Menu                                                   */
/************************************************************************/

enum line_menu_en{	LINE_MENU_TITLE=0,
					LINE_MENU_START,
					LINE_MENU_CONFIG,
					LINE_MENU_BACK,
					LINE_MENU_NUM };

const char _30title_str[]		PROGMEM = "LINE SOLVER";
const char _31start_str[]		PROGMEM = ">Start";
const char _32config_str[]		PROGMEM = ">Config Par.";
const char _33back_str[]		PROGMEM = ">Back Main";

void _30title_fkt(void)
{
	
}

void _31start_fkt(void)
{
	
	schedulerDeleteTask(taskID[2]);		// task LCD Menu
	schedulerDeleteTask(taskID[3]);		// task keyUpdate
	taskID[4] = schedulerAddTask(pid_line_follow_main, 0, 20);
}

void _32config_fkt(void)
{
	schedulerDeleteTask(taskID[2]);		// task LCD Menu
	schedulerDeleteTask(taskID[3]);		// task keyUpdate
	taskID[4] = schedulerAddTask(pid_line_config, 0, 20);
}

void _33back_fkt(void)
{
	menuState = MAIN_MENU_STATE;
	schedulerDeleteTask(taskID[4]);
	menuPointer = 1;
}

const menu_t line_menu[LINE_MENU_NUM] PROGMEM =
{
	{ _30title_str,		_30title_fkt	},
	{ _31start_str,		_31start_fkt	},
	{ _32config_str,	_32config_fkt	},
	{ _33back_str,		_33back_fkt		}
};

// End Maze Solver I Menu

/************************************************************************/
/* Maze Solver I Menu                                                   */
/************************************************************************/

enum maze_1_menu_en{	MAZE_1_MENU_TITLE=0, 
						MAZE_1_MENU_START,
						MAZE_1_MENU_CONFIG, 
						MAZE_1_MENU_BACK, 
						MAZE_1_MENU_NUM };

const char _40title_str[]		PROGMEM = "MAZE_SOLVER I";
const char _41start_str[]		PROGMEM = ">Start Maze";
const char _42config_str[]		PROGMEM = ">Config Par.";
const char _43back_str[]		PROGMEM = ">Back Main";

void _40title_fkt(void)
{
	
}

void _41start_fkt(void)
{
	static bool firstRun = true;
	
	if (firstRun)
	{
		firstRun = false;
		taskID[4] = schedulerAddTask(task_maze_I, 0, 10);
		schedulerDeleteTask(taskID[2]);		// task LCD Menu
		schedulerDeleteTask(taskID[3]);		// task keyUpdate
	}
	else
	{
		firstRun = true;
		schedulerDeleteTask(taskID[4]);
		taskID[2] = schedulerAddTask(task_lcdMainMenu, 10, 100);
		taskID[3] = schedulerAddTask(keyUpdate, 0, 50);
	}
}

void _42config_fkt(void)
{
	
}

void _43back_fkt(void)
{
	menuState = MAIN_MENU_STATE;
	menuPointer = 1;
}

const menu_t maze_1_menu[MAZE_1_MENU_NUM] PROGMEM =
{
	{ _40title_str,		_40title_fkt	},
	{ _41start_str,		_41start_fkt	},
	{ _42config_str,	_42config_fkt	},
	{ _43back_str,		_43back_fkt		}
};

// End Maze Solver I Menu 

/************************************************************************/
/* Maze Solver II Menu                                                   */
/************************************************************************/

enum maze_2_menu_en{	MAZE_2_MENU_TITLE=0,
						MAZE_2_MENU_START,
						MAZE_2_MENU_CONFIG,
						MAZE_2_MENU_BACK,
						MAZE_2_MENU_NUM };

const char _50title_str[]		PROGMEM = "MAZE_SOLVER II";
const char _51start_str[]		PROGMEM = ">Start Maze";
const char _52config_str[]		PROGMEM = ">Config Par.";
const char _53back_str[]		PROGMEM = ">Back Main";

void _50title_fkt(void)
{
	
}

void _51start_fkt(void)
{
	
}

void _52config_fkt(void)
{
	
}

void _53back_fkt(void)
{
	menuState = MAIN_MENU_STATE;
	menuPointer = 1;
}

const menu_t maze_2_menu[MAZE_2_MENU_NUM] PROGMEM =
{
	{ _50title_str,		_50title_fkt	},
	{ _51start_str,		_51start_fkt	},
	{ _52config_str,	_52config_fkt	},
	{ _53back_str,		_53back_fkt		}
};

// End Maze Solver II Menu


/************************************************************************/
/* MENU Config			                                                */
/************************************************************************/

enum config_menu_en{	CONFIG_MENU_BACK=0,
	CONFIG_MENU_LINE_SENSORS,
CONFIG_MENU_NUM };

const char _60back_str[]	PROGMEM = ">Back to Main";
const char _61line_sensors_str[]	PROGMEM =  ">Line Sensors";

void _60backMain(void)
{
	menuState = MAIN_MENU_STATE;
	
	menuPointer = 1;
}

void _61config_line_sensors(void)
{
	
}

const menu_t config_menu[CONFIG_MENU_NUM] PROGMEM =
{
	{ _60back_str,			_60backMain	},
	{ _61line_sensors_str,	_61config_line_sensors	}
};

/************************************************************************/
/*    End MENU Config                                                   */
/************************************************************************/

void task_lcdMainMenu(void)
{
	
	bool _color = LCD_BLACK;
		
	int8_t cursor_offset = 0;
		
	if(menuPointer > MENU_DISP_NUM) cursor_offset = menuPointer - MENU_DISP_NUM;
		
	int8_t pointerMax = MENU_DISP_NUM + cursor_offset +1;
		
	lcd_clear();
	
	switch(menuState)
	{
		case MAIN_MENU_STATE:
			if(menuPointer == 0) menuPointer = 1;
			
			for (int8_t i=cursor_offset; i<pointerMax; i++)
			{
				if (i >= MAIN_MENU_NUM) break;
				
				if (i == menuPointer) _color = LCD_WHITE;
				else _color = LCD_BLACK;
				
				dispSetCusor(0, i-cursor_offset, 1);
				
				char buffer[20];
				
				strcpy_P(buffer, (char*)pgm_read_word(&(mainMenu[i].text)));
				
				lcd_write_string(buffer, 1, _color);
			}
		break;
		
		case REMOTE_MENU_STATE:
			// NOP()
		break;
		
		case SOUND_MENU_STATE:
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
		
		case LINE_FOL_STATE:
		if(menuPointer == 0) menuPointer = 1;
		
		for (int8_t i=cursor_offset; i<pointerMax; i++)
		{
			if (i >= LINE_MENU_NUM) break;
			
			if (i == menuPointer) _color = LCD_WHITE;
			else _color = LCD_BLACK;
			
			dispSetCusor(0, i-cursor_offset, 1);
			
			char buffer[20];
			
			strcpy_P(buffer, (char*)pgm_read_word(&(line_menu[i].text)));
			
			lcd_write_string(buffer, 1, _color);
		}
		break;
		
		case MAZE_SOLV_1_STATE:
			if(menuPointer == 0) menuPointer = 1;
			
			for (int8_t i=cursor_offset; i<pointerMax; i++)
			{
				if (i >= MAZE_1_MENU_NUM) break;
				
				if (i == menuPointer) _color = LCD_WHITE;
				else _color = LCD_BLACK;
				
				dispSetCusor(0, i-cursor_offset, 1);
				
				char buffer[20];
				
				strcpy_P(buffer, (char*)pgm_read_word(&(maze_1_menu[i].text)));
				
				lcd_write_string(buffer, 1, _color);
			}
		break;
		
		case MAZE_SOLV_2_STATE:
		if(menuPointer == 0) menuPointer = 1;
		
		for (int8_t i=cursor_offset; i<pointerMax; i++)
		{
			if (i >= MAZE_2_MENU_NUM) break;
			
			if (i == menuPointer) _color = LCD_WHITE;
			else _color = LCD_BLACK;
			
			dispSetCusor(0, i-cursor_offset, 1);
			
			char buffer[20];
			
			strcpy_P(buffer, (char*)pgm_read_word(&(maze_2_menu[i].text)));
			
			lcd_write_string(buffer, 1, _color);
		}
		break;
		
		case MAZE_CONFIG_STATE:
		if(menuPointer == 0) menuPointer = 1;
		
		for (int8_t i=cursor_offset; i<pointerMax; i++)
		{
			if (i >= MAZE_1_MENU_NUM) break;
			
			if (i == menuPointer) _color = LCD_WHITE;
			else _color = LCD_BLACK;
			
			dispSetCusor(0, i-cursor_offset, 1);
			
			char buffer[20];
			
			strcpy_P(buffer, (char*)pgm_read_word(&(maze_1_menu[i].text)));
			
			lcd_write_string(buffer, 1, _color);
		}
		break;
	}
		
	lcd_render();
	
}

void keyUpdate(void)
{
	static bool select_sw = true, start_sw = true, up_sw = true, down_sw = true;;
	
	MenuFnct _startProgram;
	
	readSwitch();
	
	switch(menuState)
	{
		case MAIN_MENU_STATE:
			menuMax = MAIN_MENU_NUM;
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
