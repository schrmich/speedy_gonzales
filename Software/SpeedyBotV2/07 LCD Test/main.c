/*
 * 07 LCD Test.c
 *
 * Created: 24.01.2021 21:08:39
 * Author : mschr
 */ 

#include <avr/io.h>
#include "speedy.h"
#include "lcd_shield.h"
#include "menu.h"

int main(void)
{
	systemInit();
	shieldInit();
	
	taskID[0] = schedulerAddTask(taskSystem, 0, 10);
	taskID[2] = schedulerAddTask(menu_render, 20, 500);
	taskID[3] = schedulerAddTask(menu_key_update, 10, 100);
	
	LOG_INFO("LCD Menu Test");
	
    while (1) 
    {
		schedulerDispatchTasks();
    }
}

