

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

	#include "USART.h"	
	#include "DALLAS.h"	


	#define BUZ (1<<PD4)
	#define LED_GREEN (1<<PB1)
	#define LED_YELLOW (1<<PB0)
	#define LED_RED (1<<PB2)

	#define PORT_LED PORTB
	#define PORT_BUZ PORTD


	#define START_TIMER0 TCCR0=(1<<CS02)|(0<<CS01)|(1<<CS00);	//Timer0 start clock 7 812.5 Hz.
	
void init(){

// Port A initialization //

	DDRA=0x00;

	PORTA=0x00;
	
///////////////////////////////////////////////////////////////////

// Port B initialization //

	DDRB=(LED_GREEN) | (LED_YELLOW) | (LED_RED);

	PORTB=0x00;
	
///////////////////////////////////////////////////////////////////

// Port C initialization //

	DDRC=0x00;

	PORTC=0x00;

///////////////////////////////////////////////////////////////////

// Port D initialization //

	DDRD=(BUZ);

	PORTD=0x00;

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////

//Analog Comparator initialization.

	ACSR|=_BV(ACD);		//Disable Comparator.

/////////////////////////////////////////////////////////////////////

//Init Timer0//

TIMSK|=(1<<TOIE0);

/////////////////////////////////////////////////////////////////////

}


int main(void){

	init();

	usart0_init();		//Initialization UART. Configuration USART.h.

	START_TIMER0;
	
	sei();
	
	while(1)
	{

	


	}

}



//INTERRUPT//

ISR(TIMER0_OVF_vect){

	char data_dallas[8];

	if(!(dallas_master(data_dallas, 0x33)))
	{
		
		uint8_t data1[8]={0x01, 0x55, 0xAA, 0x55, 0xBC, 0x3A, 0x01, 0xBC};


		PORT_LED|=LED_YELLOW;

		for(uint8_t t0=0; t0<8; t0++)
		{
			if(data_dallas[t0]!=data1[t0]) 	PORT_LED&=~LED_YELLOW;
		}


		uint8_t data2[8]={0x01, 0x55, 0xAA, 0x55, 0x24, 0xAE, 0x05, 0x28};
		
		PORT_LED|=LED_GREEN;

		for(uint8_t t0=0; t0<8; t0++)
		{
			if(data_dallas[t0]!=data2[t0]) 	PORT_LED&=~LED_GREEN;
		}

	}
	else
	{
		PORT_LED&=~LED_YELLOW;
		PORT_LED&=~LED_GREEN;
	}

}
