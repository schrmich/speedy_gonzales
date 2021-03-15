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

#ifndef __NOKIA_3110_H__
#define __NOKIA_3110_H__

#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdbool.h>

#define PORT_LCD PORTB
#define DDR_LCD DDRB

#define LCD_RST PINA0
#define LCD_DC PINA1
#define LCD_SCE PINB4
#define LCD_DIN PINB5
#define LCD_CLK PINB7

#define LCD_CONTRAST 0x40

#define LCD_WIDTH  84
#define LCD_HEIGHT 48

#define LCD_BLACK 1
#define LCD_WHITE 0


/*
 * Must be called once before any other function, initializes display
 */
void lcd_init(void);



/*
 * Clear screen buffer
 */
void lcd_clear(void);

/**
 * Power of display
 * @lcd: lcd nokia struct
 * @on: 1 - on; 0 - off;
 */
void lcd_power(uint8_t on);


#define MODE_BLANK 0x08
#define MODE_NORMAL 0x0C
#define MODE_ALL_ON 0x09
#define MODE_INVERSE 0x0D

/**
 * Set Display mode
 * @mode: MODE_BLANK, MODE_NORMAL, MODE_ALL_ON, MODE_INVERSE;
 */
void lcd_set_mode(uint8_t mode);

/**
 * Set single pixel
 * @x: horizontal pozition
 * @y: vertical position
 * @value: show/hide pixel
 */
void lcd_set_pixel(uint8_t x, uint8_t y, uint8_t value);

/**
 * Draw single char with 1-6 scale
 * @code: char code
 * @scale: size of char
 * @color: font color back or white
 */
//void lcd_write_char(char code, uint8_t scale);
void lcd_write_char(char code, uint8_t scale, bool color);

/**
 * Draw string. Example: writeString("abc",3);
 * @str: sending string
 * @scale: size of text
 * @color: font color back or white
 */
void lcd_write_string(const char *str, uint8_t scale, bool color);

/**
 * Draw string stored in Flash memory. Example: writeString_p(PSTR("abc"),3);
 * @str: sending string
 * @scale: size of text
 * @color: font color back or white
 */
void lcd_write_string_p(const char *str, uint8_t scale, bool color);

/**
 * Macro: Draw string stored in Flash memory. Example: writeString_P("abc",3);
 * @str: sending string
 * @scale: size of text
 * @color: font color back or white
 */
#define lcd_write_string_P(__s, __scale, __color)	lcd_write_string_p(PSTR(__s), __scale, __color)

/**
 * Set cursor position
 * @x: horizontal position
 * @y: vertical position
 */
void lcd_set_cursor(uint8_t x, uint8_t y);

/**
 * Draw Horizontal Line
 * @x: horizontal position
 * @y: vertical position
 * @w: length
 */
void lcd_drawHLine(uint8_t x, uint8_t y, uint8_t w);

/**
 * Draw Vertical Line
 * @x: horizontal position
 * @y: vertical position
 * @w: length
 */
void lcd_drawVLine(uint8_t x, uint8_t y, uint8_t w);

/**
 * Draw Frame
 * @x: horizontal position
 * @y: vertical position
 * @w: with
 * @h: hight
 */
void lcd_drawFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

/**
 * Draw filled Frame
 * @x: horizontal position
 * @y: vertical position
 * @w: width
 * @h: hight
 */
void lcd_drawFillFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

/**
 * Draw Line usingBresenham's algorithm
 * @x0: X-Start point
 * @y0: Y-Start point
 * @x1: X-End point
 * @y1: Y-End point
 */
void lcd_drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

/**
 * Draw Circle using 
 * @xc: horizontal center position
 * @yc: vertical center position
 * @r: Radius
 */
void lcd_drawCircle(int16_t xc, int16_t yc, int16_t r);

/**
 * Draw bitmap 
 * @x: horizontal position
 * @y: vertical position
 * @bmp[]: Bitmap data
 * @w: Width of the bitmap 
 * @h: Hight of the bitmap
 */
void lcd_drawBMP( uint8_t x, uint8_t y, const unsigned char* PROGMEM bmp, uint8_t w, uint8_t h);

/**
 * Invert a box in Screen buffer 
* @x0: X-Start point
* @y0: Y-Start point
* @x1: X-End point
* @y1: Y-End point
 */
//void lcd_invertScreen(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1);

/*
 * Render screen to display
 */
void lcd_render(void);


#endif
