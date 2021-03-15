/*********************************************************************
;* FileName     : SRAM_driver.h
;* Dependencies : See INCLUDES section below
;* Processor    : Atmel 8bit AVR (new Version)    
;* Assembler/
;* Linker       : HI-TECH PICC STD 9.60 /PRO 9.65
;* Company      : Microchip Technology, Inc.
;*
;*                     Software License Agreement                      
;*                                                                     
;* Microchip Technology Inc. (“Microchip”) licenses this software to you
;* solely for use with Microchip microcontrollers and Microchip serial
;* EEPROM products.  The software is owned by Microchip and/or its
;* licensors, and is protected under applicable copyright laws.  All
;* rights reserved.
;*
;* SOFTWARE IS PROVIDED "AS IS."  MICROCHIP AND ITS LICENSOR EXPRESSLY
;* DISCLAIM ANY WARRANTY OF ANY KIND, WHETHER EXPRESS OR IMPLIED,
;* INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
;* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR
;* NON-INFRINGEMENT. IN NO EVENT SHALL MICROCHIP AND ITS LICENSORS BE
;* LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL
;* DAMAGES, LOST PROFITS OR LOST DATA, HARM TO YOUR EQUIPMENT, COST OF
;* PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
;* BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
;* ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER SIMILAR COSTS.
;*
;* Author                   Date                Version         Comment
;*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;* Michael Schreiber		03-Jan-18			V1.1			ATMEGA Version
;* Deepak Kumar Rana        20-Feb-08           V1.0            Original PIC-MCU
;*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

/*!
 
 @file

 @brief Serial Backup NV-SRAM 32kx8 (SPI) 
 
 */

#ifndef SRAM_DRIVER_H_
#define SRAM_DRIVER_H_

#include <avr/io.h>
#include <stdint.h>
#include <stddef.h>

#define SRAM_CS_DDR DDRB
#define SRAM_CS_PORT PORTB
#define SRAM_CS_PIN PINB4

 




void sramWriteByte(uint16_t ram_address, uint8_t data);

uint8_t sramReadByte(uint16_t ram_address);

void sramWriteWord( uint16_t address,uint16_t Data_Out);

uint16_t sramReadWord( uint16_t address );

void sramWritePage(uint16_t address, void *page, size_t range);

void sramReadPage(uint16_t address, void *page, size_t range);

void sramReadSeq(uint16_t address, void *data, size_t range);

void sramWriteSeq(uint16_t address, void *data, size_t range);


/**************************************************************************	
	;Define SPI commands for the SRAM
;**************************************************************************/
#define  SRAM_READ   0x03  	//read from memory array beginning at the selected address
#define  SRAM_WRITE  0x02   //write to memory array beginning at the selected address
#define  RDSR   0x05     	//read the status register
#define  WRSR   0x01     	//write the status register

#define BYTE_MODE 0x00		// ;bit7 =0 bit6 =0 Byte Mode
#define PAGE_MODE 0x80  	// ;bit7 =1 bit6 =0 Page Mode
#define SEQ_MODE  0x40  	// ;bit7 =0 bit6 =1 Sequential mode

#define PAGE_SIZE 32
#define PAGE_NUM 2048
#define ADDRESS_MAX	0x1999 

#endif /* SRAM_DRIVER_H_ */