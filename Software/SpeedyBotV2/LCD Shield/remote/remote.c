/*
 * remote.c
 *
 * Created: 29.04.2020 19:42:41
 *  Author: mschr
 */ 

#include "remote.h"
#include "speedy.h"
#include "scheduler51.h"
#include "../lcd_shield.h"
#include "../menu.h"

#define DRIVE_SPEED 180
#define TURN_SPEED 80

//#define COMMAND_SEND_PREFIX			'*'
#define COMMAND_DIRECTION_FWD		'1'
#define COMMAND_DIRECTION_R			'2'
#define COMMAND_DIRECTION_REVERSE	'3'
#define COMMAND_DIRECTION_L			'4'
#define COMMAND_DIRECTION_STOP		'0'
//#define COMMAND_LED					'B'
//#define COMMAND_LED_OFF				'b'

static uint8_t command;
static bool firstRun = true;

#define RM_MENU_DIS_STATUS	0
#define RM_MENU_DIS_SENSORS	1
#define RM_MENU_DIS_BACK	2
#define RM_MENU_NUM 3
int8_t rm_menuPointer = 0;

void remoteExit(void);

void comandProzessor(void)
{
	
	uint16_t uart_receive = uart_getc();
		
	if( uart_receive == UART_NO_DATA )
	{
		command = 0;
		return;
	}
		
	command = (uint8_t) uart_receive;
		
		
	switch (command)
	{
		case COMMAND_DIRECTION_FWD:
		setStatusLED(GREEN);
		enableDevice(EN_ENCODERS);
		motor_speed(DRIVE_SPEED, DRIVE_SPEED);
		motorDirection(MOT_FWD, MOT_FWD);
		break;
			
		case COMMAND_DIRECTION_R:
		setStatusLED(YELLOW);
		disableDevice(EN_ENCODERS);
		motor_speed(TURN_SPEED, TURN_SPEED);
		motorDirection(MOT_FWD, MOT_REV);
		break;
			
		case COMMAND_DIRECTION_REVERSE:
		setStatusLED(RED);
		enableDevice(EN_ENCODERS);
		motor_speed(DRIVE_SPEED, DRIVE_SPEED);
		motorDirection(MOT_REV, MOT_REV);
		break;
			
		case COMMAND_DIRECTION_L:
		setStatusLED(YELLOW);
		disableDevice(EN_ENCODERS);
		motor_speed(TURN_SPEED, TURN_SPEED);
		motorDirection(MOT_REV, MOT_FWD);
		break;
			
		case COMMAND_DIRECTION_STOP:
		setStatusLED(OFF);
		disableDevice(EN_ENCODERS);
		motorDirection(MOT_BREAK, MOT_BREAK);
		break;
			
		default:
		disableDevice(EN_ENCODERS);
		setStatusLED(OFF);
		motorDirection(MOT_BREAK, MOT_BREAK);
		break;
	}
	

}

void disp_backMain(void)
{
	lcd_clear();
	
	dispSetCusor(0, 0, 1);
	lcd_write_string_P("*EXIT*", 2, LCD_WHITE); 
	
	lcd_render();
}

void dispRemote(void)
{
	if (rm_menuPointer == RM_MENU_DIS_STATUS) dispStatus();
	else if (rm_menuPointer == RM_MENU_DIS_SENSORS) dispSensors();
	else disp_backMain();
}

void rm_keyUpdate(void)
{
	static bool select_sw = true, start_sw = true, up_sw = true, down_sw = true;;
	
	readSwitch();
	
	if (!PCF8574_data.select && select_sw)
	{
		rm_menuPointer++;
		
		if (rm_menuPointer >= RM_MENU_NUM) rm_menuPointer = 0;
	}
	
	if (!PCF8574_data.start && start_sw)
	{
		if (rm_menuPointer == RM_MENU_DIS_BACK) remoteExit();
		else lineRead();
	}
	
	if (!PCF8574_data.up && up_sw)
	{
		
	}
	
	if (!PCF8574_data.down && down_sw)
	{
		
	}
	
	select_sw = PCF8574_data.select;
	start_sw = PCF8574_data.start;
	up_sw = PCF8574_data.up;
	down_sw = PCF8574_data.down;
}

void remoteInit(void)
{	
	taskID[2] = schedulerAddTask(dispRemote, 20, 100);
	taskID[3] = schedulerAddTask(rm_keyUpdate, 0, 50);
	
	rm_menuPointer = RM_MENU_DIS_STATUS;
	encoderReset();
}

void remoteExit(void)
{
	schedulerDeleteTask(taskID[2]);		// dispRemote
	schedulerDeleteTask(taskID[3]);		// rm_keyUpdate
	schedulerDeleteTask(taskID[4]);		// task_remote_program
	
	menu_scheduler_restore();
	
	firstRun = true;
}

void task_remote_program(void)
{
	if (firstRun)
	{
		remoteInit();
		firstRun = false;
	}
	
	comandProzessor();
	
}

