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
char pathMemory[PATH_MEM_SIZE];
uint8_t pathIndex;

const char optimize1[] PROGMEM ="LUL";	// 'S'
const char optimize2[] PROGMEM ="SUL";	// 'R'
const char optimize3[] PROGMEM ="SUS";	// 'U'
const char optimize4[] PROGMEM ="RUL";	// 'U'
const char optimize5[] PROGMEM ="LUS";	// 'R'
const char optimize6[] PROGMEM ="LUR";	// 'U'
const char optimize7[] PROGMEM ="RUS";	// 'L'
const char optimize8[] PROGMEM ="RUR";	// 'S'


//uint8_t LinePattern = 00010101;

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
	
	if ( (initial == P_END) && (second == P_END) )
		actualIntersection = I_END;

	return actualIntersection;
}


char *printIntersection(char *interType, intersection_t intersection)
{
	switch (intersection)
	{
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


char mazeSearch(intersection_t intersection)
{
	char actualPath;
	
	switch(intersection)
	{
		case I_DEAD_END:
		actualPath = U_TURN;
		break;
		
		case I_END:
		actualPath = FINISH;
		break;
		
		case  I_STRAIGHT_LEFT:
		actualPath = LEFT_TURN;
		break;
		
		case I_STRAIGHT_RIGHT:
		actualPath = STRAIGHT;
		break;
		
		case  I_T_CROSS:
		actualPath = LEFT_TURN;
		break;
		
		case I_FOUR_WAY:
		actualPath = LEFT_TURN;
		break;
		
		case  I_LEFT:
		actualPath = LEFT_TURN;
		break;
		
		case I_RIGHT:
		actualPath = RIGHT_TURN;
		break;
		
		default:
		actualPath = LINE_LOST;
		break;
	}
	
	
	return actualPath;
}

bool replaceString(char *source, char *search, char replace, int lenght)
{
	char tempString[lenght];
	char *ptr;
	
	ptr = strstr(source, search);
	if (ptr == NULL)
	return false;
	strcpy(tempString, ptr+3);
	*ptr = replace;
	*(++ptr) = '\0';
	strcat(source, tempString);
	return true;
}

void mazeOptimize(char *path)
{
	char strBuffer[5];
	int length;
	length = strlen(path);
	bool finish = true;
	
	while(finish)
	{
		finish = false;
		finish |= replaceString(path, memcpy_P(strBuffer, optimize1, 4), 'S', length);
		finish |= replaceString(path, memcpy_P(strBuffer, optimize2, 4), 'R', length);
		finish |= replaceString(path, memcpy_P(strBuffer, optimize3, 4), 'U', length);
		finish |= replaceString(path, memcpy_P(strBuffer, optimize4, 4), 'U', length);
		finish |= replaceString(path, memcpy_P(strBuffer, optimize5, 4), 'R', length);
		finish |= replaceString(path, memcpy_P(strBuffer, optimize6, 4), 'U', length);
		finish |= replaceString(path, memcpy_P(strBuffer, optimize7, 4), 'L', length);
		finish |= replaceString(path, memcpy_P(strBuffer, optimize8, 4), 'S', length);
	}
}

bool pathCompare(char storedPath, intersection_t actualIntersection)
{	
	switch(storedPath)
	{
		case LEFT_TURN:
		if (	(actualIntersection == I_LEFT)		||
				(actualIntersection == I_FOUR_WAY)	|| 
				(actualIntersection == I_T_CROSS)	|| 
				(actualIntersection == I_STRAIGHT_LEFT) )
			return true;
		break;
		
		case RIGHT_TURN:
		if (	(actualIntersection == I_RIGHT)		||
				(actualIntersection == I_FOUR_WAY)	||
				(actualIntersection == I_T_CROSS)	||
				(actualIntersection == I_STRAIGHT_RIGHT) )
			return true;
		break;
		
		case STRAIGHT:
		if (	(actualIntersection == I_FOUR_WAY) ||
				(actualIntersection == I_STRAIGHT_RIGHT) ||
				(actualIntersection == I_STRAIGHT_LEFT) )
			return true;
		break;
		
		case U_TURN:
			return true;
		break;
		
		default:
			return false;
		break;
	}
	
	return false;
}