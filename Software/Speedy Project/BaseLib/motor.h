/*
* motor.h
*
* Created: 09.10.2020 20:58:52
*  Author: mschr
*/


#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>

// SPEED @ 80 ca. 8cm/s
// SPEED @ 120 ca. 17,5cm/s

//#define TURN_SPEED 45
#define TURN_SPEED 100

#define MOT_BREAK	0
#define MOT_FWD		1
#define MOT_REV		2

#define BLOCKING 0
#define NO_BLOCKING 1

#define MOTOR_DIFF_L -13


#define ROTATE_90 370
#define ROTATE_180 800

class Motor
{
	private:
	
	
	public:
	
	struct {
		union {
			uint8_t byte;
			struct
			{
				uint8_t leftDir:2;
				uint8_t rightDir:2;
				uint8_t autoDrive:1;
				uint8_t reserve:3;
			};
		};
		
		int16_t speedLeft;
		int16_t speedRight;
		int8_t deltaLeft;
		int16_t encoder[2];
		uint16_t timer;
		float rotateGain;
		uint8_t rotateOffset;
	} Status;
	
	/*!
	@brief Motor Konstrukor
	*/
	Motor();
	
	~Motor();


	/*!
	@brief	Set motor direction
	
	\param	MOT_FWD, MOT_REV, MOT_BREAK
	\param	MOT_FWD, MOT_REV, MOT_BREAK
	*/
	void direction(uint8_t directionL, uint8_t directionR);


	/*!
	@brief	Set motor speed
	
	The motor turn at the desired speed at the direction set in
	funktion motor_direction()
	\param	0-255
	\param	0-255
	*/
	void speed(int16_t left, int16_t right);
	
	// Wenn die Motoren im NON_BLOCKING Modus sind wird überprüft wann
	// die Fahrzeit abgelaufen ist, wenn ja werden die Motoren angehalten
	void taskDrive(void);
	
};


#endif /* MOTOR_H_ */