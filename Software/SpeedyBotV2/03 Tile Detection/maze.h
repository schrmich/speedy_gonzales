/*
 * maze.h
 *
 * Created: 12.06.2019 20:02:31
 *  Author: mschr
 */ 


#ifndef MAZE_H_
#define MAZE_H_

#include "system.h"
#include "pid.h"

// Zeit um zwischen den states zu warten
#define DELAY_STATE 500
#define DRIVE_DEAD_TIME 500

#define DELAY_TILE1 100
#define DELAY_TILE2 300
#define DELAY_MOTOROFF 600

#define LOGGING

#define REVERSE_TIME 1000

// Zeitbasis für PID Regler
// TA = 20ms
#define TA 50

/***********************************/
#define BASE_SPEED 80
#define K_P     0.5
#define K_I     0.00
#define K_D     0.5
/***********************************/


extern pidData_t pidData;

#define P_RIGHT		16
#define P_CRIGHT	8
#define P_CENTER	4
#define P_CLEFT		2
#define P_LEFT		1
#define P_FRONT		0b01110
#define P_END		0b11111

extern uint8_t linePattern;

void printPattern(uint8_t data);
uint8_t patternRead(void);

#define STRAIGHT	'S'
#define LEFT_TURN	'L'
#define RIGHT_TURN	'R'
#define U_TURN		'U'

typedef enum {
				I_DEAD_END,
				I_LEFT,
				I_RIGHT,
				I_T_CROSS,
				I_FOUR_WAY,
				I_STRAIGHT_LEFT,
				I_STRAIGHT_RIGHT,
				I_END
}intersection_t;

intersection_t validateIntersection(uint8_t initial, uint8_t second);
void printIntersection(intersection_t intersection);

#endif /* MAZE_H_ */