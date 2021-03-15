/*
 * Tile Detection
 *
 * Created: 14.12.2017 16:35:18
 *  Author: mschr
 */ 

#include "main.h"
#include "speedy.h"
#include "map.h"
#include "maze.h"
#include "simulate.h"
#include "fsm.h"
#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>


int main(void)
{
	systemInit();
	 
	#ifdef SIMULATOR
	taskID[TASK_SIMULATOR] = schedulerAddTask(simulator, 0, 500);
	simuInit();
	myprintf_P("Battery:%i \n", batt_voltage());
	myprintf_P("Press Start.\n");
	maze_init();
	mazePrint();
	while(!startButton());
	
	#else
	taskID[TASK_STATE_MACHINE] = schedulerAddTask(fsm_task, 10, 10);
	taskID[TASK_SYSTEM] = schedulerAddTask(taskSystem, 0, 10);
	#endif	/* SIMULATOR */
	
	
	while (1)
	{
		//schedulerUpdate();
		schedulerDispatchTasks();
	}
	
	return 0;
}