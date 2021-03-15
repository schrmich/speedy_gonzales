
/*
 * encoder.h
 *
 * Created: 08.08.2019 11:00:56
 *  Author: schreiberm
 */ 

#include "system.h"
#include "motor.h"

#ifndef ENCODER_H_
#define ENCODER_H_

#define ENCODER_LEFT 0
#define ENCODER_RIGHT 1

#define ENCODER_SLICES 32

/************************************************************************/
/*	Initialisiert die Radencoder                                        */
/************************************************************************/ 
void encoderInit(void);


/************************************************************************/
/* Reset Encoders                                                       */
/************************************************************************/
void encoderReset(void);

#endif /* ENCODER_H_ */