/*
 * encoder.c
 *
 * Created: 08.08.2019 11:04:11
 *  Author: schreiberm
 */ 

#include "system.h"
#include "encoder.h"
#include <stdbool.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

/************************************************************************/
/*	Initialisiert die Radencoder                                        */
/************************************************************************/
void encoderInit(void)
{
	// Konfiguiere Externe Interrupts INT0 & INT1, Triggerung auf beide Flanken
	EICRA |= (1<<ISC10) | (1<<ISC00);
	
	// Interrupts aktivieren
	EIMSK |= (1<<INT1) | (1<<INT0);
}

/************************************************************************/
/* Reset Encoders                                                       */
/************************************************************************/
void encoderReset(void)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		motorStatus.encoder[LEFT] = 0;
		motorStatus.encoder[RIGHT] = 0;
	}
}

/************************************************************************/
/* Interrupt Service Routinen                                           */
/************************************************************************/

// Linker Motor
ISR(INT0_vect)
{
	if (systemStatus.encoders == true)
	{
		if (motorStatus.leftDir == MOT_FWD)
			motorStatus.encoder[LEFT]++;
		else if (motorStatus.leftDir == MOT_REV)
			motorStatus.encoder[LEFT]--;
	}
}

// Rechter Motor
ISR(INT1_vect)
{
	if (systemStatus.encoders == true)
	{
		if (motorStatus.rightDir == MOT_FWD)
		motorStatus.encoder[RIGHT]++;
		else if (motorStatus.rightDir == MOT_REV)
		motorStatus.encoder[RIGHT]--;
	}
}