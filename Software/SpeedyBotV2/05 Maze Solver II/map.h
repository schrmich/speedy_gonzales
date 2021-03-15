/*
 * flood.h
 *
 * Created: 10.02.2020 07:19:14
 *  Author: mschr
 */ 

#include <stdint.h>
#include "maze.h"
#include "stack.h"

#ifndef MAP_H_
#define MAP_H_

typedef enum {	NORTH=0, EAST, SOUTH, WEST, DIR_ERROR} direction_t;
	
#define MAZE_X	5
#define MAZE_Y	4

#define STACK_SIZE 30

//#define TEST_STACK

#define CELL_LENGHT 20

#define GOAL_X	0
#define GOAL_Y	3

#define START_X 0
#define START_Y 0

typedef enum { MAZE_SEARCH, MAZE_BACKWARD, MAZE_DEAD_END, MAZE_GOAL, MAZE_RESTART }stateMaze_t;

extern stateMaze_t mazeState;

typedef struct{	
	union {
		uint8_t byte;
		struct
		{
			uint8_t wall_N:1;
			uint8_t wall_S:1;
			uint8_t wall_E:1;
			uint8_t wall_W:1;
			uint8_t visited:1;
			uint8_t goal:1;
			uint8_t start:1;
			uint8_t dead:1;
		};
	};
		int8_t distance; 
}tile_t;

typedef struct {
	uint8_t x;
	uint8_t y;
}position_t;


tile_t maze[MAZE_X][MAZE_Y];

extern direction_t bot_orientation;
extern position_t bot_position;
extern uint8_t distance_counter;

extern struct Stack *searchStack;

void maze_init(void);

uint8_t go_Cell(position_t destination);

uint8_t maze_search(intersection_t actual_intersection);

void floodFill(int8_t x, int8_t y, uint8_t fill);

void mazePrint(void);

void store_actual_walls(intersection_t intersection);

void cellUpdate(intersection_t actual_intersection);

uint8_t mazeRun(intersection_t actual_intersection);

#endif /* MAP_H_ */