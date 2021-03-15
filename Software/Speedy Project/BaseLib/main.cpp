/*
 * BaseLib.cpp
 *
 * Created: 11.10.2020 19:06:20
 * Author : mschr
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "speedy.h"


int main(void)
{
	scheduler.taskID[0] = scheduler.addTask(baseSystem.taskSystem, 0, 1000);
	scheduler.taskID[1] = scheduler.addTask(motor.taskDrive, 0, 10);
	
	motor.direction(MOT_FWD, MOT_REV);
	motor.speed(80, 80);
	_delay_ms(1000);
	motor.direction(MOT_BREAK, MOT_BREAK);
	
    /* Replace with your application code */
    while (1) 
    {
    }
}

