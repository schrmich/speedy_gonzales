/*
 * simulate.h
 *
 * Created: 25.02.2020 21:39:59
 *  Author: mschr
 */ 

#include "map.h"
#include "main.h"

#ifndef SIMULATE_H_
#define SIMULATE_H_


#ifdef SIMULATOR

extern uint8_t nextMove;

#undef GOAL_X
#define GOAL_X	0
#undef GOAL_Y
#define GOAL_Y	3

#undef MAZE_X
#define MAZE_X 5

#undef MAZE_Y
#define MAZE_Y 4

void simuInit(void);

void simulator(void);

#endif /* SIMULATOR */

#endif /* SIMULATE_H_ */