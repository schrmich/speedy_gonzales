/*
 * line_sensors.h
 *
 * Created: 23.05.2019 20:43:58
 *  Author: mschr
 */ 


#ifndef LINE_SENSORS_H_
#define LINE_SENSORS_H_

#include <stdint.h>
#include <stdbool.h>

#define LINE_LEFT	0
#define LINE_CLEFT	1
#define LINE_CENTER 2
#define LINE_CRIGHT 3
#define LINE_RIGHT	4

#define LINE_SENSOR_DELAY 800	// Verzögerung zwischen Emitter Ein und Messung in µs

#define CALIB_ROTATE_90 800
#define CALIB_ROTATE_180 1600

//#define LINE_TRESHOLD 160
#define LINE_TRESHOLD 200
#define TURN_SPEED_CALIBRATE 50

struct{
	union {
		uint8_t byte;
		struct
		{
			uint8_t calibrated:1;
			uint8_t enabled:1;
			uint8_t reserve:6;
		};
	};
	int16_t analogData[5];
	int16_t calibData[5];
	bool digitalData[5];
}lineSensors;

//extern uint16_t lineOffset[5] EEMEM;

void lineRead(void);

/************************************************************************/
/* Berechnet aus den zwei Center-Left und Center-Right Sensoren			*/ 
/* die Differenz. Wenn der Rückgabewert kleiner als 0 dann weicht der	*/
/* Roboter links von der Linie ab. Wenn der Rückgabewert größer als 0	*/
/* dann weicht der Roboter rechts von der Linie ab. Kann als			*/
/* Eingangsgröße eines PID Reglers verwendet werden                     */
/************************************************************************/
int16_t followTrack(void);

void lineInit(void);

void calibrateSensors(void);


#endif /* LINE_SENSORS_H_ */