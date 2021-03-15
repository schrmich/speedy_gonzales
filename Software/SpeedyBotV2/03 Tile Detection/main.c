/*
 * Tile Detection
 *
 * Created: 14.12.2017 16:35:18
 *  Author: mschr
 */ 

#include "system.h"
#include "fsm.h"
#include "maze.h"
#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>





int main(void)
{
	systemInit();

	while (1)
	{
		taskSystem();
		
		
		fsm_task();
		
	}
}