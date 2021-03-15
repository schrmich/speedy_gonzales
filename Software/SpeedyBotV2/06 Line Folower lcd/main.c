/*
 * 06 Line Follower lcd
 *
 * Created: 14.08.2019 12:30:33
 * Author : schreiberm
 */ 

#include "system.h"
#include "pid.h"
#include "lcd_shield.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdlib.h>

#define CMD_GO			'g'
#define CMD_STOP		's'
#define CMD_SET_PARA	'p'
#define CMD_SET_SPEED	't'

#define LINE_LOST_NUM 10

/***********************************/
#define BASE_SPEED 100
#define K_P     0.5f
#define K_I     0.00f
#define K_D     0.8f
/***********************************/

///***********************************/
//#define BASE_SPEED 120
//#define K_P     0.6
//#define K_I     0.0
//#define K_D     1.0
///***********************************/

///***********************************/
//#define BASE_SPEED 180
//#define K_P     1.00
//#define K_I     0.00
//#define K_D     1.00
///***********************************/

int16_t lineReference;
pidData_t pidLineData;
float k_p, k_i, k_d;
int16_t baseSpeed = BASE_SPEED;
bool drive = false;
uint8_t lineLost_num = 0;

// Stopwatch1 wird als Zeitgeber für PID Regler benutzt
// TA = 20ms
#define TA 20


void printMenu(void)
{
	char buffer[15];
	uart_puts_P("\n\nProgramm PID-Regler Linienfolger.");
	uart_puts_P("\n'p' - Set (P)arameter.");
	uart_puts_P("\n's' - (S)top Robot");
	uart_puts_P("\n'g' - (G)o Drive");
	uart_puts_P("\n't' - Se(t) Speed");
	uart_puts_P("\nCurrent values");
	uart_puts_P("\nKP: ");
	uart_puts(dtostrf(k_p, 4, 1, buffer));
	uart_puts_P("\nKI:");
	uart_puts(dtostrf(k_i, 4, 1, buffer));
	uart_puts_P("\nKD:");
	uart_puts(dtostrf(k_d, 4, 1, buffer));
	uart_puts_P("\nSpeed:");
	uart_putInt(baseSpeed);
	setStatusLED(GREEN);
	uart_puts_P("\nBattery: ");
	uart_putInt(systemStatus.battery);
}

void receiverNumber(char *buffer)
{
	uint16_t temp;
	char rxChar;
	uint8_t index = 0;
	bool rxComplete = false;
	
	while (!rxComplete)
	{
		temp = uart_getc();
		
		if (temp != UART_NO_DATA)
		{
			rxChar = (char) temp;
			// Überprüfe ob Trennzeichen vorhanden
			if (rxChar == '\n')
			{
				buffer[index++] = '\0';	// String terminieren
				rxComplete = true;		// Zahlen String fertig
			}
			else
			{
				buffer[index++] = rxChar;	// Neues Zeichen einfügen
			}
		}
	}
}

void receiveParameters(void)
{
	char buffer[10];

	uart_puts_P("\nPID-Reglerparameter einstellen.");
	uart_puts_P("\nSet KP: ");
	receiverNumber(buffer);
	uart_puts(buffer);
	k_p = atof(buffer);
	
	uart_puts_P("\nSet KI: ");
	receiverNumber(buffer);
	uart_puts(buffer);
	k_i = atof(buffer);

	uart_puts_P("\nSet KD: ");
	receiverNumber(buffer);
	uart_puts(buffer);
	k_d = atof(buffer);
	
	uart_puts_P("\nRegelparameter uebernommen.");
}


void driveHandler(void)
{
	int16_t pidOutput;
	int16_t left, right;
	static bool lineLost = false;
	
	if (drive == true)
	{
		startStopwatch1();	// Ausführungszyklus
		startStopwatch2();	// Debugausgaben
		startStopwatch3();	// Linie verloren Timer
		
		if (getStopwatch(STOPWATCH1) >= TA)
		{
			lineRead();
			lineReference = followTrack();
			//lineReference = lineReference >> 1;
			setStopwatch(STOPWATCH1, 0);
			pidOutput = pid_Controller(0, lineReference, &pidLineData);
			
			left = baseSpeed + pidOutput;
			right = baseSpeed - pidOutput;
			
			motor_speed(left, right);
			
			bool lineLost_temp = true; // Temporär setzen
			for (uint8_t i=0; i<5; i++)
			{
				if (lineSensors.digitalData[i] == 1)  lineLost_temp = false;
			}
			
			if (lineLost_temp == true) lineLost_num++;
		
			if (lineLost_num >= LINE_LOST_NUM)
			{
				drive = false;
				motorDirection(MOT_BREAK, MOT_BREAK);
				setStatusLED(YELLOW);
				uart_puts_P("\nLine is lost!\nBot stopped.\n");
			}
			
			if (getStopwatch(STOPWATCH3) > 400)	
			{
				setStopwatch(STOPWATCH3, 0);
				lineLost_num = 0;
			}
			
			// Monitoring
			if ( getStopwatch(STOPWATCH2) > 100 )
			{
				LOG_DEBUG("Motor Left:%i Right:%i", motorStatus.speedLeft, motorStatus.speedRight);
				LOG_DEBUG("Line-Ref:%i", lineReference);
				LOG_DEBUG("PID-Out:%i", pidOutput);
				LOG_DEBUG("Akku:%i", batt_voltage());
				setStopwatch(STOPWATCH2,0);
			}
		}
	}
	else
	{
		stopStopwatch1();
		stopStopwatch2();
		stopStopwatch3();
	}
}

void commandProcessor(void)
{
	uint16_t receive;
	char cmd;
	
	receive = uart_getc();
	
	if (receive < UART_NO_DATA)
	{
		cmd = (char) receive;
		
		// Warten auf zweites Zeichen '\n' 
		while(uart_getc() != '\n') {};
		
		
		if (cmd == CMD_SET_PARA)
		{
			motorDirection(MOT_BREAK,MOT_BREAK);
			setStatusLED(YELLOW);
			receiveParameters();
			pid_Init(k_p*SCALING_FACTOR, k_i*SCALING_FACTOR, k_d*SCALING_FACTOR, &pidLineData);
			setStatusLED(GREEN);
		}
		else if (cmd == CMD_STOP)
		{
			uart_puts_P("\nBot Stop.");
			motorDirection(MOT_BREAK, MOT_BREAK);
			printMenu();
			setStatusLED(GREEN);
			drive = false;
		}
		else if (cmd == CMD_GO)
		{
			uart_puts_P("\nBot Start.");
			motorDirection(MOT_FWD, MOT_FWD);
			motor_speed(baseSpeed, baseSpeed);
			pid_Reset_Integrator(&pidLineData);
			setStatusLED(RED);
			drive = true;
		}
		else if (cmd == CMD_SET_SPEED)
		{
			char buffer[10];
			uart_puts_P("\nSet Speed:");
			setStatusLED(YELLOW);
			receiverNumber(buffer);
			baseSpeed = atoi(buffer);
			uart_puts(buffer);
			setStatusLED(GREEN);
		}
	}
}

int main(void)
{
    systemInit();
	drive_setStateChangedHandler(driveHandler);

	enableDevice(EN_BLUETOOTH);
	
	waitStart();
	k_p = K_P;
	k_i = K_I;
	k_d = K_D;
	printMenu();
	pid_Init(K_P*SCALING_FACTOR, K_I*SCALING_FACTOR, K_D*SCALING_FACTOR, &pidLineData);
	systemStatus.pwrON = true;
	
    while (1) 
    {
		taskSystem();
		
		commandProcessor();
		
		if (userButton())
		{
			uart_puts_P("\nBot Stop.");
			motorDirection(MOT_BREAK, MOT_BREAK);
			setStatusLED(GREEN);
		}
		
		
    }
}

