/*
 * Tile Detection
 *
 * Created: 14.12.2017 16:35:18
 *  Author: mschr
 */ 

#include "system.h"
#include "fsm.h"
#include "maze.h"
#include "scheduler51.h"
#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>

enum {TASK_STATE_MACHINE, TASK_SYSTEM, TASK_NUMBER};
	
uint8_t taskID[TASK_NUMBER];

int main(void)
{
	systemInit();
	taskID[TASK_STATE_MACHINE] = schedulerAddTask(fsm_task, 10, 10);
	taskID[TASK_SYSTEM] = schedulerAddTask(taskSystem, 0, 5); 
	
	while (1)
	{
		schedulerDispatchTasks();
	}
	
	return 0;
}