/*
 * testfunctions.c
 *
 * Created: 31.03.2020 16:26:34
 *  Author: mschr
 */ 

#include "speedy.h"
#include <util/atomic.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

//#define MOTOR_SPEED 200
#define MOTOR_SPEED 120

uint16_t angle;


uint8_t taskID_1, taskID_2, taskID_3;

uint8_t parserRotateEncoder(void)
{
	char buffer[10];
	uint8_t _angle;
	uint16_t temp;
	uint8_t index = 0;
	bool rxComplete = false;
	
	uart_puts_P("\nSet Angle: ");
	
	while (!rxComplete)
	{
		temp = uart_getc();
		if (temp != UART_NO_DATA)
		{
			buffer[index] = (char) temp;
			if (buffer[index] == '\n')
			{
				buffer[++index] = '\0';
				rxComplete = true;
			}
			index++;
		}
	}
	uart_puts(buffer);
	_angle = atoi(buffer);
	
	return _angle;
}

void TestRotateEncoder(void)
{
	uint8_t encoder_sum = 0;
	uint8_t _angle = 0;
	
	_angle = parserRotateEncoder();
	
	enableDevice(EN_ENCODERS);
	encoderReset();
	motor_speed(140, 140);
	motorDirection(MOT_FWD, MOT_REV);
	setStatusLED(RED);
	_delay_ms(50);
	motor_speed(80, 80);
	while(encoder_sum <= _angle)
	{
		encoder_sum = abs(motorStatus.encoder[LEFT]) + abs(motorStatus.encoder[RIGHT]);
	}
	
	setStatusLED(GREEN);
	motorDirection(MOT_BREAK, MOT_BREAK);
	motor_speed(0, 0);
	
	disableDevice(EN_ENCODERS);
	
}

void parserRotate(void)
{
	char buffer[5];
	uint16_t temp;
	uint8_t index = 0;
	bool rxComplete = false;
	
	uart_puts_P("\nSet Motor Gain: ");
	
	while (!rxComplete)
	{
		temp = uart_getc();
		if (temp != UART_NO_DATA)
		{
			buffer[index] = (char) temp;
			if (buffer[index] == '\n')
			{
				buffer[++index] = '\0';
				rxComplete = true;
			}
			index++;
		}
	}
	uart_puts(buffer);
	motorStatus.rotateGain = atof(buffer);
	
	uart_puts_P("Set Motor Offset: ");
	rxComplete = false;
	index = 0;
	
	while (!rxComplete)
	{
		temp = uart_getc();
		if (temp != UART_NO_DATA)
		{
			buffer[index] = (char) temp;
			if (buffer[index] == '\n')
			{
				buffer[++index] = '\0';
				rxComplete = true;
			}
			index++;
		}
	}
	uart_puts(buffer);
	motorStatus.rotateOffset = atoi(buffer);
	
	uart_puts_P("Set Angle: ");
	rxComplete = false;
	index = 0;
	
	while (!rxComplete)
	{
		temp = uart_getc();
		if (temp != UART_NO_DATA)
		{
			buffer[index] = (char) temp;
			if (buffer[index] == '\n')
			{
				buffer[++index] = '\0';
				rxComplete = true;
			}
			index++;
		}
	}
	uart_puts(buffer);
	angle = atoi(buffer);
	
}

void rotateTimerTest(void)
{
	rotate(LEFT, ROTATE_90, BLOCKING);
	_delay_ms(1000);
	rotate(LEFT, ROTATE_90, BLOCKING);
	_delay_ms(1000);
	rotate(LEFT, ROTATE_90, BLOCKING);
	_delay_ms(1000);
	rotate(LEFT, ROTATE_90, BLOCKING);
	_delay_ms(1000);
	
	rotate(RIGHT, ROTATE_90, BLOCKING);
	_delay_ms(1000);
	rotate(RIGHT, ROTATE_90, BLOCKING);
	_delay_ms(1000);
	rotate(RIGHT, ROTATE_90, BLOCKING);
	_delay_ms(1000);
	rotate(RIGHT, ROTATE_90, BLOCKING);
	_delay_ms(1000);
	
	rotate(LEFT, ROTATE_180, BLOCKING);
	_delay_ms(1000);
	rotate(RIGHT, ROTATE_180, BLOCKING);
	_delay_ms(1000);
	
}

void driveTest(void)
{
	enableDevice(EN_ENCODERS);
	int32_t L_speed, R_speed, L_distance, R_distance;
	int32_t deltaTime;
	static int32_t deltaTime_Last = 0;
	static int16_t L_distanceLast, R_distanceLast;
	
	char buffer[20];
	
	motorDirection(MOT_FWD, MOT_FWD);
	motor_speed(MOTOR_SPEED, MOTOR_SPEED);
	
	/************************************************************************/
	/* Zurück gelegte Entfernung                                             */
	/************************************************************************/
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		L_distance = motorStatus.encoder[LEFT];
		R_distance = motorStatus.encoder[RIGHT];
	}
	deltaTime = get_time_ms();
	
	L_distance = (L_distance * 15) /2;
	R_distance = (R_distance * 15) /2;
	
	uart_puts_P("\n\nLeft Distance: ");
	itoa(L_distance/10, buffer, 10);
	uart_puts(buffer);
	uart_puts("cm");
	uart_puts_P("\nRight Distance: ");
	itoa(R_distance/10, buffer, 10);
	uart_puts(buffer);
	uart_puts("cm");
	
	/************************************************************************/
	/* Aktuelle Geschwindigkeit                                             */
	/************************************************************************/
	L_speed = ((L_distance-L_distanceLast)*100) / (deltaTime-deltaTime_Last);
	uart_puts_P("\nLeft Speed: ");
	itoa(L_speed, buffer, 10);
	uart_puts(buffer);
	uart_puts("cm/s");
	R_speed = ((R_distance-R_distanceLast)*100) / (deltaTime-deltaTime_Last);
	uart_puts_P("\nRight Speed: ");
	itoa(R_speed, buffer, 10);
	uart_puts(buffer);
	uart_puts("cm/s");
	
	itoa(systemStatus.battery, buffer, 10);
	uart_puts_P("\nU_Batt: ");
	uart_puts(buffer);
	
	L_distanceLast = L_distance;
	R_distanceLast = R_distance;
	deltaTime_Last = deltaTime;
	
	if (userButton())
	{
		encoderReset();
	}
}

void encoderTest(void)
{
	enableDevice(EN_ENCODERS);
	
	setStatusLED(RED);
	
	char buffer[20];
	motorDirection(MOT_FWD, MOT_FWD);
	motor_speed(MOTOR_SPEED, MOTOR_SPEED);
	uart_puts_P("\n\nEncoder L: ");
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		itoa(motorStatus.encoder[LEFT], buffer, 10);
	}
	uart_puts(buffer);
	
	uart_puts_P("\nEncoder R: ");
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		itoa(motorStatus.encoder[RIGHT], buffer, 10);
	}
	uart_puts(buffer);
	
	uart_puts_P("\nU_Batt: ");
	uart_putInt(systemStatus.battery);
	
	if (userButton())
	{
		encoderReset();
	}
}

void buttonTest(void)
{
	if (startButton())
	setStatusLED(GREEN);
	else if(userButton())
	setStatusLED(RED);
	else
	setStatusLED(OFF);
}



void lineTest(void)
{
	int16_t center;
	
	lineRead();
	
	uart_puts_P("\nAnalogeSensordaten:\n");
	LOG_DEBUG("LINE Left: %d", lineSensors.analogData[0]);
	LOG_DEBUG("LINE CLeft: %d", lineSensors.analogData[1]);
	LOG_DEBUG("LINE Center: %d", lineSensors.analogData[2]);
	LOG_DEBUG("LINE CRight: %d", lineSensors.analogData[3]);
	LOG_DEBUG("LINE Right: %d", lineSensors.analogData[4]);
	
	uart_puts_P("\nKalibrierdaten:\n");
	LOG_DEBUG("LINE Left: %d", lineSensors.calibData[0]);
	LOG_DEBUG("LINE CLeft: %d", lineSensors.calibData[1]);
	LOG_DEBUG("LINE Center: %d", lineSensors.calibData[2]);
	LOG_DEBUG("LINE CRight: %d", lineSensors.calibData[3]);
	LOG_DEBUG("LINE Right: %d", lineSensors.calibData[4]);
	
	center = followTrack();
	LOG_DEBUG("Center: %d\n", center);
	
	LOG_DEBUG("Digitale Sensordaten:");
	for(uint8_t i=0; i<5; i++)
	{
		uart_putInt(lineSensors.digitalData[i]);
	}
	
	_delay_ms(1000);
}


void motorTest(void)
{
	static bool ledFlag = false;
	//uart_puts_P("\nMotortest rotate Left NO_BLOCKING Mode");
	
	startStopwatch1();
	
	if( (motorStatus.autoDrive) && (getStopwatch(STOPWATCH1) > 100))
	{
		
		ledFlag ^= true;
		setStopwatch(STOPWATCH1, 0);
		
		if (ledFlag)
		setStatusLED(RED);
		else
		setStatusLED(OFF);
	}
	
	uart_puts_P("\nMotor Test Blocking Mode");
	motor_speed(MOTOR_SPEED,MOTOR_SPEED);
	
	motorDirection(MOT_FWD, MOT_FWD);
	setStatusLED(GREEN);
	//_delay_ms(2000);
	//motorDirection(MOT_REV, MOT_BREAK);
	//setStatusLED(RED);
	//_delay_ms(2000);
	//
	//motorDirection(MOT_BREAK, MOT_BREAK);
	//setStatusLED(YELLOW);
	//_delay_ms(2000);
	//
	//motorDirection(MOT_BREAK, MOT_FWD);
	//setStatusLED(GREEN);
	//_delay_ms(2000);
	//motorDirection(MOT_BREAK, MOT_REV);
	//setStatusLED(RED);
	//_delay_ms(2000);
	//motorDirection(MOT_BREAK, MOT_BREAK);
	//setStatusLED(YELLOW);
	//_delay_ms(2000);
}

uint16_t distance = 0;

void parserDistance(void)
{
	char buffer[10];
	uint8_t _distance;
	uint16_t temp;
	uint8_t index = 0;
	bool rxComplete = false;
	
	uart_puts_P("\nSet Distance: ");
	
	while (!rxComplete)
	{
		temp = uart_getc();
		if (temp != UART_NO_DATA)
		{
			buffer[index] = (char) temp;
			if (buffer[index] == '\n')
			{
				buffer[++index] = '\0';
				rxComplete = true;
			}
			index++;
		}
	}
	uart_puts(buffer);
	_distance = atoi(buffer);
	
	distance = _distance;
}

void testDistance(void)
{
	
	
	if (userButton())
	{
		taskID_2 = schedulerAddTask(parserDistance, 0 ,0);
	}
	
	if (startButton())
	{
		enableDevice(EN_ENCODERS);
		setStatusLED(YELLOW);
		_delay_ms(2000);
		setStatusLED(RED);
		encoderReset();
		motor_speed(MOTOR_SPEED, MOTOR_SPEED);
		motorDirection(MOT_FWD, MOT_FWD);
		
		uint16_t encSum = 0;
		
		while ( encSum <= distance )
		{
			encSum = motorStatus.encoder[ENCODER_LEFT] + motorStatus.encoder[ENCODER_RIGHT];
			taskSystem();
		}
		motorDirection(MOT_BREAK, MOT_BREAK);
		disableDevice(EN_ENCODERS);
		setStatusLED(GREEN);
		_delay_ms(800);
	}
}



void testLogging(void)
{
	static int16_t i,j = 0;
	
	LOG_DEBUG("Akku: %dmV", batt_voltage());
	LOG_WARNING("Achtung");
	LOG_ERROR("2 Argumente %i in diesem String: %i", i++, j--);
	LOG_FATAL("Schlimmer Fehler ;)");
	LOG_INFO("Allgemeine Information");
}
