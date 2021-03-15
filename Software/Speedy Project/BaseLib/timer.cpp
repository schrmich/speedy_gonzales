/*
 * timer.c
 *
 * Created: 10.10.2019 21:16:48
 *  Author: mschr
 */ 

#include "timer.h"
#include <util/atomic.h>

volatile stopwatches_t stopwatches;

void setStopwatch(uint16_t watch, uint16_t timer)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		switch(watch)
		{
			case STOPWATCH1:
			stopwatches.watch1 = timer;
			break;
			case STOPWATCH2:
			stopwatches.watch2 = timer;
			break;
			case STOPWATCH3:
			stopwatches.watch3 = timer;
			break;
			case STOPWATCH4:
			stopwatches.watch4 = timer;
			break;
			case STOPWATCH5:
			stopwatches.watch5 = timer;
			break;
			case STOPWATCH6:
			stopwatches.watch6 = timer;
			break;
			case STOPWATCH7:
			stopwatches.watch7 = timer;
			break;
			case STOPWATCH8:
			stopwatches.watch8 = timer;
			break;
		}
	}
}

uint16_t getStopwatch(uint8_t watch)
{
	uint16_t timer = 0;
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		switch(watch)
		{
			case STOPWATCH1:
			timer = stopwatches.watch1;
			break;
			case STOPWATCH2:
			timer = stopwatches.watch2;
			break;
			case STOPWATCH3:
			timer = stopwatches.watch3;
			break;
			case STOPWATCH4:
			timer = stopwatches.watch4;
			break;
			case STOPWATCH5:
			timer = stopwatches.watch5;
			break;
			case STOPWATCH6:
			timer = stopwatches.watch6;
			break;
			case STOPWATCH7:
			timer = stopwatches.watch7;
			break;
			case STOPWATCH8:
			timer = stopwatches.watch8;
			break;
		}
	}
	return timer;
}