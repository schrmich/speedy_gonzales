/*
 * lcd_shield.c
 *
 * Created: 28.04.2020 20:44:21
 *  Author: Michael
 */ 

#include <avr/pgmspace.h>
#include "speedy.h"
#include "lcd_shield.h"


void dispStatus(void)
{
	lcd_clear();
	
	char buffer[20];
	uint32_t _battPerc = systemStatus.battery;
	_battPerc -= V_BATT_MIN;
	_battPerc *= 100;
	_battPerc = _battPerc / (8000-V_BATT_MIN);
	
	if (_battPerc > 100) _battPerc = 100;
	
	dispSetCusor(0, 0, 1);
	lcd_write_string_P("*SYSTEM SATUS*", 1, LCD_WHITE);
	
	dispSetCusor(0, 1, 1);
	sprintf_P(buffer, PSTR("BATT: %3i%%"), (int)_battPerc);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(0, 2, 1);
	sprintf_P(buffer, PSTR("TIME: %2im %2is"), (get_time_s()/60), (get_time_s()%60) );
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	lcd_drawHLine(0, 23, 84);
	lcd_drawVLine(42, 24, 24);
	
	dispSetCusor(0, 3, 1);
	sprintf_P(buffer, PSTR("Pow: %i"), systemStatus.pwrON);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(0, 4, 1);
	sprintf_P(buffer, PSTR("BLED:%i"), systemStatus.bottomLeds);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(0, 5, 1);
	sprintf_P(buffer, PSTR("BLTH:%i"), systemStatus.bluetooth);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(8, 3, 1);
	sprintf_P(buffer, PSTR("Enc: %i"), systemStatus.encoders);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(8, 4, 1);
	sprintf_P(buffer, PSTR("ERR: %i"), systemStatus.error);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(8, 5, 1);
	sprintf_P(buffer, PSTR("LINE:%i"), lineSensors.enabled);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	lcd_render();
}

void dispSensors(void)
{
	lcd_clear();
	
	char buffer[20];
	
	dispSetCusor(0, 0, 1);
	lcd_write_string_P("*LINEA:D ENC *", 1, LCD_WHITE);
	
	dispSetCusor(0, 1, 1);
	sprintf_P(buffer, PSTR("L %4i:%1i"), lineSensors.analogData[LINE_LEFT], lineSensors.digitalData[LINE_LEFT]);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(0, 2, 1);
	sprintf_P(buffer, PSTR("LC%4i:%1i"), lineSensors.analogData[LINE_CLEFT], lineSensors.digitalData[LINE_CLEFT]);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(0, 3, 1);
	sprintf_P(buffer, PSTR("C %4i:%1i"), lineSensors.analogData[LINE_CENTER], lineSensors.digitalData[LINE_CENTER]);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(0, 4, 1);
	sprintf_P(buffer, PSTR("RC%4i:%1i"), lineSensors.analogData[LINE_CRIGHT], lineSensors.digitalData[LINE_CRIGHT]);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(0, 5, 1);
	sprintf_P(buffer, PSTR("R %4i:%1i"), lineSensors.analogData[LINE_RIGHT], lineSensors.digitalData[LINE_RIGHT]);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	lcd_drawVLine(CHAR_WIDHT*8-1, 0, 48);
	
	dispSetCusor(8, 1, 1);
	sprintf_P(buffer, PSTR("L %4i"), motorStatus.encoder[LEFT]);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(8, 2, 1);
	sprintf_P(buffer, PSTR("R %4i"), motorStatus.encoder[RIGHT]);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(8, 3, 1);
	lcd_write_string_P("MT_PWM" , 1, LCD_WHITE);
	
	dispSetCusor(8, 4, 1);
	sprintf_P(buffer, PSTR("L  %3i"), motorStatus.speedLeft);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	dispSetCusor(8, 5, 1);
	sprintf_P(buffer, PSTR("R  %3i"), motorStatus.speedRight);
	lcd_write_string(buffer, 1, LCD_BLACK);
	
	
	lcd_render();
}

void dispSetCusor(uint8_t x, uint8_t y, uint8_t scale)
{
	lcd_set_cursor(CHAR_WIDHT *x*scale, CHAR_HIGHT*y*scale);
}



void shieldInit(void)
{
	lcd_init();
	
	PCF8574_data.byte = 0xff;
	
	PCF8574_data.led_bl = true;
	PCF8574_data.led_rt = true;
	
	i2c_start_wait(PCF8574_ADRESS + I2C_WRITE);
	i2c_write(PCF8574_data.byte);
	i2c_stop();
	
	taskID[1] = schedulerAddTask(readSwitch, 10, 100);
}

void readSwitch(void)
{
	i2c_start(PCF8574_ADRESS + I2C_READ);
	PCF8574_data.byte = i2c_readNak();
	i2c_stop();
}

void writeLeds(uint8_t led)
{
	if (led == LED_BLUE) 
	{
		PCF8574_data.led_bl = false;
		PCF8574_data.led_rt = true;
	}
	else if (led == LED_RED) 
	{
		PCF8574_data.led_bl = true;
		PCF8574_data.led_rt = false;
	}
	else if (led == LED_PURPLE)
	{
		PCF8574_data.led_bl = false;
		PCF8574_data.led_rt = false;
	}
	else
	{
		PCF8574_data.led_bl = true;
		PCF8574_data.led_rt = true;
	}
	
	i2c_start(PCF8574_ADRESS + I2C_WRITE);
	i2c_write(PCF8574_data.byte);
	i2c_stop();
}

