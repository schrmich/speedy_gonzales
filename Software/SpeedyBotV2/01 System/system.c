/*
 * system.c
 *
 * Created: 23.11.2017 12:46:47
 *  Author: schreiberm
 */ 


#include "speedy.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <util/atomic.h>
#include <avr/wdt.h>
#include <stdlib.h>


#define V_BATT ADC2D

volatile stopwatches_t stopwatches;


void waitStart(void)
{
	bool toogle = false;
	
	startStopwatch1();
	
	while(!startButton())
	{
		if (getStopwatch(STOPWATCH1) > 500)
		{
			toogle ^= true;
			
			if (toogle)
				setStatusLED(GREEN);
			else
				setStatusLED(OFF);
			setStopwatch(STOPWATCH1, 0);
		}
	}
	
	systemStatus.pwrON = true;
	setStopwatch(STOPWATCH1, 0);;
	stopStopwatch1();
	setStatusLED(OFF);
}

/* ADC Einzelmessung */
uint16_t adc_Read( uint8_t channel )
{
	// Kanal waehlen, ohne andere Bits zu beeinfluﬂen
	ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);
	ADCSRA |= (1<<ADSC);            // eine Wandlung "single conversion"
	while (ADCSRA & (1<<ADSC) ) {   // auf Abschluss der Konvertierung warten
	}
	return ADCW;       // ADC auslesen und zur¸ckgeben
}

/* ADC Mehrfachmessung mit Mittelwertbbildung */
/* beachte: Wertebereich der Summenvariablen */
uint16_t adc_Read_Avg( uint8_t channel, uint8_t nsamples )
{
	uint32_t sum = 0;

	for (uint8_t i = 0; i < nsamples; ++i ) {
		sum += adc_Read( channel );
	}

	return (uint16_t)( sum / nsamples );
}


uint16_t batt_voltage(void)
{
	uint32_t adc_raw;
	
	adc_raw = adc_Read(V_BATT);
	
	adc_raw = adc_raw << 1;
	
	adc_raw = (adc_raw * V_REF) / 1024;
	
	return (uint16_t)adc_raw;
}

void setStatusLED(uint8_t led)
{
	// Erstmal beide Leds lˆschen
	PORTB |= (1<<GREEN) | (1<<RED);
	// Setze leds
	if (led == RED)
		PORTB &= ~(1<<RED);
	else if (led == GREEN)
		PORTB &= ~(1<<GREEN);
	else if (led == YELLOW)
		PORTB &= ~( (1<<GREEN) | (1<<RED) );	
}

void enableDevice(uint8_t device)
{
	switch (device)
	{
	case EN_ALL:
		PORTB &= ~(1<<PINB2);	// Encoder
		PORTC &= ~(1<<PINC5);	// Bluetooth
		systemStatus.bluetooth = true;
		systemStatus.encoders = true;
		break;
	case EN_ENCODERS:
		PORTB &= ~(1<<PINB2);				// Encoder
		systemStatus.encoders = true;
		break;
	case EN_BLUETOOTH:
		PORTC &= ~(1<<PINC5);				// Bluetooth
		systemStatus.bluetooth = true;
		break;
	}
	
}

void disableDevice(uint8_t device)
{
	switch (device)
	{
	case EN_ALL:
		PORTB |= (1<<PINB2);	// Encoder
		PORTC |= (1<<PINC5);	// Bluetooth
		systemStatus.bluetooth = false;
		systemStatus.encoders = false;
		break;
	case EN_ENCODERS:
		PORTB |= (1<<PINB2);				// Encoder
		systemStatus.encoders = false;
		break;
	case EN_BLUETOOTH:
		PORTC |= (1<<PINC5);				// Bluetooth
		systemStatus.bluetooth = false;
		break;
	}
}


uint32_t get_time_ms(void)
{
	uint32_t temp;
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		temp = systemStatus.time;
	}
	return temp;
}

uint16_t get_time_s(void)
{
	uint32_t temp;
	temp = get_time_ms() / 1000;
	return (uint16_t)temp;
}

void time_reset(void)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		systemStatus.time = 0;
	}
}


void uart_putInt(int16_t number)
{
	char buffer[10];
	
	itoa(number, buffer, 10);
	uart_puts(buffer);
}


// -----------------------------------------------------------------
// Battery Monitor State changed handler:
void battery_update_DUMMY(void){};
static void (*battery_stateChangedHandler)(void) = battery_update_DUMMY;
/**
 * Use this function to set the Low-Battery state change handler. 
 * 
 */
void battery_setStateChangedHandler(void (*batteryHandler)(void)) 
{
	battery_stateChangedHandler = batteryHandler;
}

void taskBattery(void)
{
	// Automatische ‹berpr¸fung der LiPo-Akkuspannung jede Sekunde
	// um zu verhindern das der Akku aus versehen tiefentladen werden kann.
	
	systemStatus.battery = batt_voltage();
			
	if (systemStatus.battery < V_BATT_MIN)
	{
		systemStatus.lowBatt = true;
	}
	
	if (systemStatus.lowBatt)
	{
		battery_stateChangedHandler();
		motorDirection(MOT_BREAK, MOT_BREAK);
		disableDevice(EN_ENCODERS);
		clearLEDBlue();
		systemStatus.pwrON = false;
		setStatusLED(OFF);
		LOG_ERROR("Low Battery:%i", batt_voltage());
		
		for (uint8_t i=0; i<20; i++)
		{
			setStatusLED(RED);
			_delay_ms(200);
			setStatusLED(OFF);
			_delay_ms(200);
		}
		
		disableDevice(EN_ALL);
		setStatusLED(OFF);
		
		set_sleep_mode (SLEEP_MODE_PWR_DOWN);
		while(1) 
		{
			sleep_mode();
		}
	}
}
// ------------------------------------------------------------------------


// -----------------------------------------------------------------
// Drive State changed handler:
void drive_update_DUMMY(void){}
static void (*drive_stateChangedHandler)(void) = drive_update_DUMMY;
/**
 * Use this function to set the drive state change handler. 
 * 
 */
void drive_setStateChangedHandler(void (*driveHandler)(void)) 
{
	drive_stateChangedHandler = driveHandler;
}

// Wenn die Motoren im NON_BLOCKING Modus sind wird ¸berpr¸ft wann
// die Fahrzeit abgelaufen ist, wenn ja werden die Motoren angehalten
void taskDrive(void)
{
	if ( motorStatus.autoDrive && systemStatus.pwrON)
	{
		if (getStopwatch(STOPWATCH8) >= motorStatus.timer)
		{
			motorDirection(MOT_BREAK, MOT_BREAK);
			motorStatus.autoDrive = false;
			stopStopwatch8();
		}
	}
	drive_stateChangedHandler();
}
// ------------------------------------------------------------------------


void taskSystem(void)
{
	taskBattery();
	taskDrive();
}

/*!
	@brief	Initialisiert die Hardware (Ports, ADC, PWM)
	
	Diese Funktion muss als erstes aufgerufen werden.
*/
void systemInit(void)
{
	cli();
	
	// PORTB initialisieren
	DDRB |= (1<<PINB0) | (1<<PINB1) | (1<<PINB2);
	
	// PORTC initialisieren
	DDRC |= (1<<PINC2) | (1<<PINC3) | (1<<PINC4) | (1<<PINC5) | (1<<PINC6) | (1<<PINC7);
	
	// PORTD initialisieren
	DDRD |= (1<<PIND4) | (1<<PIND5) | (1<<PIND6) | (1<<PIND7);
	
	#ifndef LCD_SHIELD
	// Interne PullUps der Schalter aktivieren
	PORTB |= (1<<PINB4) | (1<<PINB3);
	#endif
	
	enableDevice(EN_BLUETOOTH);
	disableDevice(EN_ENCODERS);

	// Benutze AVCC als Referenz
	// ADC Prescaler @ 128
	// ADC-Clock @ 125kHz / 13 = 9,62kHz sampling rate, 104µs Sampling time
	ADMUX |= (1<< REFS0);
	ADCSRA |= ( (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0) );
	ADCSRA |= (1<<ADSC);                  // eine ADC-Wandlung 
	while (ADCSRA & (1<<ADSC) ) {         // auf Abschluss der Konvertierung warten
	}
	/*	ADCW muss einmal gelesen werden, sonst wird Ergebnis der n‰chsten
		Wandlung nicht ¸bernommen. */
	(void) ADCW;
	
	//// ADC Offsetabgleich mit Hilfe der internen 1,1V Bandgap Referenz
	//systemStatus.adc_kalibration = adc_Read_Avg(0x0E, 16) - BANDGAP_REF;
	
	// Timer0 wird als Systemtimer mit einer Auflˆsung von 1ms verwendet
	// Timer 0 im CTC Mode2, Prescaler 64, Timer0A1 Interupt alle 1ms 
	TCCR0A |= (1<<WGM01);
	TCCR0B |= (1<<CS01) | (1<<CS00);
	TIMSK0 |= (1<<OCIE0A);
	OCR0A = 0xF9;
	
	systemStatus.battery = batt_voltage();
	systemStatus.time = 0;
	systemStatus.byte = 0;
	lineSensors.byte = 0;
	
	lineInit();
	clearLEDBlue();
	setStatusLED(OFF);
	motorInit();
	encoderInit();
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
	
	static FILE mystdout = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE);
	stdout = &mystdout;
	
	soundInit();
	
	scheduler_Init();
	
	i2c_init();
	
	systemStatus.pwrON = true;
	
	taskID[0] = schedulerAddTask(taskSystem, 0, 10);
	
	sei();
	
	//taskSystem();
}

ISR(TIMER0_COMPA_vect)
{
	// 32bit timer (1ms resolution)
	systemStatus.time++;
	
	// All 1ms based timing stuff
	// 16bit Stopwatches:
	if(stopwatches.watches & STOPWATCH1)
		stopwatches.watch1++;
	if(stopwatches.watches & STOPWATCH2)
		stopwatches.watch2++;
	if(stopwatches.watches & STOPWATCH3)
		stopwatches.watch3++;
	if(stopwatches.watches & STOPWATCH4)
		stopwatches.watch4++;
	if(stopwatches.watches & STOPWATCH5)
		stopwatches.watch5++;
	if(stopwatches.watches & STOPWATCH6)
		stopwatches.watch6++;
	if(stopwatches.watches & STOPWATCH7)
		stopwatches.watch7++;
	if(stopwatches.watches & STOPWATCH8)
		stopwatches.watch8++;
	schedulerUpdate();
}