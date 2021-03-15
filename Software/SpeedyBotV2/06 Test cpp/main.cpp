/*
 * 06 Test cpp.cpp
 *
 * Created: 09.10.2020 20:57:05
 * Author : mschr
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "motor.h"

Motor Motors;

int main(void)
{
	Motors.speed(100, 100);
	Motors.direction(MOT_FWD, MOT_FWD);
	_delay_ms(1000);
	Motors.direction(MOT_BREAK, MOT_BREAK);
	
    /* Replace with your application code */
    while (1) 
    {
		
    }
}

