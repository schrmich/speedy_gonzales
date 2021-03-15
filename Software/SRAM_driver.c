/*********************************************************************
;* FileName     : SRAM_driver.c
;* Dependencies : See INCLUDES section below
;* Processor    : PIC16F family of PICmicro - 8 bit MCU      
;* Assembler/
;* Linker       : HI-TECH PICC STD 9.60/ PRO 9.65
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
;* Deepak Kumar Rana        20-Feb-08           V1.0            Original
;*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/


#include "SRAM_driver.h"
#include "spi.h"


#define SRAM_CS_SET SRAM_CS_PORT &= ~(1<<SRAM_CS_PIN)
#define SRAM_CS_CLR SRAM_CS_PORT |= (1<<SRAM_CS_PIN)

void Write_SR(uint8_t s_reg);

uint8_t Read_SR(void);

/**************************************************************************
; Function              : sramWritePage  
; Side Effects          : User needs to make sure that he is writing within the page  
; Overview              : The data stored in the controller data memory location Data_Writepage is written to the SRAM
;                        
**************************************************************************/

void sramWritePage(uint16_t address, void *page, size_t range)
{ 	
	uint8_t Add_High, Add_Low;
	uint8_t i;
	uint8_t *data_pointer;
	
	if ( (address+PAGE_SIZE) > ADDRESS_MAX )
	{
		return;
	}
	
	if (range > PAGE_SIZE)
	{
		return;
	}
	
	data_pointer = (uint8_t *) page;
	
	Add_Low = address;
	Add_High = address >> 8;
	
	Write_SR(PAGE_MODE);
	
	SRAM_CS_SET;	  		 			//Enable SRAM
	  
	SPI_MasterTransmit(SRAM_WRITE);  		//Write to commad to SR
	  
	SPI_MasterTransmit(Add_High);		//High Address byte of the SRAM
	
	SPI_MasterTransmit(Add_Low);		//Low Address byte of the SRAM
     
	for(i=0; i<range; i++)				//Writing to page of the SRAM
	{
		SPI_MasterTransmit( *(data_pointer +i) );   	//Write the data to the SRAM address location
	}
	
	SRAM_CS_CLR;						// Disable the SRAM 
}


/**************************************************************************
; Function              : sramReadPage  
; Precondition          : SRAM needs to be configured for page mode before calling this routine.The address location and number of 
;						  bytes to be read are passed from the fuction call in SRAM_main.c
; Side Effects          : User needs to make sure that he is reading within the page    
; Overview              : The Number of bytes requested are read from the specified SRAM address location onwards                         
**************************************************************************/
  
 


void sramReadPage(uint16_t address, void *page, size_t range)
{ 	
	uint8_t Add_High, Add_Low;
	uint8_t i;
	uint8_t *data_pointer;
	
	if ( (address+PAGE_SIZE) > ADDRESS_MAX )
	{
		return;
	}
	
	if (range > PAGE_SIZE)
	{
		return;
	}
	
	data_pointer = (uint8_t *) page;
	
	Add_Low = address;
	Add_High = address >> 8;
	
	Write_SR(PAGE_MODE);
	
	SRAM_CS_SET;	  		 				//Enable SRAM
	
	SPI_MasterTransmit(SRAM_READ);  				//Write to commad to SR
	
	SPI_MasterTransmit(Add_High);			//High Address byte of the SRAM
	
	SPI_MasterTransmit(Add_Low);			//Low Address byte of the SRAM
	
	for(i=0; i<range; i++)				//Writing to page of the SRAM
	{
		*(data_pointer+i) = SPI_MasterTransmit(0xFF);	//Read the data from the SRAM address location
	}
	
	SRAM_CS_CLR;							// Disable the SRAM	  
}



/**************************************************************************
; Function              :sramWriteSeq
; PreCondition          :User needs to make sure that the SRAM has be configured to Sequential mode.The address location and number of 
;						 bytes to be written are passed from the fuction call in SRAM_main.c  
; Side Effects          :None
; Overview              :The data stored in the controller data memory location Data_Writepage is written to the SRAM                  
						 In Sequence Mode we can write across the page boundary.	
**************************************************************************/
  
 

void sramWriteSeq(uint16_t address, void *data, size_t range) 
{
    uint8_t Add_High, Add_Low;
    uint8_t i;
	uint8_t *data_pointer;
	
	data_pointer = (uint8_t *) data;
    
	if ( (address+range) > ADDRESS_MAX )
	{
		return;
	}
	
    Add_Low = address;
    Add_High = address >> 8;
    
	Write_SR(SEQ_MODE);
	
    SRAM_CS_SET;	  		 			//Enable SRAM
    
    SPI_MasterTransmit(SRAM_WRITE);  		//Write to commad to SR
    
    SPI_MasterTransmit(Add_High);		//High Address byte of the SRAM
    
    SPI_MasterTransmit(Add_Low);		//Low Address byte of the SRAM
    
    for(i=0; i<range; i++)				//Writing to page of the SRAM
    {		
	    SPI_MasterTransmit(*(data_pointer+i));   	//Write the data to the SRAM address location
    }
    
    SRAM_CS_CLR;						// Disable the SRAM
}





/**************************************************************************
; Function              :sramReadSeq
; Precondition          :User needs to make sure that the SRAM has been configured to Sequential mode.The address location and number of 
;						 bytes to be read are passed from the fuction call in SRAM_main.c 
; Side Effects          :None 
; Overview              :The Number of bytes requested are read from the specified SRAM address location onwards into the Data_Readpage
						 memory location.                 
**************************************************************************/
  
 
void sramReadSeq(uint16_t address, void *data, size_t range)
{

	uint8_t Add_High, Add_Low;
	uint8_t i;
	uint8_t *data_pointer;
	
	data_pointer = (uint8_t *) data;
	
	if ( (address+range) > ADDRESS_MAX )
	{
		return;
	}
	
	Add_Low = address;
	Add_High = address >> 8;
	
	Write_SR(SEQ_MODE);
	
	SRAM_CS_SET;	  		 				//Enable SRAM
	
	SPI_MasterTransmit(SRAM_READ);  		//Write to commad to SR
	
	SPI_MasterTransmit(Add_High);			//High Address byte of the SRAM
	
	SPI_MasterTransmit(Add_Low);			//Low Address byte of the SRAM
	
	for(i=0; i<range; i++)					//Writing to page of the SRAM
	{
		*(data_pointer+i) = SPI_MasterTransmit(0xFF);	//Read the data from the SRAM address location
	}
	
	SRAM_CS_CLR;							// Disable the SRAM
}


/**************************************************************************
; Function              :sramWriteByte
; Precondition          :Make sure that before calling this fuction the status register of SRAM is configured for Byte Mode
; Side Effects          :None
; Overview              :This performs the operation of writing byte stored in Data_Out location to SRAM.
;
**************************************************************************/


void sramWriteByte( uint16_t address,uint8_t Data_Out)
{
	uint8_t Add_High, Add_Low;
	
	Add_Low = address;
	Add_High = address >> 8;
	
	Write_SR(BYTE_MODE);
	
	SRAM_CS_SET;	  		 			//Enable SRAM
	
	SPI_MasterTransmit(SRAM_WRITE);  		//Write to commad to SR
	
	SPI_MasterTransmit(Add_High);		//High Address byte of the SRAM

	SPI_MasterTransmit(Add_Low);		//Low Address byte of the SRAM
	
	
	SPI_MasterTransmit(Data_Out);	   //Write to commad to SR
	
	SRAM_CS_CLR;					   //Disable the SRAM
}



/**************************************************************************
; Function              :sramReadByte
; Precondition          :Make sure that before calling this fuction the status register of SRAM is configured for Byte Mode
; Side Effects          :After the end of this function Data_In contains the data read from the SRAM
; Overview              :This performs the operation of reading byte from SRAM.The read content is stored at location Data_In
;
**************************************************************************/

uint8_t sramReadByte( uint16_t address)
{
	uint8_t Add_High, Add_Low, data_out;
	
	Add_Low = address;
	Add_High = address >> 8;
	
	Write_SR(BYTE_MODE);
	
	SRAM_CS_SET;	  		 				//Enable SRAM
	
	SPI_MasterTransmit(SRAM_READ);  				//Read to commad to SR
	
	SPI_MasterTransmit(Add_High);			//High Address byte of the SRAM
	
	SPI_MasterTransmit(Add_Low);			//Low Address byte of the SRAM
	
	data_out = SPI_MasterTransmit(0xFF);	//Write dummy commad to SR
	
	SRAM_CS_CLR;							//Disable the SRAM
	
	return data_out;    					// pass the read data back
}


/**************************************************************************
; Function              :sramWriteWord
; Precondition          :Make sure that before calling this fuction the status register of SRAM is configured for Byte Mode
; Side Effects          :None
; Overview              :This performs the operation of writing word stored in Data_Out location to SRAM.
;
**************************************************************************/

void sramWriteWord( uint16_t address,uint16_t Data_Out)
{
	uint8_t low_byte, high_byte;
	
	low_byte = Data_Out;
	high_byte = Data_Out >> 8;
	
	sramWriteByte(address, low_byte);
	sramWriteByte(address+1, high_byte);
}

/**************************************************************************
; Function              :sramReadWord
; Precondition          :Make sure that before calling this fuction the status register of SRAM is configured for Byte Mode
; Side Effects          :After the end of this function data contains the data read from the SRAM
; Overview              :This performs the operation of reading word from SRAM.The read content is stored at location data
;
**************************************************************************/

uint16_t sramReadWord( uint16_t address )
{
	uint8_t low_byte, high_byte;
	uint16_t data;
	
	low_byte = sramReadByte(address);
	high_byte = sramReadByte(address+1);
	
	data = high_byte;
	data = data << 8;
	data += low_byte;
	
	return data;
}


void Write_SR(uint8_t s_reg)
{

	SRAM_CS_SET;		  		  //Enable SRAM
	  
	SPI_MasterTransmit(WRSR);	  //Write to commad to SR
	SPI_MasterTransmit(s_reg);   	  //Write the mode passed from the main fuctionto SR
	
	SRAM_CS_CLR;				  // Disable the SRAM 

}


uint8_t Read_SR()
{
	uint8_t s_reg;
	
	SRAM_CS_SET;	  		 				//Enable SRAM
	  
	SPI_MasterTransmit(RDSR);  				//Read_Mode_Register commad
	  
	s_reg = SPI_MasterTransmit(0xFF);		//Write dummy commad to SR, and read Status_Reg
	  
	SRAM_CS_CLR;							//Disable the SRAM
	 
	return s_reg;							//pass the value read back to the called function.
}