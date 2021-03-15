/*
 * simulate.c
 *
 * Created: 25.02.2020 21:40:42
 *  Author: mschr
 */ 

#include "simulate.h"
#include "map.h"
#include "main.h"

#ifdef SIMULATOR

uint8_t nextMove;
bool SearchFlag = true;

intersection_t intersSim[40];
intersection_t intersSimRun[20];
uint8_t intersectionPointer;

void simuInit(void)
{
	intersSim[0] = I_START;
	intersSim[1] = I_RIGHT;
	intersSim[2] = I_FOUR_WAY;
	intersSim[3] = I_LEFT;
	intersSim[4] = I_STRAIGHT_RIGHT;
	intersSim[5] = I_T_CROSS;
	intersSim[6] = I_STRAIGHT;
	intersSim[7] = I_RIGHT;
	intersSim[8] = I_STRAIGHT;
	intersSim[9] = I_STRAIGHT_RIGHT;
	intersSim[10] = I_RIGHT;
	intersSim[11] = I_STRAIGHT;
	intersSim[12] = I_STRAIGHT;
	intersSim[13] = I_RIGHT;
	intersSim[14] = I_STRAIGHT;
	intersSim[15] = I_STRAIGHT;
	intersSim[16] = I_LEFT;
	intersSim[17] = I_STRAIGHT_LEFT;
	intersSim[18] = I_RIGHT;
	intersSim[19] = I_LEFT;
	intersSim[20] = I_LEFT;
	intersSim[21] = I_T_CROSS;
	intersSim[22] = I_STRAIGHT;
	intersSim[23] = I_LEFT;
	intersSim[24] = I_STRAIGHT;
	intersSim[25] = I_STRAIGHT_LEFT;
	intersSim[26] = I_STRAIGHT;
	intersSim[27] = I_END;
	intersSim[28] = I_STRAIGHT;
	intersSim[29] = I_STRAIGHT_RIGHT;
	intersSim[30] = I_STRAIGHT_LEFT;
	intersSim[31] = I_RIGHT;
	intersSim[32] = I_FOUR_WAY;
	intersSim[33] = I_LEFT;
	intersSim[34] = I_DEAD_END;
	intersSim[35] = I_RIGHT;
	intersSim[36] = I_FOUR_WAY;
	intersSim[37] = I_LEFT;
	intersSim[38] = I_DEAD_END;
	intersSim[39] = I_STRAIGHT;
	
	intersSimRun[0] = I_START;
	intersSimRun[1] = I_RIGHT;
	intersSimRun[2] = I_FOUR_WAY;
	intersSimRun[3] = I_LEFT;
	intersSimRun[4] = I_STRAIGHT_RIGHT;
	intersSimRun[5] = I_T_CROSS;
	intersSimRun[6] = I_STRAIGHT;
	intersSimRun[7] = I_END;
	
	intersectionPointer = 0;
	
	nextMove = STRAIGHT;
}

void simulator(void)
{
	
	if (SearchFlag)
	{
		cellUpdate(intersSim[intersectionPointer]);
		mazePrint();
		
		if (nextMove != FINISH)
		{
			nextMove = maze_search(intersSim[intersectionPointer]);
			intersectionPointer++;
		}
		else
		{
			SearchFlag = false;
			intersectionPointer = 0;
			nextMove = STRAIGHT;
		}
	}
	else
	{
		
		if (nextMove != FINISH)
		{
			nextMove = mazeRun(intersSimRun[intersectionPointer++]);
			mazePrint();
			LOG_DEBUG("NextMove: %c", nextMove);
			intersectionPointer++;
		}
	}
	
	
}

#endif	//SIMULATOR