
//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <avr/eeprom.h>
	#include <avr/wdt.h>


// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>

// User functions.
	#include "USART.h"

//////////////////////////////////////////////

void init(void)
{

	DDRA = 0xFF;
	PORTA = 0x00;

	DDRB = 0xFF;
	PORTB = 0xFF;

	DDRC=0x00;
	PORTC=0x00;

	DDRD= 0x00;
	PORTD= 0x00;

}

int main(void)
{

	init();

	usart0_init();	

	PORTA = 0xFF;
	PORTB = 0xFF;

	sei();

	while(1)
	{
		


	}

}
