




//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>


// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

	uint16_t Pause;

void init()
{

	// Port B initialization //

		DDRB=(1<<PB0)|(0<<PB1)|(0<<PB2)|(0<<PB3)|(0<<PB4);

		PORTB=(0<<PB0)|(0<<PB1)|(0<<PB2)|(0<<PB3)|(0<<PB4);

	///////////////////////////

	//Timer1 initialization.

	//	TCCR0B=(0<<CS12)|(0<<CS11)|(1<<CS10);	//Timer1 clock on.

	//	OCR0A=0;

	//	TCCR0A=(1<<COM0A1)|(1<<COM0A0)|(1<<WGM00);

	/////////////////////////////////////////////////////////////////////
	
	//Analog Comparator initialization.

		ACSR|=(1<<ACD);		//Disable Comparator.

	/////////////////////////////////////////////////////////////////////

	//ADC initialization.

		sei();				//Enable Interrupt for ADC seting.

		DIDR0=(1<<ADC3D) | (1<<ADC2D);	//Disable digital input signal.


		ADCSRA=(1<<ADEN);	//Enable ADC.

		ADCSRA=(1<<ADEN) | (1<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);		
 	
		ADCSRB=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

		//ADMUX=(0<<REFS0) | (0<<REFS1) | (1<<ADLAR);		//Uref=AVcc.

		cli();				//Disable Interrupt.

	/////////////////////////////////////////////////////////////////////


}


int main()
{
	
	init();

//	sei();

	Pause=1;

	uint8_t ValueRes;
	uint8_t ValueFot;


	while(1)
	{

		ADMUX=(0<<MUX3) | (0<<MUX2) | (1<<MUX1) | (1<<MUX0) |(1<<ADLAR);

		
		ADCSRA|=(1<<ADSC);				//Start ADC.

		while (ADCSRA & (1<<ADSC)); 	//Wait for the AD conversion to complete.

		ValueRes=ADCH;





		ADMUX=(0<<MUX3) | (0<<MUX2) | (1<<MUX1) | (0<<MUX0) |(1<<ADLAR);

		
		ADCSRA|=(1<<ADSC);				//Start ADC.

		while (ADCSRA & (1<<ADSC)); 	//Wait for the AD conversion to complete.

		ValueFot=ADCH;

		if(ValueFot<ValueRes)
		{
			PORTB|=(1<<PB0);
		}
	
			//if(OCR0A<255)
			//	OCR0A++;

		if(ValueFot>ValueRes)
		{
		PORTB&=~(1<<PB0);
			//if(OCR0A>0)
			//	OCR0A--;
		}

		//_delay_ms(Pause);
	}



}

