
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

	#include "USART.h"

	#define LED (1<<PD6)

void init(void)
{

	DDRA=0x00;
	PORTA=0xFF;

	DDRB=0x00;
	PORTB=0xFF;

	DDRC=0x00;
	PORTC=0xFF;

	DDRD=(LED);
	PORTD=0x00;

	DDRE=0x00;
	PORTE=0xFF;


//////////////////////////////////////////////



//////////////////////////////////////////////

}

////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	
	init();


	while(1)
	{

	}
}
