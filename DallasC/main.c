

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

	#include "DALLAS.h"	

	#define BUZ (1<<PD4)
	#define LED_RED (1<<PC0)
	#define LED_GREEN (1<<PC1)
//	#define LED_YELLOW (1<<PB0)
	#define PORT_LED PORTC
//	#define PORT_BUZ PORTD

	
	uint8_t command=0x33;


void init(){


// Port B initialization //

	DDRB=(1<<PB2)|(1<<PB0);

	PORTB=(1<<PB0);
	
///////////////////////////////////////////////////////////////////

// Port C initialization //

	DDRC=(LED_RED)|(LED_GREEN);

	PORTC=(LED_RED)|(LED_GREEN);

///////////////////////////////////////////////////////////////////

// Port D initialization //

	DDRD=0x00;

	PORTD=0x00;

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////

//Analog Comparator initialization.

	ACSR|=_BV(ACD);		//Disable Comparator.

/////////////////////////////////////////////////////////////////////

//Init Timer0//

TIMSK0|=(1<<TOIE0);

/////////////////////////////////////////////////////////////////////

}


int main(void){

	init();

//	usart0_init();		//Initialization UART. Configuration USART.h.

	TCCR0B=(1<<CS02)|(0<<CS01)|(1<<CS00);	//Timer0 clock off.
	
	sei();
	
	while(1)
	{

	


	}

}




//INTERRUPT//

ISR(TIMER0_OVF_vect){

	char data_dallas[8];

	if(!(dallas(data_dallas, command)))
	{


		uint8_t data1[8]={0x01, 0x55, 0xAA, 0x55, 0xBC, 0x3A, 0x01, 0xBC};


		

		for(uint8_t t0=0; t0<8; t0++)
		{
			if(data_dallas[t0]!=data1[t0]) 	PORT_LED|=LED_RED;
			
		}


		uint8_t data2[8]={0x01, 0xBD, 0xBB, 0x81, 0x0A, 0x00, 0x00, 0x73};
		
		

		for(uint8_t t0=0; t0<8; t0++)
			if(data_dallas[t0]!=data2[t0]) 	PORT_LED|=LED_GREEN;

				
	}
	else
	{
//		PORT_LED&=~LED_YELLOW;
		PORT_LED&=~LED_GREEN;
		PORT_LED&=~LED_RED;

	}

}
