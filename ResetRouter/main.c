


//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>


// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

	#define RELAY	(1<<PB3)
	#define IN1		(1<<PB4)

	#define IN1_OFF	((PINB & (IN1)))
	#define IN1_ON	(!(PINB & (IN1)))

	#define RELAY_ON	PORTB|=RELAY
	#define RELAY_OFF	PORTB&=~RELAY

	#define TIME_OFF		3


void init()
{

	// Port B initialization //

		DDRB=(0<<PB0)|(0<<PB1)|(0<<PB2)|(1<<PB3)|(0<<PB4);

		PORTB=(0<<PB0)|(0<<PB1)|(0<<PB2)|(1<<PB3)|(0<<PB4);

	///////////////////////////

	//Timer1 initialization.

	//	TCCR0B=(0<<CS12)|(0<<CS11)|(1<<CS10);	//Timer1 clock on.

	//	OCR0A=0;

	//	TCCR0A=(1<<COM0A1)|(1<<COM0A0)|(1<<WGM00);

	/////////////////////////////////////////////////////////////////////
	
	//Analog Comparator initialization.

		ACSR|=(1<<ACD);		//Disable Comparator.

	////////////////////////////////////////////////////////////////////


}


int main()
{
	
	init();


	RELAY_ON;

	for(uint8_t i=0; i<3; i++)
		_delay_ms(1000);

	while(1)
	{
		
		while(IN1_OFF);

		while(IN1_ON);

		for(uint8_t i=0; i<5; i++)
			_delay_ms(1000);

		while(IN1_ON);


		if(IN1_OFF)
		{
			_delay_ms(1000);

			if(IN1_OFF)
			{
				RELAY_OFF;

				for(uint8_t i=TIME_OFF; i>0; i--)
					_delay_ms(1000);

				RELAY_ON;
			}

		}

		

		for(uint8_t i=0; i<3; i++)
			_delay_ms(1000);

		
	}

}





