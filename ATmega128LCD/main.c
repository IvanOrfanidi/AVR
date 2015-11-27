

//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <avr/eeprom.h>

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>

	#include "lcd.h"
	#include "USART.h"
		
	#define BUT1		(1<<PA0)
	#define BUT2		(1<<PA1)
	#define BUT3		(1<<PA2)
	#define BUT4		(1<<PA3)
	#define BUT5		(1<<PA4)
	#define DALLAS 	(1<<PA5)
	#define RELAY 	(1<<PA6)
	
	#define PIN_BUT PINA

	#define PORT_RS485	PORTB
	#define RE_DE		(1<<PB1)


void init(void)
{
	// Input/Output Ports initialization

	// Port A initialization

	DDRA=(RELAY) | (DALLAS);
	PORTA=(BUT1) | (BUT2) |(BUT3) |(BUT4) |(BUT5);

	// Port B initialization

	DDRB=RE_DE;
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


	PORT_RS485&=~RE_DE;

	sei();

	while(1)
	{
		uint8_t Len=usart0_rx_len();
		if(Len)
		{
			uint8_t Data_RX[Len];
			for(uint8_t i=0; i<Len; i++)
				Data_RX[i] = usart0_read();

			PORT_RS485|=RE_DE;

			for(uint8_t i=0; i<Len; i++)
				usart0_write(Data_RX[i]);

			_delay_ms(10);

			PORT_RS485&=~RE_DE;

		}
		
		if(!(PIN_BUT & BUT1))
		{
			char msg[]="Hello World!";

			PORT_RS485|=RE_DE;

			for(uint8_t i=0; i<sizeof(msg); i++)
				usart0_write(msg[i]);

			_delay_ms(10);

			PORT_RS485&=~RE_DE;
		}	


	}

}



