/*
 * 07 LCD Menu.c
 *
 * Created: 25.04.2020 15:47:36
 * Author : Michael
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "speedy.h"
#include "lcd_shield.h"
#include "menu.h"


/************************************************************************/
/* Task scheduler Belegung												*/
/* taskID[0] - task_System                                              */  
/* taskID[1] - task_switch (I2C-shadow register)						*/
/* taskID[2] - task_render (Display)                                    */
/* taskID[3] - task_keyHandler											*/
/* taskID[4] - task_Subprogramm											*/
/************************************************************************/

int main(void)
{
	systemInit();
	shieldInit();
		
	menu_scheduler_restore();
	
    while (1) 
    {
		schedulerDispatchTasks();
		
    }
}

