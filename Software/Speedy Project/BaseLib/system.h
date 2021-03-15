/*
 * system.h
 *
 * Created: 11.10.2020 19:06:54
 *  Author: mschr
 */ 


#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef UART_BAUD_RATE
#define UART_BAUD_RATE	115200UL
#endif

#define OFF 10
#define ON	16

#define LEFT	 0
#define RIGHT	 1
#define AHEAD	 2
#define BACKWARD 3

#define LED_BOTTOM	PINC6
#define setLEDBlue()	(PORTC |= (1<<LED_BOTTOM))
#define clearLEDBlue()	(PORTC &= ~(1<<LED_BOTTOM))

// Beispiel:	setStatusLED(YELLOW);
//				setStatusLED(OFF);
#define GREEN	0
#define RED		1
#define YELLOW	3

#define startButton()	!( PINB & (1<<PINB4) )
#define userButton()	!( PINB & (1<<PINB3) )

#define ENCODER_LEFT 0
#define ENCODER_RIGHT 1

#define myprintf_P(_format, ...)	printf_P(PSTR(_format), ## __VA_ARGS__)






//#define EN_LINE			0
#define EN_BLUETOOTH	1
#define EN_ENCODERS		3
#define EN_ALL			0xFF


#define V_BATT_MIN 6800
#define V_REF	5000


class BaseSystem
{
	private:
	
	public:
	
	/*!
	@brief	Systemwerte und Flags von Speedy
	*/
	struct{	
		union {
			uint8_t byte;
			struct
			{
				uint8_t pwrON:1;
				uint8_t bottomLeds:1;
				uint8_t bluetooth:1;
				uint8_t lowBatt:1;
				uint8_t encoders:1;
				uint8_t error:1;
				uint8_t reserve:2;
			};
		};
			uint16_t battery;
			uint32_t time;
	}status;
	
	/************************************************************************/
	/* Wartet auf drücken des Start Knopfes und Status LED blinkt grün.     */
	/************************************************************************/
	void waitStart(void);

	/************************************************************************/
	/*  Initialisiert die Hardware (Ports, ADC, PWM)						*/
	/*	Diese Funktion muss als erstes aufgerufen werden.                   */
	/************************************************************************/
	BaseSystem();

	/*************************************************************************
	* ADC Einzelmessung                                                      *
	*************************************************************************/
	uint16_t adc_Read( uint8_t channel );

	/*************************************************************************
	* @brief	ADC Mehrfachmessung mit Mittelwertbbildung					 *
	* beachte: Wertebereich der Rückgabevariable                             *
	*************************************************************************/
	uint16_t adc_Read_Avg( uint8_t channel, uint8_t nsamples );

	/************************************************************************/
	/* Gibt die Systemzeit seit dem Start in Millisekunden zurück           */
	/************************************************************************/
	uint32_t get_time_ms(void);

	/************************************************************************/
	/* Gibt die Systemzeit seit dem Start in Sekunden zurück		        */
	/************************************************************************/
	uint16_t get_time_s(void);

	/************************************************************************/
	/* Systemzeit wird zurück gesetzt.                                      */
	/************************************************************************/
	void time_reset(void);

	/************************************************************************/
	/* Batteriespannung in mV                                               */
	/************************************************************************/
	uint16_t batt_voltage(void);

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	void enableDevice(uint8_t device);
	
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	void disableDevice(uint8_t device);

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	void setStatusLED(uint8_t led);
	
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	void taskSystem(void);
	
};


#endif /* SYSTEM_H_ */