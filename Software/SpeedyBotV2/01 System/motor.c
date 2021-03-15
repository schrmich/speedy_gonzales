/*
 * motor.c
 *
 * Created: 01.08.2019 09:16:35
 *  Author: schreiberm
 */ 


#include "motor.h"
#include "system.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define MOTOR_DIR_RA	PIND4
#define MOTOR_DIR_RB	PINC2
#define MOTOR_DIR_LA	PINC3
#define MOTOR_DIR_LB	PINC4

void motorDirection(uint8_t directionL, uint8_t directionR)
{
	/************************************************************************/
	/* Linker Motor                                                         */
	/************************************************************************/
	if (directionL == MOT_FWD)
	{
		PORTC |= (1<<MOTOR_DIR_LA);
		PORTC &= ~(1<<MOTOR_DIR_LB);
		motorStatus.leftDir = MOT_FWD;
	}
	else if (directionL == MOT_REV)
	{
		PORTC |= (1<<MOTOR_DIR_LB);
		PORTC &= ~(1<<MOTOR_DIR_LA);
		motorStatus.leftDir = MOT_REV;
	}
	else
	{
		PORTC &= ~( (1<<MOTOR_DIR_LA) | (1<<MOTOR_DIR_LB) );
		motorStatus.leftDir = MOT_BREAK;
	}
	
	/************************************************************************/
	/* Rechter Motor                                                        */
	/************************************************************************/
	if (directionR == MOT_FWD)
	{
		PORTC &= ~(1<<MOTOR_DIR_RB);
		PORTD |= (1<<MOTOR_DIR_RA);
		motorStatus.rightDir = MOT_FWD;
	}
	else if (directionR == MOT_REV)
	{
		PORTC |= (1<<MOTOR_DIR_RB);
		PORTD &= ~(1<<MOTOR_DIR_RA);
		motorStatus.rightDir = MOT_REV;
	}
	else
	{
		PORTC &= ~(1<<MOTOR_DIR_RB);
		PORTD &= ~(1<<MOTOR_DIR_RA);
		motorStatus.rightDir = MOT_BREAK;
	}
	
}

void motor_speed(int16_t left, int16_t right)
{
	motorStatus.speedRight = right;
	motorStatus.speedLeft = left + motorStatus.deltaLeft;
	
	if (motorStatus.speedLeft  > UINT8_MAX)
	{
		motorStatus.speedLeft  = UINT8_MAX;
	}
	else if (motorStatus.speedLeft  < 0)
	{
		motorStatus.speedLeft  = 0;
	}
	
	if (motorStatus.speedRight > UINT8_MAX)
	{
		motorStatus.speedRight = UINT8_MAX;
	}
	else if (motorStatus.speedRight < 0)
	{
		motorStatus.speedRight = 0;
	}
	
	// Set Left Motor Speed
	OCR2A = (uint8_t)motorStatus.speedLeft;
	OCR2B = (uint8_t)motorStatus.speedRight;
}

void rotate(uint8_t direction, uint16_t angle, uint8_t blocking)
{
	
	uint16_t timer;
	motor_speed(TURN_SPEED, TURN_SPEED);
		
	if (blocking == BLOCKING)
	{
		if (direction == LEFT)
		{
			motorDirection(MOT_REV, MOT_FWD);
			setStopwatch(STOPWATCH8,0);
			startStopwatch8();
			do
			{
				timer = getStopwatch(STOPWATCH8);
			}while(timer < angle);
				
			motorDirection(MOT_BREAK, MOT_BREAK);
			stopStopwatch8();
		}
		else if (direction == RIGHT)
		{
			motorDirection(MOT_FWD, MOT_REV);
			setStopwatch(STOPWATCH8,0);
			startStopwatch8();
			do
			{	
				timer = getStopwatch(STOPWATCH8);
			}while(timer < angle);
				
			motorDirection(MOT_BREAK, MOT_BREAK);
			stopStopwatch8();
		}
	}
	else if (blocking == NO_BLOCKING)
	{
			
		if (direction == LEFT)	motorDirection(MOT_REV, MOT_FWD);
		else if (direction == RIGHT) motorDirection(MOT_FWD, MOT_REV);
			
		motorStatus.autoDrive= true;
		setStopwatch(STOPWATCH8,0);
		startStopwatch8();
		motorStatus.timer = angle;
	}
	
}


void motorInit(void)
{
	motorDirection(MOT_BREAK, MOT_BREAK);
	
	// Motor PWM Treiber Timer2
	// Mode 3 Fast-PWM, Prescaler 8 f_pwm = 7812 HZ
	TCCR2A |= (1<<COM2A1) | (1<<COM0B1) | (1<<WGM21) | (1<<WGM20);
	TCCR2B |= (1<<CS22) | (1<<CS21) | (1<<CS20);
	//TIMSK2 |= (1<<TOIE2);
	OCR2A = 0x00;
	OCR2B = 0x00;
	
	motorStatus.deltaLeft = MOTOR_DIFF_L;
}