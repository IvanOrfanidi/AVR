

/*********************************************
Project : ATmega128_MT
Version : 01
Date    : 14.1.2007
Author  : Slavcho TOmov                 
Company : Olimex                            
Comments: Demo program


Chip type           : ATmega128
Program type        : Application
Clock frequency     : 
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 1024
*********************************************/

//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/eeprom.h>
	#include <avr/pgmspace.h>
	#include <util/delay.h>
	#include <util/crc16.h>

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

	#include "lcd.h"
	#include "USART.h"


#define		B1				(1<<PA0)  
#define		B2				(1<<PA1)
#define		B3				(1<<PA2)
#define		B4				(1<<PA3)
#define		B5				(1<<PA4)


#define 	RELAY_HIGH		PORTA |= (1<<PA6)
#define 	RELAY_LOW		PORTA &= ~(1<<PA6)                      

#define		DALLAS			(BIT5)


const uint8_t msg0[]={0x01, 0x03, 0x00, 0x01, 0x00, 0x06};


void init(void)
{
	// Input/Output Ports initialization
	// Port A initialization
	PORTA=(0<<PA6);
	DDRA=0x40;

	// Port B initialization
	PORTB=0x00;
	DDRB=0x00;

	// Port C initialization
	PORTC=0x00;
	DDRC=0xF7;

	// Port D initialization
	PORTD=0x00;
	DDRD=0x08;

	// Port E initialization
	PORTE=0x00;
	DDRE=0x30;

	// Port F initialization
	PORTF=0x00;
	DDRF=0x00;

	// Port G initialization
	PORTG=0x00;
	DDRG=0x00;
}

	

int main()
{
	init();

	usart0_init();

	usart1_init();

	//LCD initialisation 
	LCD_Ini(); 					     
	// delay_ms(2);  		
	SEND_CMD(DISP_ON);
	// delay_ms(10);  						
	SEND_CMD(CLR_DISP); 
	// write
	//SEND_STR(" www.olimex.com");



	uint16_t modbus_crc;

	sei();

/*
	while(1)
	{
		modbus_crc=0xFFFF;

		for(uint16_t i=0; i<sizeof(msg0); i++)
			modbus_crc=_crc16_update(modbus_crc, msg0[i]);

		uint8_t modbus_crc_l=modbus_crc>>8;
		uint8_t modbus_crc_h=modbus_crc;

		for(uint16_t i=0; i<sizeof(msg0); i++)
		{
			usart1_write(msg0[i]);
			_delay_ms(USART1_DELAY_MS);
		}

		usart1_write(modbus_crc_h);
		_delay_ms(USART1_DELAY_MS);

		usart1_write(modbus_crc_l);
		
		_delay_ms(500);

		usart1_clear_tx_buffer();

		uint8_t Len=usart1_rx_len();

		
		if(Len)
		{
			uint8_t Data[Len];

			for(uint8_t i=0; i<Len; i++)
				Data[i]=usart1_read();

			usart1_clear_rx_buffer();
				
			modbus_crc=0xFFFF;

			for(uint8_t i=0; i<Len; i++)
				modbus_crc=_crc16_update(modbus_crc, Data[i]);
				
			if(!(modbus_crc))
			{
				SEND_CMD(DD_RAM_ADDR);

				for(uint8_t i=4; i!=16; i=i+2)
					SEND_CHAR(Data[i]);
			}
		}
		else
		{

		}			
	}
*/


	while(1)
	{
		modbus_crc=0xFFFF;

		for(uint16_t i=0; i<sizeof(msg0); i++)
			modbus_crc=_crc16_update(modbus_crc, msg0[i]);

		uint8_t modbus_crc_l=modbus_crc>>8;
		uint8_t modbus_crc_h=modbus_crc;

		for(uint16_t i=0; i<sizeof(msg0); i++)
		{
			usart0_write(msg0[i]);
			_delay_ms(USART1_DELAY_MS);
		}

		usart0_write(modbus_crc_h);
		_delay_ms(USART1_DELAY_MS);

		usart0_write(modbus_crc_l);
		
		_delay_ms(500);

		usart0_clear_tx_buffer();

		uint8_t Len=usart0_rx_len();

		
		if(Len)
		{
			uint8_t Data[Len];

			for(uint8_t i=0; i<Len; i++)
				Data[i]=usart0_read();

			usart1_clear_rx_buffer();
				
			modbus_crc=0xFFFF;

			for(uint8_t i=0; i<Len; i++)
				modbus_crc=_crc16_update(modbus_crc, Data[i]);
				
			if(!(modbus_crc))
			{
				SEND_CMD(DD_RAM_ADDR);

				for(uint8_t i=4; i!=16; i=i+2)
					SEND_CHAR(Data[i]);
			}
		}
		else
		{

		}
	}			

}


