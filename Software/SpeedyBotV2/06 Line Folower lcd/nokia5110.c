/* Nokia 5110 LCD AVR Library
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 *
 * Original library written by SkewPL, http://skew.tk
 */

#include "nokia5110.h"

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "nokia5110_chars.h"

#define sgn(x) ((x<0)?-1:((x>0)?1:0)) /* macro to return the sign of a
                                         number */
static struct {
    /* screen byte massive */
    uint8_t screen[504];

    /* cursor position */
    uint8_t cursor_x;
    uint8_t cursor_y;

} nokia_lcd = {
    .cursor_x = 0,
    .cursor_y = 0
};

void spi_masterInit(void)
{
	//SPSR |= (1<<SPI2X);
	SPCR = (1<<SPE) | (1<<MSTR);
}

uint8_t spi_masterTransmit(uint8_t data)
{
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

/**
 * Sending data to LCD
 * @bytes: data
 * @is_data: transfer mode: 1 - data; 0 - command;
 */
static void write(uint8_t bytes, uint8_t is_data)
{
	/* Enable controller */
	PORT_LCD &= ~(1 << LCD_SCE);

	/* We are sending data */
	if (is_data)
		PORTA |= (1 << LCD_DC);
	/* We are sending commands */
	else
		PORTA &= ~(1 << LCD_DC);
	
	spi_masterTransmit(bytes);

	/* Disable controller */
	PORT_LCD |= (1 << LCD_SCE);
}

static void write_cmd(uint8_t cmd)
{
	write(cmd, 0);
}

static void write_data(uint8_t data)
{
	write(data, 1);
}

/*
 * Public functions
 */

void lcd_init(void)
{
	register unsigned i;
	
	
	/* Set pins as output */
	DDRA |= (1 << LCD_RST);
	DDRA |= (1 << LCD_DC);
	DDR_LCD |= (1 << LCD_SCE);
	DDR_LCD |= (1 << LCD_DIN);
	DDR_LCD |= (1 << LCD_CLK);
	
	spi_masterInit();
	
	/* Reset display */
	PORTA |= (1 << LCD_RST);
	PORT_LCD |= (1 << LCD_SCE);
	_delay_ms(10);
	PORTA &= ~(1 << LCD_RST);
	_delay_ms(70);
	PORTA |= (1 << LCD_RST);

	/*
	 * Initialize display
	 */
	/* Enable controller */
	PORT_LCD &= ~(1 << LCD_SCE);
	/* -LCD Extended Commands mode- */
	write_cmd(0x21);
	/* LCD bias mode 1:48 */
	write_cmd(0x13);
	/* Set temperature coefficient */
	write_cmd(0x06);
	/* Default VOP (3.06 + 66 * 0.06 = 7V) */
	write_cmd(0xC2);
	/* Standard Commands mode, powered down */
	write_cmd(0x20);
	/* LCD in normal mode */
	write_cmd(0x09);

	/* Clear LCD RAM */
	write_cmd(0x80);
	write_cmd(LCD_CONTRAST);
	for (i = 0; i < 504; i++)
		write_data(0x00);

	/* Activate LCD */
	write_cmd(0x08);
	write_cmd(0x0C);
}

void lcd_set_mode(uint8_t mode)
{
	write_cmd(mode);
}

void lcd_clear(void)
{
	register unsigned i;
	/* Set column and row to 0 */
	write_cmd(0x80);
	write_cmd(0x40);
	/*Cursor too */
	nokia_lcd.cursor_x = 0;
	nokia_lcd.cursor_y = 0;
	/* Clear everything (504 bytes = 84cols * 48 rows / 8 bits) */
	for(i = 0;i < 504; i++)
		nokia_lcd.screen[i] = 0x00;
}

void lcd_power(uint8_t on)
{
	write_cmd(on ? 0x20 : 0x24);
}

void lcd_set_pixel(uint8_t x, uint8_t y, uint8_t value)
{
	uint8_t *byte = &nokia_lcd.screen[y/8*84+x];
	if (value)
		*byte |= (1 << (y % 8));
	else
		*byte &= ~(1 << (y % 8));
}

//void lcd_write_char(char code, uint8_t scale)
//{
	//register uint8_t x, y;
	//
	//for (x = 0; x < 5*scale; x++)
		//for (y = 0; y < 7*scale; y++)
			//if (pgm_read_byte(&CHARSET[code-32][x/scale]) & (1 << y/scale))
				//lcd_set_pixel(nokia_lcd.cursor_x + x, nokia_lcd.cursor_y + y, 1);
			//else
				//lcd_set_pixel(nokia_lcd.cursor_x + x, nokia_lcd.cursor_y + y, 0);
//
	//nokia_lcd.cursor_x += 5*scale + 1;
	//if (nokia_lcd.cursor_x >= 84) {
		//nokia_lcd.cursor_x = 0;
		//nokia_lcd.cursor_y += 7*scale + 1;
	//}
	//if (nokia_lcd.cursor_y >= 48) {
		//nokia_lcd.cursor_x = 0;
		//nokia_lcd.cursor_y = 0;
	//}
//}

void lcd_write_char(char code, uint8_t scale, bool color)
{
	register uint8_t x, y;
	
	for (x = 0; x < 5*scale; x++)
	{
		for (y = 0; y < 7*scale; y++)
		{
			if (pgm_read_byte(&CHARSET[code-32][x/scale]) & (1 << y/scale))
				lcd_set_pixel(nokia_lcd.cursor_x + x, nokia_lcd.cursor_y + y, color);
			else
				lcd_set_pixel(nokia_lcd.cursor_x + x, nokia_lcd.cursor_y + y, !color);
		}
	}
	
	if (color != LCD_BLACK)
	{
		for (y = 0; y < 7*scale; y++)
		{
			lcd_set_pixel(nokia_lcd.cursor_x+5*scale, nokia_lcd.cursor_y+y, 1);
		}
	}
	
	
	nokia_lcd.cursor_x += 5*scale + 1;
	if (nokia_lcd.cursor_x >= 84) {
		nokia_lcd.cursor_x = 0;
		nokia_lcd.cursor_y += 7*scale + 1;
	}
	if (nokia_lcd.cursor_y >= 48) {
		nokia_lcd.cursor_x = 0;
		nokia_lcd.cursor_y = 0;
	}
}

void lcd_write_string(const char *str, uint8_t scale, bool color)
{
	while(*str)
		lcd_write_char(*str++, scale, color);
}

void lcd_write_string_p(const char *str, uint8_t scale, bool color)
{
	char c;
	while((c = pgm_read_byte(str++)))
		lcd_write_char(c, scale, color);
}

void lcd_set_cursor(uint8_t x, uint8_t y)
{
	nokia_lcd.cursor_x = x;
	nokia_lcd.cursor_y = y;
}

/**
 * Draw Horizontal Line
 * @x: horizontal position
 * @y: vertical position
 * @w: length
 */
void lcd_drawHLine(uint8_t x, uint8_t y, uint8_t w)
{
	for (uint8_t i=0; i<w; i++)
	{
		lcd_set_pixel(x+i, y, 1);
	}
}

/**
 * Draw Vertical Line
 * @x: horizontal position
 * @y: vertical position
 * @w: length
 */
void lcd_drawVLine(uint8_t x, uint8_t y, uint8_t w)
{
	for (uint8_t i=0; i<w; i++)
	{
		lcd_set_pixel(x, y+i, 1);
	}
}

/**
 * Draw Line
 * @x0: X-Start point
 * @y0: Y-Start point
 * @x1: X-End point
 * @y1: Y-End point
 */
void lcd_drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	if (x0 >= LCD_WIDTH || x1 >= LCD_WIDTH)	return;
	if (y0 >= LCD_HEIGHT || y1 >= LCD_HEIGHT)	return;
	
	// Iterators, counters required by algorithm
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	
	// Calculate line deltas
	dx = x1 - x0;
	dy = y1 - y0;
	
	// Create a positive copy of deltas (makes iterating easier)
	dx1 = abs(dx);
	dy1 = abs(dy);
	
	// Calculate error intervals for both axis
	px = 2 * dy1 - dx1;
	py = 2 * dx1 - dy1;
	
	// The line is X-axis dominant
	if (dy1 <= dx1) {
		
		// Line is drawn left to right
		if (dx >= 0) {
			x = x0; y = y0; xe = x1;
			} else { // Line is drawn right to left (swap ends)
			x = x1; y = y1; xe = x0;
		}
		
		lcd_set_pixel(x, y, 1);
		
		// Rasterize the line
		for (i = 0; x < xe; i++) {
			x = x + 1;
			
			// Deal with octants...
			if (px < 0) {
				px = px + 2 * dy1;
				} else {
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
					y = y + 1;
					} else {
					y = y - 1;
				}
				px = px + 2 * (dy1 - dx1);
			}
			
			// Draw pixel from line span at
			// currently rasterized position
			lcd_set_pixel(x, y, 1);
		}
		
		} else { // The line is Y-axis dominant
		
		// Line is drawn bottom to top
		if (dy >= 0) {
			x = x0; y = y0; ye = y1;
			} else { // Line is drawn top to bottom
			x = x1; y = y1; ye = y0;
		}
		
		lcd_set_pixel(x, y, 1);
		
		// Rasterize the line
		for (i = 0; y < ye; i++) {
			y = y + 1;
			
			// Deal with octants...
			if (py <= 0) {
				py = py + 2 * dx1;
				} else {
				if ((dx < 0 && dy<0) || (dx > 0 && dy > 0)) {
					x = x + 1;
					} else {
					x = x - 1;
				}
				py = py + 2 * (dx1 - dy1);
			}
			
			// Draw pixel from line span at
			// currently rasterized position
			lcd_set_pixel(x, y, 1);
		}
	}
}

/**
 * Draw Frame
 * @x: horizontal position
 * @y: vertical position
 * @w: with
 * @h: hight
 */
void lcd_drawFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	lcd_drawHLine(x, y, w);
	lcd_drawHLine(x, y+h, w+1);
	
	lcd_drawVLine(x, y, h);
	lcd_drawVLine(x+w, y, h+1);
}

/**
 * Draw filled Frame
 * @x: horizontal position
 * @y: vertical position
 * @w: with
 * @h: hight
 */
void lcd_drawFillFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	for (uint8_t dy=y; dy<(y+h); dy++)
	{
		for (uint8_t dx=x; dx<(x+w); dx++)
		{
			lcd_set_pixel(dx, dy, 1);
		}
	}
}

// Function to put pixels
// at subsequence points
void drawCircleHelper(int16_t xc, int16_t yc, int16_t x, int16_t y)
{
	lcd_set_pixel(xc+x, yc+y, 1);
	lcd_set_pixel(xc-x, yc+y, 1);
	lcd_set_pixel(xc+x, yc-y, 1);
	lcd_set_pixel(xc-x, yc-y, 1);
	lcd_set_pixel(xc+y, yc+x, 1);
	lcd_set_pixel(xc-y, yc+x, 1);
	lcd_set_pixel(xc+y, yc-x, 1);
	lcd_set_pixel(xc-y, yc-x, 1);
}

/**
 * Draw Circle using 
 * @x: horizontal position
 * @y: vertical position
 * @r: Radius
 */
void lcd_drawCircle(int16_t xc, int16_t yc, int16_t r)
{
	int x = 0, y = r;
	int d = 3 - 2 * r;
	drawCircleHelper(xc, yc, x, y);
	while (y >= x)
	{
		// for each pixel we will
		// draw all eight pixels
		
		x++;
		
		// check for decision parameter
		// and correspondingly
		// update d, x, y
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else
		d = d + 4 * x + 6;
		drawCircleHelper(xc, yc, x, y);
	}
}

/**
 * Draw bitmap 
 * @x: horizontal position
 * @y: vertical position
 * @bmp[]: Bitmap data
 * @w: Width of the bitmap 
 * @h: Hight of the bitmap
 */
void lcd_drawBMP( uint8_t x, uint8_t y, const unsigned char* PROGMEM bmp, uint8_t w, uint8_t h)
{
	for (uint8_t dy=y; dy<(y+h); dy++)
	{
		for (uint8_t dx=x; dx<(x+w); dx++)
		{
			if (pgm_read_byte(bmp))
			{
			}
			lcd_set_pixel(dx, dy, 1);
		}
	}
}

void lcd_render(void)
{
	register unsigned i;
	/* Set column and row to 0 */
	write_cmd(0x80);
	write_cmd(0x40);

	/* Write screen to display */
	for (i = 0; i < 504; i++)
		write_data(nokia_lcd.screen[i]);
	
	
	
}
