
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



/**
* @brief Entry Point of the FSM.
* Needs to be called from the main program to execute the FSM.
*/
void fsm_task(void);



#endif /* FSM_H_ */