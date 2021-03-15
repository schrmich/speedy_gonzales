/*
 * line_sensors.c
 *
 * Created: 23.11.2017 18:33:10
 *  Author: mschr
 */ 

#include "line_sensors.h"
#include "system.h"
#include <util/delay.h>
#include <stdlib.h>
#include <avr/eeprom.h>



int16_t lineCalib_ee[5] EEMEM;

static void line_pwr(uint8_t pwr)
{
	if (pwr == ON)
	{
		PORTC |= (1<<PINC7);
		_delay_us(LINE_SENSOR_DELAY);
	}
	else
	{
		PORTC &= ~(1<<PINC7);
	}
}


void lineRead(void)
{
	uint16_t temp;
	
	line_pwr(ON);
	for (uint8_t i = 0; i < 5; i++)
	{
		lineSensors.analogData[i] = adc_Read(i+ADC3D);
	}
	line_pwr(OFF);
	
	temp = lineSensors.analogData[LINE_RIGHT];
	lineSensors.analogData[LINE_RIGHT] = lineSensors.analogData[LINE_CRIGHT];
	lineSensors.analogData[LINE_CRIGHT] = temp;
	
	if (lineSensors.calibrated)
	{
		for(uint8_t i=0; i<5; i++)
		{
			if ( (lineSensors.analogData[i] + LINE_TRESHOLD) > lineSensors.calibData[i] )	lineSensors.digitalData[i] = true;
			else	lineSensors.digitalData[i] = false;
		}
	}
}

int16_t followTrack()
{
	int16_t center;
	
	center = lineSensors.analogData[LINE_CLEFT] - lineSensors.analogData[LINE_CRIGHT];
	//center = 1*lineSensors.analogData[LINE_LEFT] + 2*lineSensors.analogData[LINE_CLEFT] + 3*lineSensors.analogData[LINE_CENTER] + 4*lineSensors.analogData[LINE_CRIGHT] + 5*lineSensors.analogData[LINE_RIGHT];
	center = center >> 2;
	
	return center;
}

void lineInit(void)
{
	lineSensors.byte = 0;
	
	eeprom_read_block(lineSensors.calibData, lineCalib_ee, sizeof(lineSensors.calibData));
	
	lineSensors.calibrated = true;
}

void calib_rotate(uint8_t direction, uint16_t angle)
{
	
	motor_speed(TURN_SPEED_CALIBRATE, TURN_SPEED_CALIBRATE);
	

	if (direction == LEFT)	motorDirection(MOT_REV, MOT_FWD);
	else if (direction == RIGHT) motorDirection(MOT_FWD, MOT_REV);
		
	motorStatus.autoDrive= true;
	setStopwatch(STOPWATCH8,0);
	startStopwatch8();
	motorStatus.timer = angle;
	
}

void calibrateSensors(void)
{
	uint16_t lineMax[5], lineMin[5];
	lineSensors.calibrated = false;
	
	setStatusLED(RED);
	
	lineRead();
	
	for (uint8_t i=0; i<5; i++)
	{
		lineMin[i] = lineSensors.analogData[i];
		lineMax[i] = lineSensors.analogData[i];
	}
	
	calib_rotate(LEFT, CALIB_ROTATE_90);
	
	while(motorStatus.autoDrive)
	{
		lineRead();
		
		for (uint8_t i=0; i<5; i++)
		{
			if (lineMin[i] > lineSensors.analogData[i])	lineMin[i] = lineSensors.analogData[i];
			if (lineMax[i] < lineSensors.analogData[i])	lineMax[i] = lineSensors.analogData[i];
		}
		
		taskSystem();
	}
	
	_delay_ms(200);
	
	calib_rotate(RIGHT, CALIB_ROTATE_180);
	
	while(motorStatus.autoDrive)
	{
		lineRead();
		for (uint8_t i=0; i<5; i++)
		{
			if (lineMin[i] > lineSensors.analogData[i])	lineMin[i] = lineSensors.analogData[i];
			if (lineMax[i] < lineSensors.analogData[i])	lineMax[i] = lineSensors.analogData[i];
		}
		taskSystem();
	}
	
	_delay_ms(200);
	
	calib_rotate(LEFT, CALIB_ROTATE_90);
	
	while(motorStatus.autoDrive)
	{
		lineRead();
		for (uint8_t i=0; i<5; i++)
		{
			if (lineMin[i] > lineSensors.analogData[i])	lineMin[i] = lineSensors.analogData[i];
			if (lineMax[i] < lineSensors.analogData[i])	lineMax[i] = lineSensors.analogData[i];
		}
		taskSystem();
	}
	
	
	setStatusLED(OFF);
	
	LOG_INFO("Kalibration Data\n");
	for (uint8_t i=0; i<5; i++)
	{
		lineSensors.calibData[i] = (lineMax[i] + lineMin[i]) / 2;
		LOG_INFO("%i: %i", i, lineSensors.calibData[i]);
	}
	
	_delay_ms(1000);
	
	eeprom_write_block(lineSensors.calibData, lineCalib_ee, sizeof(lineCalib_ee));
	
	
	lineSensors.calibrated = true;
}


