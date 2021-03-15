/*
 * lcd_shield.h
 *
 * Created: 28.04.2020 20:43:41
 *  Author: Michael
 */ 


#ifndef LCD_SHIELD_H_
#define LCD_SHIELD_H_

#include <stdint.h>
#include "nokia5110.h"


#define PCF8574_ADRESS	0x70

#define CHAR_WIDHT	6
#define CHAR_HIGHT	8

#define LED_BLUE	1
#define LED_RED		0
#define LED_PURPLE	2

union{
	uint8_t byte;
	struct{
		unsigned select:1;
		unsigned start:1;
		unsigned up:1;
		unsigned down:1;
		unsigned led_bl:1;
		unsigned led_rt:1;
		unsigned reserve:2;
	};
}PCF8574_data;

extern int8_t menuPointer;

void shieldInit(void);

void dispSetCusor(uint8_t x, uint8_t y, uint8_t scale);

void readSwitch(void);

void writeLeds(uint8_t led);

void dispStatus(void);

void dispSensors(void);


#endif /* LCD_SHIELD_H_ */