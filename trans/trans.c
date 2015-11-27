

//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>


// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>



void init()
{

	// Port B initialization //

	DDRB=(0<<PB0)|(0<<PB1)|(0<<PB2)|(0<<PB3)|(1<<PB4);

	PORTB=(1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3)|(1<<PB4);



}


int main()
{
	
	init();
	uint8_t y;

	//sei();

	while(1)
	{
		if((PINB & (1<<PB3)))
		{
			DDRB|=(1<<PB4);
			PORTB&=~(1<<PB4);
		}
			else
			{
		
				PORTB|=(1<<PB4);
				DDRB&=~(1<<PB4);
				//_delay_ms(30);
			}


	}

}

