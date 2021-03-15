
#include "speedy.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>


//CPU: 16 Mhz, Prescale: 256
const char PROGMEM frequencies[48] = {
	// C 	 C# 	 D 	 D# 	 E 	 F 	 F# 	 G 	 G# 	 A 	 A# 	 B
	118,	 112,	 105,	 99,	 94,	 88,	 83,	 79,	 74,	 70,	 66,	 62,
	59,	 55,	 52,	 49,	 46,	 44,	 41,	 39,	 37,	 35,	 33,	 31,
	29,	 27,	 26,	 24,	 23,	 21,	 20,	 19,	 18,	 17,	 16,	 15,
	14,	 13,	 12,	 12,	 11,	 10,	 10,	 9,	 8,	 8,	 7,	 7
};


/*Beispiel #defines müssen angepasst werden! */
#define TON_AN    TCCR1B |= (1<<CS12) //Prescale: 256
#define TON_AUS	  TCCR1B &= ~(1<<CS12)
#define TON       OCR1A

void soundInit(void)
{
	TCCR1A |= (1<<COM1A0);
	TCCR1B |= (1<<WGM12);
	OCR1A = 0;
	//ICR1 = 0xff;
	TON_AUS;
}

void playSound(const char *melody)
{
	
	uint16_t index=0;
	uint8_t default_octave= pgm_read_byte(&melody[index++])&0x0F;
	uint16_t bpm=60000/pgm_read_byte(&melody[index++]);
	
	while(pgm_read_byte(&melody[index]) != 0)
	{
		
		
		//Note laden
		uint8_t note=pgm_read_byte(&melody[index++]);
		uint16_t duration=bpm>>(note>>5);
		if(note&0x10) duration=(duration*3)/2;
		uint8_t octave=default_octave;
		if((note&0x0F)==0x0E)
		{
			note=pgm_read_byte(&melody[index++]);
			octave=note>>4;
		}
		if((note&0x0F)!=0x0F)
		{
			TCNT1 = 0;
			TON  = pgm_read_byte(&frequencies[octave*12+(note&0x0F)-1]);
			//LOG_DEBUG("%2iNote:%i", index, TON);
			TON_AN;
		}
		while(--duration){
			_delay_ms(4);
		}
		TON_AUS;
		_delay_ms(4);
	}
	
}