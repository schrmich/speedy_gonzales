/*!
 * @file fsm.c
 *
 * @date 12.04.2016
 * @author Michael Schreiber
 */ 

#include "system.h"
#include "fsm.h"
#include "pid.h"
#include "maze.h"
#include <stdlib.h>
#include <util/atomic.h>
#include <avr/interrupt.h>

/************************************************************************/
/* Function forward declarations                                        */
/************************************************************************/

/*
	States des Zustandsautomaten
*/
state_t do_state_reset(instance_data_t*data);
state_t do_state_begin(instance_data_t *data);
state_t do_state_debug(instance_data_t *data);
state_t do_state_drive(instance_data_t *data);
state_t do_state_stop(instance_data_t *data);
state_t do_state_control(instance_data_t *data);
state_t do_state_turn(instance_data_t *data);
state_t do_state_finish(instance_data_t *data);
state_t do_state_error(instance_data_t *data);

/*
	Transitions
*/
void in_state_begin(instance_data_t *data);
void out_state_begin(instance_data_t *data);
void in_state_debug(instance_data_t *data);
void out_state_debug(instance_data_t *data);
void in_state_drive(instance_data_t *data);
void out_state_drive(instance_data_t *data);
void in_state_stop(instance_data_t *data);
void out_state_stop(instance_data_t *data);
void out_state_control(instance_data_t *data);
void in_state_turn(instance_data_t *data);
void out_state_turn(instance_data_t *data);
void in_state_finish(instance_data_t *data);
void out_state_finish(instance_data_t *data);
void in_state_error(instance_data_t *data);

// Funktionstypenvereinbarung der Instanzfunktionen
typedef state_t state_func_t(instance_data_t *data);

//	Transitions Funktion Prototyp
typedef void action_func_t(instance_data_t *data);


/************************************************************************/
/* Makros and definitions.                                              */
/************************************************************************/

// 	Funktionstabelle
//	Achtung: Muss gleicher Reihenfolge wie der state_t enum sein!
state_func_t* const state_table[NUM_STATES] = {
	do_state_reset,
	do_state_begin,
	do_state_debug,
	do_state_drive,
	do_state_stop,
	do_state_control,
	do_state_turn,
	do_state_finish,
	do_state_error,
	};

// 	Hier werden die Transitions ausgeführt
//	Achtung: Muss gleicher Reihenfolge wie der state_t enum sein!
action_func_t * const action_table[NUM_STATES][2] = {
	{ NULL,							NULL					},	// State Reset
	{ in_state_begin, 				out_state_begin			},	// State Begin
	{ in_state_debug,				out_state_debug			},	// State Debug
	{ in_state_drive,				out_state_drive			},	// State Drive
	{ in_state_stop,				out_state_stop			},	// State Stop
	{ NULL,							out_state_control		},	// State control
	{ in_state_turn,				out_state_turn			},	// State turn
	{ in_state_finish,				out_state_finish,		},	// State finish
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
	//LOG_INFO("State Reset");
	data->byte = 0;
	
	//disableDevice(EN_ALL);
	clearLEDBlue();
	pid_Init(K_P*SCALING_FACTOR, K_I*SCALING_FACTOR, K_D*SCALING_FACTOR, &pidData );
	disableDevice( EN_ENCODERS );
	
	return STATE_BEGIN;
}

/************************************************************************/
/* State BEGIN                                                          */
/************************************************************************/

void in_state_begin(instance_data_t *data)
{
	//LOG_INFO("State Begin");
	uart_puts_P("\n\nProgramm Maze Search.\n");
	
	pid_Init(K_P*SCALING_FACTOR, K_I*SCALING_FACTOR, K_D*SCALING_FACTOR, &pidData);
	myprintf_P("PID-Param= KP:%1.2f  KI:%1.2f  KD:%1.2f \n", K_P, K_I, K_D);
	myprintf_P("Battery:%i \n", batt_voltage());
	setStopwatch(STOPWATCH1, 0);
	setStopwatch(STOPWATCH2, 0);
	enableDevice(EN_BLUETOOTH);
	setStatusLED(GREEN);
	systemStatus.pwrON = true;
	lineSensors.calibrated = true;
	
	#ifdef TIMING_SENSORS
	myprintf_P("-USER zum Kalibrieren.\n-Start zum Sensortest\n");
	#else
	myprintf_P("-Press START for Maze Search.\n-Press USER for new calibration\n");
	#endif
}

state_t do_state_begin(instance_data_t *data)
{
	state_t next_state = STATE_BEGIN;
	
	if (startButton())
	{
		startStopwatch1();
		setStatusLED(YELLOW);
	}
	
	if (userButton())
	{
		startStopwatch2();
		setStatusLED(YELLOW);
	}
	
	if ( getStopwatch(STOPWATCH1) > 2000 )
	{
		setStatusLED(RED);
		next_state = STATE_DRIVE;
		
		#ifdef TIMING_SENSORS
		next_state = STATE_DEBUG_SENSORS;
		#endif
	}
	
	if (getStopwatch(STOPWATCH2) > 1500)
	{
		calibrateSensors();
		myprintf_P("Calibration finished.\n");
		stopStopwatch2();
		setStopwatch(STOPWATCH2, 0);
	}
	
	return next_state;
}

void out_state_begin(instance_data_t *data)
{
	myprintf_P("Start Maze search!\n");
	setStatusLED(OFF);
	stopStopwatch1();
}


/************************************************************************/
/* State Debug		                                                    */
/************************************************************************/

void in_state_debug(instance_data_t *data)
{
	LOG_DEBUG("Liniensensor Timing Test");
	
	motorDirection(MOT_FWD, MOT_FWD);
	motor_speed(BASE_SPEED, BASE_SPEED);
	
	pid_Reset_Integrator(&pidData);
	setStatusLED(YELLOW);
	data->tileDetected = false;
	
	setStopwatch(STOPWATCH1, 0);
	startStopwatch1();	// Timer für die Motorregelung
	
	setStopwatch(STOPWATCH2, 0);
	startStopwatch2();	// Deadtime nach dem Losfahren
	
	stopStopwatch3();	
	setStopwatch(STOPWATCH3, 0);	// Timer Debugausgabe
	
	startStopwatch4();
	setStopwatch(STOPWATCH4, 0);	// Timer Blinken LED
	
	stopStopwatch5();
	setStopwatch(STOPWATCH5, 0);
	
	
	lineRead();	// Initiale Sensorabtastung
}

state_t do_state_debug(instance_data_t *data)
{
	state_t next_state = STATE_DEBUG_SENSORS;
	
	
	/*
		Prüfe während der Fahrt, ob sich eine Kreuzung im Weg befindet
	*/
	if ( !data->tileDetected )
	{
		// Prüfe auf  Abzweigung links oder rechts
		if ( lineSensors.digitalData[LINE_LEFT] || lineSensors.digitalData[LINE_RIGHT] )
		{
			enableDevice(EN_ENCODERS);
			LOG_DEBUG("Tile Detected.");
			motor_speed(BASE_SPEED, BASE_SPEED);
			startStopwatch3();
			startStopwatch5();
			data->tileDetected = true;
			encoderReset();
		}
	}
	
	/*
		Fahre Linie entlang
		Motor Regelung
	*/
	if ( (getStopwatch(STOPWATCH1) >= TA) && !data->tileDetected)
	{
		int16_t pidOutput, lineReference;
		int16_t left, right;
			
		lineRead();
			
		lineReference = followTrack() -10;
			
		setStopwatch(STOPWATCH1, 0);
			
		pidOutput = pid_Controller(0, lineReference, &pidData);
			
		left = BASE_SPEED + pidOutput;
		right = BASE_SPEED - pidOutput;
			
		motor_speed(left, right);
	}
	
	
	if ( (getStopwatch(STOPWATCH3) >= 20) && data->tileDetected)
	{
		setStopwatch(STOPWATCH3, 0);
		
		uint16_t binaer, distance;
		
		lineRead();
		
		data->tileFirst = patternRead();
		binaer = dezToBin(data->tileFirst);
		
		distance = motorStatus.encoder[ENCODER_LEFT] + motorStatus.encoder[ENCODER_RIGHT];
		
		LOG_DEBUG("t:%ims E:%i s%05d", getStopwatch(STOPWATCH5), distance, binaer);
		
		if (getStopwatch(STOPWATCH5) > 500)
		{
			disableDevice(EN_ENCODERS);
			next_state = STATE_RESET;
		}
	}
	
	/*
		StatusLed gelb blinkend
	*/
	if (getStopwatch(STOPWATCH4) > 100)
	{
		data->toogleLED ^= true;
		
		if (data->toogleLED)
			setStatusLED(YELLOW);
		else
			setStatusLED(OFF);
		
		setStopwatch(STOPWATCH4, 0);
	}
	
	
	return next_state;
}

void out_state_debug(instance_data_t *data)
{
	LOG_DEBUG("Test Finished.");
	motor_speed(0, 0);
	motorDirection(MOT_BREAK,MOT_BREAK);
	stopStopwatch1();
	stopStopwatch2();
	stopStopwatch3();
	stopStopwatch4();
	stopStopwatch5();
}

/************************************************************************/
/* State Drive		                                                    */
/************************************************************************/
void in_state_drive(instance_data_t *data)
{
	//LOG_INFO("State Drive");
	motorDirection(MOT_FWD, MOT_FWD);
	motor_speed(BASE_SPEED, BASE_SPEED);
	
	pid_Reset_Integrator(&pidData);
	setStatusLED(YELLOW);
	data->tileDetected = false;
	setStopwatch(STOPWATCH1, 0);
	startStopwatch1();	// Timer für die Motorregelung
	setStopwatch(STOPWATCH2, 0);
	startStopwatch2();	// Deadtime nach dem Losfahren
	startStopwatch3();	// Staus LED blinken
	
	lineRead();	// Initiale Sensorabtastung	
}

state_t do_state_drive(instance_data_t *data)
{
	state_t next_state = STATE_DRIVE;
	
	
	/*
		Prüfe während der Fahrt, ob sich eine Kreuzung im Weg befindet
		Deadtime, nach dem Start soll für eine kurze Zeit keine Kreuzungsdetektion erfolgen
	*/
	if (getStopwatch(STOPWATCH2) >= DRIVE_DEAD_TIME)
	{
		// Prüfe auf  Abzweigung links oder rechts
		if ( lineSensors.digitalData[LINE_LEFT] || lineSensors.digitalData[LINE_RIGHT] )
		{
			//LOG_INFO("Tile Detected.");
			next_state = STATE_STOP;
			data->tileDetected = true;
		}
		
		// Prüfe auf Sackgasse
		if( !(lineSensors.digitalData[LINE_LEFT] ||
			lineSensors.digitalData[LINE_CLEFT] ||
			lineSensors.digitalData[LINE_CENTER] ||
			lineSensors.digitalData[LINE_CRIGHT] ||
			lineSensors.digitalData[LINE_RIGHT]) )
		{
			//LOG_INFO("Dead End detected.");
			next_state = STATE_STOP;
			data->tileDetected = true;
		}
	}
	
	/*
		Fahre Linie entlang
		Motor Regelung
	*/
	if ( (getStopwatch(STOPWATCH1) >= TA) && !data->tileDetected)
	{
		int16_t pidOutput, lineReference;
		int16_t left, right;
			
		lineRead();
			
		lineReference = followTrack() -10;
			
		setStopwatch(STOPWATCH1, 0);
			
		pidOutput = pid_Controller(0, lineReference, &pidData);
			
		left = BASE_SPEED + pidOutput;
		right = BASE_SPEED - pidOutput;
			
		motor_speed(left, right);
	}
	
	/*
		StatusLed gelb blinkend
	*/
	if (getStopwatch(STOPWATCH3) > 100)
	{
		data->toogleLED ^= true;
		
		if (data->toogleLED)
			setStatusLED(YELLOW);
		else
			setStatusLED(OFF);
		
		setStopwatch(STOPWATCH3, 0);
	}
	
	
	return next_state;
}

void out_state_drive(instance_data_t *data)
{
	// Tile gefunden-> Fahre noch etwas Geradeaus um eine sichere Detektion zu gewährleisten
	motor_speed(BASE_SPEED, BASE_SPEED);
	stopStopwatch1();
	stopStopwatch2();
	stopStopwatch3();
	setStatusLED(OFF);
}

/************************************************************************/
/* State Stop                                                           */
/************************************************************************/
void in_state_stop(instance_data_t *data)
{
	setStopwatch(STOPWATCH1, 0);
	startStopwatch1();				// Messintervall Timing
	
	data->firstMeaurement = false;
	data->secondMeasurement = false;
}

state_t do_state_stop(instance_data_t *data)
{
	state_t next_state = STATE_STOP;
	
	
	// Führe 1.Messungen nach Delay1 aus
	if (!data->firstMeaurement && (getStopwatch(STOPWATCH1) >= DELAY_TILE1))
	{
		
		lineRead();
		data->tileFirst = patternRead();
		data->firstMeaurement = true;
		
		#ifdef LOGGING
		uint16_t binaer;
		char buffer[16];
		itoa(data->tileFirst, buffer, 2);
		binaer = dezToBin(data->tileFirst);
		LOG_DEBUG("First Tile:%05d", binaer);
		#endif
	}
	
	// Führe 2.Messungen nach Delay2 aus
	if (!data->secondMeasurement && (getStopwatch(STOPWATCH1) >= DELAY_TILE2))
	{
		lineRead();
		data->tileSecond = patternRead();
		data->secondMeasurement = true;
		
		#ifdef LOGGING
		uint16_t binaer;
		char buffer[16];
		itoa(data->tileSecond, buffer, 2);
		binaer = dezToBin(data->tileSecond);
		LOG_DEBUG("Second Tile:%05d", binaer);
		#endif
	}
	
	
	// Nachlaufzeit, Bot soll erst zum Stillstand kommen
	// wenn Räder mittig der Kreuzung sind
	if (getStopwatch(STOPWATCH1) >= DELAY_MOTOROFF)
	{
		motorDirection(MOT_BREAK, MOT_BREAK);
		next_state = STATE_CONTROL;
	}
	
	return next_state;
}

void out_state_stop(instance_data_t *data)
{
	stopStopwatch1();
	setStopwatch(STOPWATCH1, 0);
	setStatusLED(OFF);
}


/************************************************************************/
/* State Control							                            */
/************************************************************************/

state_t do_state_control(instance_data_t *data)
{
	state_t next_state = STATE_TURN;
	
	char buffer[15];
	
	/*	
		Ermittle aus den zwei Mustern den richtigen Kreuzungstyp, noch keine Entscheidung
		über das Abbiegen oder Verhalten	
	*/	
	data->lastIntersection = validateIntersection(data->tileFirst, data->tileSecond);
		
	printIntersection(buffer, data->lastIntersection);
	myprintf_P("Actual Intersection Type: %s\n", buffer);
	
	/*
		Entscheidung wie gefahren werden soll, Linkehandregel!
	*/
	data->nextMove = mazeSearch(data->lastIntersection);
	
	
	/************************************************************************/
	/* Bot ist im Erkundungsmodus                                           */
	/************************************************************************/
	if (data->finishSearch == false)
	{
		/*
		Einfache Abbiegungen wie Links und Rechts werden nicht gespeichert!
		Es wird die Richtung in data->nextMove gefahren.
		*/
		if (!(data->lastIntersection == I_LEFT || data->lastIntersection == I_RIGHT))
		{
			/*
				Speichere Entscheidung im Pfad-Array
			*/
			pathMemory[pathIndex++] = data->nextMove;
			pathMemory[pathIndex] = '\0';
		
			/*
				Überprüfung ob die Speichergrenze überschritten wurde,
				wenn ja sofort Error-State ausführen
			*/
			if (pathIndex >= PATH_MEM_SIZE - 1)
			{
				data->error = ERROR_PATH_SIZE;
				return STATE_ERROR;
			}	
		}
		/*
		Maze ende erreicht?
		*/
		if (data->nextMove == FINISH)
			next_state = STATE_FINISH;
	}
	
		
	/************************************************************************/
	/* Maze run, Search completed -> drive pathmemory                       */
	/************************************************************************/
	if(data->finishSearch)  
	{
		/*
			Einfache Abbiegungen wie Links und Rechts werden nicht aus dem Speicher geladen!
			Es wird die Richtung in data->nextMove gefahren.
		*/
		if (!(data->lastIntersection == I_LEFT || data->lastIntersection == I_RIGHT))
		{
			/*
				Letzte Position ist das Ziel -> Mazerun finished
			*/
			if (pathMemory[pathIndex] == '\0')
			{
				return STATE_FINISH;
			
			}
		
			/*
				Überprüfe ob gespeicherter Pfad auf Kreuzungstyp anwendbar ist.
			*/
			if (pathCompare(pathMemory[pathIndex], data->lastIntersection))
			{
				// Kreuzung ist gültig -> fahre gespeicherten Weg
				data->nextMove = pathMemory[pathIndex++];
			}
			else
			{
				// Kreuzung ist inkompatibel mit gespeichertem Weg -> Error State
				data->error = ERROR_LINE_MATCH;
				return STATE_ERROR;
			}
		}
	}
	
	return next_state;
}

void out_state_control(instance_data_t *data)
{
	/*
		Ausgabe des aktuellen Wegspeichers 
	*/
	if (data->finishSearch == false)
		myprintf_P("Path Memory: %s\n", pathMemory);
}


/************************************************************************/
/* State Turn								                            */
/************************************************************************/
void in_state_turn(instance_data_t *data)
{
	setStatusLED(GREEN);
	
	/*
		Führe das aktuelle Bewegungskommando aus
		Nicht blockierend
	*/
	if (data->nextMove == LEFT_TURN)
		rotate(LEFT, ROTATE_90, NO_BLOCKING);
	else if(data->nextMove == RIGHT_TURN)
		rotate(RIGHT, ROTATE_90, NO_BLOCKING);
	else if (data->nextMove == U_TURN)
		rotate(RIGHT, ROTATE_180, NO_BLOCKING);
	
	stopStopwatch1();
	setStopwatch(STOPWATCH1, 0);
	startStopwatch2();
}

state_t do_state_turn(instance_data_t *data)
{
	state_t next_state = STATE_TURN;
	
	/*
		Warte bis die Drehbewegung ausgeführt ist, dann starte
		Nachlaufzeit bis der Bot zum Stillstand gekommen ist
	*/
	if (!motorStatus.autoDrive)
		startStopwatch1();
	
	if (getStopwatch(STOPWATCH1) >= 50)
		next_state = STATE_DRIVE;
	
	/*
		StatusLed gelb blinkend
	*/
	if (getStopwatch(STOPWATCH2) > 100)
	{
		data->toogleLED ^= true;
		
		if (data->toogleLED)
			setStatusLED(GREEN);
		else
			setStatusLED(OFF);
		
		setStopwatch(STOPWATCH2, 0);
	}

	return next_state;
}

void out_state_turn(instance_data_t *data)
{
	setStatusLED(OFF);
	stopStopwatch1();
	stopStopwatch2();
}


/************************************************************************/
/* State Finish								                            */
/************************************************************************/
void in_state_finish(instance_data_t *data)
{	
	if (!data->finishSearch)
	{
		myprintf_P("\n**********Maze Search finished!****************\n");
		myprintf_P("Path Memory: %s\n", pathMemory);
		mazeOptimize(pathMemory);
		myprintf_P("Path Optimized: %s\n", pathMemory);
		pathIndex = 0;
		myprintf_P("Batt:%i\n", batt_voltage());
		myprintf_P("Press Start-Button to start the Speedrun\n");
		data->finishSearch = true;
	}
	else
	{
		myprintf_P("\n*************Maze Solve finished!****************\n");
		myprintf_P("Batt:%i\n", batt_voltage());
		myprintf_P("Press Start-Button to start the Speedrun again\n\n");
		pathIndex = 0;
	}
	
	setLEDBlue();
	setStatusLED(GREEN);
	setStopwatch(STOPWATCH1,0);	
	stopStopwatch1();	// Verzögerung nach drücken Start-Taste
	setStopwatch(STOPWATCH2,0);
	startStopwatch2();	// Blinken Untere Leds
}

state_t do_state_finish(instance_data_t *data)
{
	state_t next_state = STATE_FINISH; 
	
	/*
		Blinken Unterbodenbeleuchtung
	*/
	if (getStopwatch(STOPWATCH2) > 500)
	{
		data->toogleLED ^= true;
			
		if (data->toogleLED)
		setLEDBlue();
		else
		clearLEDBlue();
			
		setStopwatch(STOPWATCH2,0);
	}
	
	/*
		Starte Mazerun
	*/
	if ( startButton() )
	{
		startStopwatch1();
		setLEDBlue();
		setStatusLED(RED);
	}
		
	if (getStopwatch(STOPWATCH1) > 2000)
	{
		next_state = STATE_DRIVE;
	}
	
	return next_state;
}

void out_state_finish(instance_data_t *data)
{
	setStatusLED(OFF);
	clearLEDBlue();
	stopStopwatch1();
}

/************************************************************************/
/* State Error                                                          */
/************************************************************************/
void in_state_error(instance_data_t *data)
{
	//disableDevice(EN_LINE);
	clearLEDBlue();
	LOG_ERROR("State Error: %i", data->error);
	switch(data->error)
	{
		case ERROR_LINE_MATCH:
		LOG_ERROR("Intersection doesn't match to stored Turn in Path-Memory!");
		break;
		case ERROR_INTER_READ:
		LOG_ERROR("Intersection Read-Error.");
		break;
		case ERROR_PATH_SIZE:
		LOG_ERROR("Path Memory Full.");
		break;
		default:
		LOG_ERROR("Unknown Error");
		break; 
	}
	
	LOG_INFO("Batt:%i", batt_voltage());
	startStopwatch1();
	setStopwatch(STOPWATCH1,0);
}

state_t do_state_error(instance_data_t *data)
{
	state_t next_state = STATE_ERROR;
	
	bool toogle = false;
	
	
	if (getStopwatch(STOPWATCH1) > 500)
	{
		toogle ^= true;
			
		if (toogle)
			setStatusLED(RED);
		else
			setStatusLED(OFF);
			
		setStopwatch(STOPWATCH1,0);
	}
	
	
	return next_state;
}

