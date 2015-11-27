

//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/eeprom.h>
	#include <avr/pgmspace.h>
	#include <util/delay.h>

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

	#include "lcd.h"
	#include "DALLAS.h"
	#include "USART.h"


	#define B1		(1<<PA0)
	#define B2		(1<<PA1)
	#define B3		(1<<PA2)
	#define B4		(1<<PA3)
	#define B5		(1<<PA4)

	#define DALLAS 	(1<<PA5)
	#define RELAY 	(1<<PA6)
	
	#define PIN_BUT PINA

	char data_dallas_back[8];

	uint8_t command=0x33;


void init(void)
{
	// Input/Output Ports initialization

	// Port A initialization

	DDRA=(RELAY) | (DALLAS);
	PORTA=(B1) | (B2) |(B3) |(B4) |(B5);

	// Port B initialization

	DDRB=0x00;
	PORTB=0x00;


	// Port C initialization

	DDRC=0xF7;

	PORTC=0x00;

	// Port D initialization

	DDRD=0x08;

	PORTD=0x00;

	// Port E initialization

	DDRE=0x30;

	PORTE=0x00;


	// Port F initialization

	DDRF=0x00;

	PORTF=0x00;

	// Port G initialization

	DDRG=0x00;

	PORTG=0x00;
}

uint8_t c = 0b00001100;
unsigned char data_lcd1;

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
	SEND_STR("     Здравствуй!");

	sei();

	while(1)
	{
		

		if(!(PIN_BUT & B1))
		{
			SEND_CMD(CLR_DISP); 
			SEND_STR("Привет Медвед");
		}
		

		if(!(PIN_BUT & B2))
			SEND_CMD(DISP_CUR2);


		if(!(PIN_BUT & B3))
		{
			char temp[]="Привет Медвед\n\r0123456789\n\r";

			for(uint8_t i=0; i<sizeof(temp); i++) usart1_write(temp[i]);
			
		}


		if(!(PIN_BUT & B4))
		{
			char temp[]="ABCDEFGHabcdefgh!\n0123456789\n";
			usart1_write_str(temp);
			
		}

	}
}
