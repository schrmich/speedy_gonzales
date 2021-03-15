/*
 * _01_System.c
 *
 * Created: 18.06.2019 08:39:04
 *  Author: schreiberm
 */ 


#include "speedy.h"
#include "testfunctions.h"
#include <util/delay.h>

uint8_t menu = 4;

int main(void)
{
	systemInit();
	
	enableDevice(EN_BLUETOOTH);
	
	//taskID_1 = schedulerAddTask(testDistance, 0, 100);
	//taskID_2 = schedulerAddTask(parserDistance, 0, 0);
	//taskID_1 = schedulerAddTask(taskSystem, 0, 10);
	
	
    while(1)
    {
		if (startButton())
		{
			setStatusLED(YELLOW);
			if (menu == 0)
			 playSound(pacman1);
			else if (menu == 1)
				playSound(beethoven);
			else if( menu == 2)
				playSound(tetris1);
			else if(menu == 3)
				playSound(indiaJones);
			else if(menu == 4)
			 playSound(mario);
			setStatusLED(GREEN);
		}
		
		if (userButton())
		{
			menu++;
			if (menu >= 5) menu = 0;
		}
		_delay_ms(150);
		
		//schedulerDispatchTasks();
    }
}
