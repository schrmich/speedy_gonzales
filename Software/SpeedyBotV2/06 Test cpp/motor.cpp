/*
 * motor.cpp
 *
 * Created: 09.10.2020 20:58:39
 *  Author: mschr
 */ 

#include <avr/io.h>
#include "motor.h"

#define MOTOR_DIR_RA	PIND4
#define MOTOR_DIR_RB	PINC2
#define MOTOR_DIR_LA	PINC3
#define MOTOR_DIR_LB	PINC4

// Konstructor
Motor::Motor()
{
	DDRC |= (1<<PINC2) | (1<<PINC3) | (1<<PINC4);
	DDRD |= (1<<PIND7) | (1<<PIND6) | (1<<PIND4);
	
	
	direction(MOT_BREAK, MOT_BREAK);
	
	// Motor PWM Treiber Timer2
	// Mode 3 Fast-PWM, Prescaler 8 f_pwm = 7812 HZ
	TCCR2A |= (1<<COM2A1) | (1<<COM0B1) | (1<<WGM21) | (1<<WGM20);
	TCCR2B |= (1<<CS22) | (1<<CS21) | (1<<CS20);
	//TIMSK2 |= (1<<TOIE2);
	OCR2A = 0x00;
	OCR2B = 0x00;
	
	Status.deltaLeft = MOTOR_DIFF_L;
}

// Destructor
Motor::~Motor()
{
	DDRC &= ~((1<<PINC2) | (1<<PINC3) | (1<<PINC4));
	DDRD &= ~((1<<PIND7) | (1<<PIND6) | (1<<PIND4));
}


void Motor::direction(uint8_t directionL, uint8_t directionR)
{
	/************************************************************************/
	/* Linker Motor                                                         */
	/************************************************************************/
	if (directionL == MOT_FWD)
	{
		PORTC |= (1<<MOTOR_DIR_LA);
		PORTC &= ~(1<<MOTOR_DIR_LB);
		Status.leftDir = MOT_FWD;
	}
	else if (directionL == MOT_REV)
	{
		PORTC |= (1<<MOTOR_DIR_LB);
		PORTC &= ~(1<<MOTOR_DIR_LA);
		Status.leftDir = MOT_REV;
	}
	else
	{
		PORTC &= ~( (1<<MOTOR_DIR_LA) | (1<<MOTOR_DIR_LB) );
		Status.leftDir = MOT_BREAK;
	}
	
	/************************************************************************/
	/* Rechter Motor                                                        */
	/************************************************************************/
	if (directionR == MOT_FWD)
	{
		PORTC &= ~(1<<MOTOR_DIR_RB);
		PORTD |= (1<<MOTOR_DIR_RA);
		Status.rightDir = MOT_FWD;
	}
	else if (directionR == MOT_REV)
	{
		PORTC |= (1<<MOTOR_DIR_RB);
		PORTD &= ~(1<<MOTOR_DIR_RA);
		Status.rightDir = MOT_REV;
	}
	else
	{
		PORTC &= ~(1<<MOTOR_DIR_RB);
		PORTD &= ~(1<<MOTOR_DIR_RA);
		Status.rightDir = MOT_BREAK;
	}
	
}

void Motor::speed(int16_t left, int16_t right)
{
	
	Status.speedRight = right;
	Status.speedLeft = left + Status.deltaLeft;
	
	if (Status.speedLeft  > 255)
	{
		Status.speedLeft  = 255;
	}
	else if (Status.speedLeft  < 0)
	{
		Status.speedLeft  = 0;
	}
	
	if (Status.speedRight > 255)
	{
		Status.speedRight = 255;
	}
	else if (Status.speedRight < 0)
	{
		Status.speedRight = 0;
	}
	
	// Set Left Motor Speed
	OCR2A = (uint8_t)Status.speedLeft;
	OCR2B = (uint8_t)Status.speedRight;
}


