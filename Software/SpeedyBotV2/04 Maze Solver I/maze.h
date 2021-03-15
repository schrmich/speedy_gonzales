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
#define DELAY_STATE 50
#define DRIVE_DEAD_TIME 250

#define DELAY_TILE1 50
#define DELAY_TILE2 200

#define DELAY_MOTOROFF 300
//#define DELAY_MOTOROFF 400



//#define TIMING_SENSORS

//#define LOGGING

// Zeitbasis für PID Regler
// TA = 40ms
#define TA 40


/***********************************/
#define BASE_SPEED 120
#define BREAK_SPEED 50	// used only for higher speeds
#define K_P     0.8
#define K_I     0.00
#define K_D     0.8
/***********************************/


#define P_RIGHT		16
#define P_CRIGHT	8
#define P_CENTER	4
#define P_CLEFT		2
#define P_LEFT		1
#define P_FRONT		0b01110
#define P_END		0b11111
#define P_TABLE_EDGE 0x00

uint8_t patternRead(void);

/*	Steuerkommandos für PathMemory Array	*/
#define STRAIGHT	'S'
#define LEFT_TURN	'L'
#define RIGHT_TURN	'R'
#define U_TURN		'U'
#define LINE_LOST	'X'
#define FINISH		'\0'

/*	Mögliche Kreuzungstypen	*/
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

#define PATH_MEM_SIZE 50
extern char pathMemory[PATH_MEM_SIZE];
extern uint8_t pathIndex;

extern pidData_t pidData;


uint16_t dezToBin(uint16_t dezimal);
intersection_t validateIntersection(uint8_t initial, uint8_t second);
char *printIntersection(char *interType, intersection_t intersection);
char mazeSearch(intersection_t intersection);
void mazeOptimize(char *path);
bool pathCompare(char storedPath, intersection_t actualIntersection);


#endif /* MAZE_H_ */