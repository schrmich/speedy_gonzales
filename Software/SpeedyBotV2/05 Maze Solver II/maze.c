/*
 * maze.c
 *
 * Created: 12.06.2019 20:02:40
 *  Author: mschr
 */ 

#include "maze.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


pidData_t pidData;

uint16_t dezToBin(uint16_t dezimal)
{
	uint16_t rest, binaer=0, faktor=1;

	while (dezimal)
	{
		rest = dezimal % 2;
		dezimal = dezimal / 2;
		
		binaer = binaer + rest * faktor;
		faktor *= 10;
	}

	return binaer;
}

uint8_t patternRead(void)
{
	uint8_t pattern = 0;
	for (uint8_t i=0; i<5; i++)
	{
		if (lineSensors.digitalData[i])
			pattern |= (1<<i);
	}
	
	return pattern;
}

intersection_t validateIntersection(uint8_t initial, uint8_t second)
{
	intersection_t actualIntersection = I_ERROR;
	
	// Auswertung Initial-Scan
	// Prüfen auf seitliche Wege
	if ( (initial &  P_LEFT) && (initial & P_RIGHT) )
		actualIntersection = I_T_CROSS;
	else if ( initial & P_LEFT)
		actualIntersection = I_LEFT;
	else if ( initial & P_RIGHT)
		actualIntersection = I_RIGHT;
	else if ( initial & P_DEAD)
		actualIntersection = I_DEAD_END;
	else if ( initial & P_CENTER)
		actualIntersection = I_STRAIGHT;
	
	// Auswertung sekundärer Scan
	// Prüfung auf Weg in Fahrtrichtung
	if (actualIntersection==I_T_CROSS && (second & P_FRONT))
		actualIntersection = I_FOUR_WAY;
	else if (actualIntersection==I_LEFT && (second & P_FRONT))
		actualIntersection = I_STRAIGHT_LEFT;
	else if (actualIntersection==I_RIGHT && (second & P_FRONT))
		actualIntersection = I_STRAIGHT_RIGHT;
	else if (actualIntersection == I_STRAIGHT && (second & P_CENTER))
		actualIntersection = I_STRAIGHT;
		
	if ( (initial == P_END) && (second == P_END) )
		actualIntersection = I_END;

	return actualIntersection;
}


char *printIntersection(char *interType, intersection_t intersection)
{
	switch (intersection)
	{
		case I_START:
		sprintf_P(interType, PSTR("I-Start"));
		break;
		
		case I_DEAD_END:
		sprintf_P(interType, PSTR("Dead End"));
		break;
		
		case I_END:
		sprintf_P(interType, PSTR("Finish"));
		break;
		
		case I_FOUR_WAY:
		sprintf_P(interType, PSTR("Four Way"));
		break;
		
		case I_LEFT:
		sprintf_P(interType, PSTR("Left"));
		break;
		
		case I_RIGHT:
		sprintf_P(interType, PSTR("Right"));
		break;
		
		case I_T_CROSS:
		sprintf_P(interType, PSTR("T-Way"));
		break;
		
		case I_STRAIGHT_LEFT:
		sprintf_P(interType, PSTR("Straight Left"));
		break;
		
		case I_STRAIGHT_RIGHT:
		sprintf_P(interType, PSTR("Straight Right"));
		break;
		
		default:
		sprintf_P(interType, PSTR("Unknown"));
		break;
	}
	
	return interType;
}


