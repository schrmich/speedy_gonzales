/*
 * maze.c
 *
 * Created: 12.06.2019 20:02:40
 *  Author: mschr
 */ 

#include "maze.h"
#include <stdio.h>

pidData_t pidData;
bool drive = false;
//uint8_t LinePattern = 00010101;

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

void printPattern(uint8_t data)
{
	char buffer[5];
	uart_puts_P("\nPattern: ");
	itoa(data, buffer, 2);
	buffer[5] = '\0';
	uart_puts(buffer);
}


intersection_t validateIntersection(uint8_t initial, uint8_t second)
{
	intersection_t actualIntersection;
	
	// Auswertung Initial-Scan
	// Prüfen auf seitliche Wege
	if ( (initial &  P_LEFT) && (initial & P_RIGHT) )
		actualIntersection = I_T_CROSS;
	else if ( initial & P_LEFT)
		actualIntersection = I_LEFT;
	else if ( initial & P_RIGHT)
		actualIntersection = I_RIGHT;
	else 
		actualIntersection = I_DEAD_END;
	
	// Auswertung sekundärer Scan
	// Prüfung auf Weg in Fahrtrichtung
	if (actualIntersection==I_T_CROSS && (second & P_FRONT))
		actualIntersection = I_FOUR_WAY;
	else if (actualIntersection==I_LEFT && (second & P_FRONT))
		actualIntersection = I_STRAIGHT_LEFT;
	else if (actualIntersection==I_RIGHT && (second & P_FRONT))
		actualIntersection = I_STRAIGHT_RIGHT;
	
	//if ( (initial == P_END) && (second == P_END) )
		//actualIntersection = I_END;
	
	return actualIntersection;
}


void printIntersection(intersection_t intersection)
{
	char buffer[20];
	uart_puts_P("\nIntersection: ");
	switch (intersection)
	{
		case I_DEAD_END:
		sprintf_P(buffer, PSTR("Dead End"));
		break;
		
		case I_END:
		sprintf_P(buffer, PSTR("Finish"));
		break;
		
		case I_FOUR_WAY:
		sprintf_P(buffer, PSTR("Four Way"));
		break;
		
		case I_LEFT:
		sprintf_P(buffer, PSTR("Left"));
		break;
		
		case I_RIGHT:
		sprintf_P(buffer, PSTR("Right"));
		break;
		
		case I_T_CROSS:
		sprintf_P(buffer, PSTR("T-Way"));
		break;
		
		case I_STRAIGHT_LEFT:
		sprintf_P(buffer, PSTR("Straight Left"));
		break;
		
		case I_STRAIGHT_RIGHT:
		sprintf_P(buffer, PSTR("Straight Right"));
		break;
		
		default:
		sprintf_P(buffer, PSTR("Unknown"));
		break;
	}
	
	uart_puts(buffer);
}