
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
	STATE_DRIVE,
	STATE_STOP,
	STATE_TILE_DETECTION,
	STATE_TURN,
	STATE_ERROR,
	NUM_STATES} state_t;


/**
* @brief Error data to save information of the last error.
* Can be analysed in the STATE_ERROR.
*/
typedef enum {ERROR_LOW_BATT, NO_ERROR} error_state_t;

/**
* @brief datastructure to interchange data from a state to another state
*/
typedef struct {
	union {
		uint8_t byte;
		struct
		{
			uint8_t finish:1;
			uint8_t start:1;
			uint8_t tileDetected:1;
			uint8_t firstMeaurement:1;
			uint8_t secondMeasurement:1;
			uint8_t reserve:3;
		};
	};
	uint16_t timer;
	uint8_t tileFirst, tileSecond;
	intersection_t lastIntersection;
	char nextMove;
	error_state_t error;
} instance_data_t;



// Funktionstypenvereinbarung der Instanzfunktionen
typedef state_t state_func_t(instance_data_t *data);

//	Transitions Funktion Prototyp
typedef void action_func_t(instance_data_t *data);

/************************************************************************/
/* Function forward declarations                                        */
/************************************************************************/

//	States des Zustandsautomaten
state_t do_state_reset(instance_data_t*data);
state_t do_state_begin(instance_data_t *data);
state_t do_state_drive(instance_data_t *data);
state_t do_state_stop(instance_data_t *data);
state_t do_state_tile_detection(instance_data_t *data);
state_t do_state_turn(instance_data_t *data);
state_t do_state_error(instance_data_t *data);

//	Transitions
void in_state_begin(instance_data_t *data);
void out_state_begin(instance_data_t *data);
void in_state_drive(instance_data_t *data);
void out_state_drive(instance_data_t *data);
void in_state_stop(instance_data_t *data);
void out_state_stop(instance_data_t *data);
void in_state_tile_detection(instance_data_t *data);
void out_state_tile_detection(instance_data_t *data);
void in_state_turn(instance_data_t *data);
void out_state_turn(instance_data_t *data);
void in_state_error(instance_data_t *data);

/**
* @brief Entry Point of the FSM.
* Needs to be called from the main program to execute the FSM.
*/
void fsm_task(void);



#endif /* FSM_H_ */