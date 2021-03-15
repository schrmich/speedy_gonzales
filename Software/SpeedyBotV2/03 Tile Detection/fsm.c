/*!
 * @file fsm.c
 *
 * @date 12.04.2016
 * @author Michael Schreiber
 */ 

#include "fsm.h"
#include "system.h"
#include "pid.h"
#include "maze.h"
#include <stdlib.h>

/************************************************************************/
/* Makros and definitions.                                              */
/************************************************************************/

// 	Funktionstabelle
//	Achtung: Muss gleicher Reihenfolge wie der state_t enum sein!
state_func_t* const state_table[NUM_STATES] = {
	do_state_reset,
	do_state_begin,
	do_state_drive,
	do_state_stop,
	do_state_tile_detection,
	do_state_turn,
	do_state_error,
	};

// 	Hier werden die Transitions ausgeführt
//	Achtung: Muss gleicher Reihenfolge wie der state_t enum sein!
action_func_t * const action_table[NUM_STATES][2] = {
	{ NULL,							NULL					},	// State Reset
	{ in_state_begin, 				out_state_begin			},	// State Begin
	{ in_state_drive,				out_state_drive			},	// State Drive
	{ in_state_stop,				out_state_stop			},	// State Stop
	{ in_state_tile_detection,		out_state_tile_detection},	// State tile_detection
	{ in_state_turn,				out_state_turn			},	// State turn
	{ in_state_error,				NULL					}	// State Error
};



/*
*	Startet die FSM, der aktualle State wird ausgeführt. Bei einem Wechsel werden die Transisions aufgerufen
*/
state_t run_state(state_t cur_state, instance_data_t *data)
{
	// do_state wird ausgeführt
	state_t new_state = state_table[cur_state](data);

	// Wenn State-Wechsel dann führe die Transitions aus
	if (cur_state != new_state)
	{
		action_func_t *action = action_table[cur_state][1];
		if (action)
		{
			action(data);
		}

		action = action_table[new_state][0];
		if (action)
		{
			action(data);
		}
	}
	return new_state;
}


/*!
* @brief Manueller Statewechsel	
* Füht einen manuellen Statewechsel aus. Die Transitions werden ausgeführt\n
* Wird benötigt wenn der neue "IN_STATE" Aufruf sofort ausgeführt werden soll und nicht erst 
* bei der nächsten Triggerung der FSM durch das schedule_flag!
*/

state_t state_transition(state_t new_state, state_t old_state, instance_data_t *data)
{
	if (old_state != new_state)
	{
		action_func_t *action = action_table[old_state][1];
		if (action)
		{
			action(data);
		}

		action = action_table[new_state][0];
		if (action)
		{
			action(data);
		}
	}
	return new_state;
}


/*
* 	Haputfunktion der Steuerung.
*/
void fsm_task(void)
{
	// Instanzdaten der FSM
	static instance_data_t steuerungsDaten;

	
	static state_t steuerungsState = STATE_RESET;

	// Aufrufen des aktuellen States
	steuerungsState = run_state(steuerungsState, &steuerungsDaten);		//Statemachine ausführen
}

/************************************************************************/
/* STATE Reset                                                          */
/************************************************************************/

state_t do_state_reset(instance_data_t*data)
{
	data->byte = 0;
	//disableDevice(EN_ALL);
	clearLEDBlue();
	pid_Init(K_P*SCALING_FACTOR, K_I*SCALING_FACTOR, K_D*SCALING_FACTOR, &pidData );
	enableDevice( EN_LINE | EN_BLUETOOTH);
	return STATE_BEGIN;
}

/************************************************************************/
/* State BEGIN                                                          */
/************************************************************************/

void in_state_begin(instance_data_t *data)
{
	uart_puts_P("\n\nProgramm Tile detection.");
	uart_puts_P("\nDruecke Startknopf um zu starten.");
	pid_Init(K_P*SCALING_FACTOR, K_I*SCALING_FACTOR, K_D*SCALING_FACTOR, &pidData);
	setStopwatch1(0);
	enableDevice(EN_BLUETOOTH);
	
	waitStart();
}

state_t do_state_begin(instance_data_t *data)
{
	state_t next_state = STATE_BEGIN;
	
	if (systemStatus.pwrON)
	{
		setStatusLED(YELLOW);
		startStopwatch1();
	}
	
	
	if ( getStopwatch1() > 1500 )
	{
		next_state = STATE_DRIVE;
	}
	
	return next_state;
}

void out_state_begin(instance_data_t *data)
{
	setStatusLED(OFF);
	stopStopwatch1();
}

/************************************************************************/
/* State Drive		                                                    */
/************************************************************************/
void in_state_drive(instance_data_t *data)
{
	motorDirection(MOT_FWD, MOT_FWD);
	motor_speed(BASE_SPEED, BASE_SPEED);
	uart_puts_P("\nDrive Forward");
	pid_Reset_Integrator(&pidData);
	data->byte = 0;
	setStatusLED(RED);
	setStopwatch1(0);
	startStopwatch1();	// Timer für die Motorregelung
	setStopwatch2(0);
	startStopwatch2();	// Deadtime nach dem Losfahren
}

state_t do_state_drive(instance_data_t *data)
{
	state_t next_state = STATE_DRIVE;
	
	if ((getStopwatch1() >= TA) && !(data->tileDetected))
	{
		int16_t pidOutput, lineReference;
		int16_t left, right;
		
		lineReference = followTrack();
		
		setStopwatch1(0);
		pidOutput = pid_Controller(0, lineReference, &pidData);
		pidOutput = pidOutput >> 2;
			
		left = BASE_SPEED + pidOutput;
		right = BASE_SPEED - pidOutput;
			
		motor_speed(left, right);
	}
	
	// Deadtime, nach Start soll für eine kurze Zeit keine Kreuzungsdetektion erfolgen
	// Prüfe während der Fahrt, ob sich eine Kreuzung im Weg befindet
	if ( (lineSensors.digitalData[LINE_LEFT] || lineSensors.digitalData[LINE_RIGHT]) && (getStopwatch2() >= DRIVE_DEAD_TIME) )
	{
		next_state = STATE_STOP;
		data->tileDetected = true;
	}
	
	// Prüfe auf Sackgasse
	if (!data->tileDetected)
	{
		if( !(lineSensors.digitalData[LINE_LEFT] ||
			lineSensors.digitalData[LINE_CLEFT] ||
			lineSensors.digitalData[LINE_CENTER] ||
			lineSensors.digitalData[LINE_CRIGHT] ||
			lineSensors.digitalData[LINE_RIGHT] ))
		{
			data->tileDetected = true;
			next_state = STATE_STOP;
		}
			
	}
	
	return next_state;
}

void out_state_drive(instance_data_t *data)
{
	setStatusLED(OFF);
	// Tile gefunden-> Fahre noch etwas Geradeaus um eine sichere Detektion zu gewährleisten
	motor_speed(BASE_SPEED, BASE_SPEED);
	stopStopwatch1();
	stopStopwatch2();
}

/************************************************************************/
/* State Stop                                                           */
/************************************************************************/
void in_state_stop(instance_data_t *data)
{
	setStopwatch1(0);
	startStopwatch1();	// Messintervall Timing
	setStopwatch2(0);	// Wartezeit
	stopStopwatch2();
	data->byte = 0;
	setStatusLED(GREEN);
}

state_t do_state_stop(instance_data_t *data)
{
	state_t next_state = STATE_STOP;
	
	// Führe 1.Messungen nach Delay1 aus
	if (!data->firstMeaurement && (getStopwatch1() >= DELAY_TILE1))
	{
		data->tileFirst = patternRead();
		data->firstMeaurement = true;
	}
	// Führe 2.Messungen nach Delay2 aus,  Motor AUS
	if (!data->secondMeasurement && (getStopwatch1() >= DELAY_TILE2))
	{
		data->tileSecond = patternRead();
		data->secondMeasurement = true;
	}
	
	// Warten bis Bot still steht
	if (getStopwatch1() >= DELAY_MOTOROFF)
	{
		motorDirection(MOT_BREAK, MOT_BREAK);
		startStopwatch2();
	}
	
	if (getStopwatch2() > DELAY_STATE)
	{
		next_state = STATE_TILE_DETECTION;
	}
	
	
	return next_state;
}

void out_state_stop(instance_data_t *data)
{
	stopStopwatch1();
	setStopwatch1(0);
	//motorDirection(MOT_BREAK, MOT_BREAK);
}

/************************************************************************/
/* State tile_detection	                                                */
/************************************************************************/
void in_state_tile_detection(instance_data_t *data)
{
	setStatusLED(OFF);
}

state_t do_state_tile_detection(instance_data_t *data)
{
	state_t next_state = STATE_TURN;
	
	
	data->lastIntersection = validateIntersection(data->tileFirst, data->tileSecond);

	if (data->lastIntersection == I_LEFT)
		data->nextMove = LEFT_TURN;
	if (data->lastIntersection == I_RIGHT)
		data->nextMove = RIGHT_TURN;	
		
	#ifdef LOGGING
	printPattern(data->tileFirst);
	printPattern(data->tileSecond);
	printIntersection(data->lastIntersection);
	#endif
		

	return next_state;
}

void out_state_tile_detection(instance_data_t *data)
{
	stopStopwatch1();
}


/************************************************************************/
/* State Turn								                            */
/************************************************************************/
void in_state_turn(instance_data_t *data)
{
	#ifdef LOGGING
	uart_puts_P("\nMove: ");
	uart_putc(data->nextMove);
	#endif
	if (data->nextMove == LEFT_TURN)
	{
		rotate(LEFT, 90, NO_BLOCKING);
	}
	else if(data->nextMove == RIGHT_TURN)
	{
		rotate(RIGHT, 90, NO_BLOCKING);
	}
	stopStopwatch1();
	setStopwatch1(0);
}

state_t do_state_turn(instance_data_t *data)
{
	state_t next_state = STATE_TURN;
	
	if (!motorStatus.autoDrive)
		startStopwatch1();
	
	if (getStopwatch1() >= DELAY_STATE)
		next_state = STATE_ERROR;
	

	return next_state;
}

void out_state_turn(instance_data_t *data)
{
	stopStopwatch1();
}


/************************************************************************/
/* State Error                                                          */
/************************************************************************/
void in_state_error(instance_data_t *data)
{
	//disableDevice(EN_LINE);
	clearLEDBlue();
	uart_puts_P("\nError state.");
	uart_puts_P("\nBatt:");
	uart_putInt(systemStatus.battery);
	startStopwatch1();
	setStopwatch1(0);
}

state_t do_state_error(instance_data_t *data)
{
	state_t next_state = STATE_ERROR;
	
	bool toogle = false;
	
	while(!startButton())
	{
		if (getStopwatch1() > 500)
		{
			toogle ^= true;
			
			if (toogle)
			{
				setStatusLED(RED);
				printPattern(patternRead());
			}
			
			else
				setStatusLED(OFF);
			setStopwatch1(0);
		}
	}
	
	return next_state;
}

