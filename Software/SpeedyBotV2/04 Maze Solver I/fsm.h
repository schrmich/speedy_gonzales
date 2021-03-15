
/**
 * fsm.h
 *
 * Created: 12.04.2016 06:22:03
 *  Author: Michael Schreiber
 *
 * Diese Steuerung hat die Aufgabe einen Joghurtbecher zu detektieren und ihn aus einem Spielfeld zu
 * schieben. Das Spielfeld kann jeder beliebiger heller Untergrund sein, welcher mit einem dunkelen
 * Streifen abgegrenzt ist. Der Bot fährt im Spielfeld herumum den Becher zu suchen. Dabei darf er nicht
 * das Spielfeld verlassen. Wurde ein Becher gefunden wird dieser mit Hilfe einer PID-Regelung zentriert
 * und so lange geschoben bis der Spielfeldrand erreicht ist. 
 */ 

#include "system.h"
#include "maze.h"

#ifndef FSM_H_
#define FSM_H_


/************************************************************************/
/* State Machines flags				                                    */
/************************************************************************/


#ifndef NULL
#define NULL (void*) 0
#endif

/**
* @brief Lists all states of the FSM.
* NUM_STATES is not a state, it is used to count the amount off all states.
*/
typedef enum {
	STATE_RESET,
	STATE_BEGIN,
	STATE_DEBUG_SENSORS,
	STATE_DRIVE,
	STATE_STOP,
	STATE_CONTROL,
	STATE_TURN,
	STATE_FINISH,
	STATE_ERROR,
	NUM_STATES
} state_t;


/**
* @brief Error data to save information of the last error.
* Can be analysed in the STATE_ERROR.
*/
typedef enum {ERROR_LINE_MATCH, ERROR_INTER_READ, ERROR_PATH_SIZE, NO_ERROR} error_state_t;

/**
* @brief datastructure to interchange data from a state to another state
*/
typedef struct {
	union {
		uint8_t byte;
		struct
		{
			uint8_t finishSearch:1;
			uint8_t tileDetected:1;
			uint8_t firstMeaurement:1;
			uint8_t secondMeasurement:1;
			uint8_t toogleLED:1;
			uint8_t reserve:3;
		};
	};
	uint8_t tileFirst, tileSecond;
	intersection_t lastIntersection;
	char nextMove;
	error_state_t error;
} instance_data_t;


/**
* @brief Entry Point of the FSM.
* Needs to be called from the main program to execute the FSM.
*/
void fsm_task(void);



#endif /* FSM_H_ */