


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

	#define KEY0	(1<<PB3)
	#define KEY1	(1<<PB4)
	#define PIN_KEY	PINB

	#define BUZ0		(1<<PB1)
	#define BUZ1		(1<<PB2)
	#define PORT_BUZ	PORTB


	#define PWM		PB0

	#define ADD_PWM_VALUE	0x0000
	#define		PAUSE			15

void init()
{

	// Port B initialization //

		DDRB=(1<<PB0)|(1<<PB1)|(1<<PB2)|(0<<PB3)|(0<<PB4);

		PORTB=(0<<PB0)|(0<<PB1)|(0<<PB2)|(1<<PB3)|(1<<PB4);

	///////////////////////////

	//Timer0 initialization.


		OCR0A=eeprom_read_byte(ADD_PWM_VALUE);

		TCCR0B=(0<<CS02)|(0<<CS01)|(1<<CS00);	//Timer1 clock on.

		TCCR0A=(1<<COM0A1)|(1<<COM0A0)|(1<<WGM00);

	/////////////////////////////////////////////////////////////////////
	
	//Analog Comparator initialization.

		ACSR|=(1<<ACD);		//Disable Comparator.

	/////////////////////////////////////////////////////////////////////

	//INIT WATCHDOG//

	//!!!WDTON fuse set to “0“ means programmed and “1“ means unprogrammed!!!//

		cli();
		wdt_reset();		//Watchdog Reset.
		wdt_enable(WDTO_4S);

	////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////

}



int main()
{

	init();

	
	while(1)
	{
		if(!(PIN_KEY & KEY0))
		{
			_delay_ms(PAUSE);

			if(!(PIN_KEY & KEY0))
			{

				if(OCR0A!=0)
				{
					OCR0A--;

					eeprom_update_byte(ADD_PWM_VALUE, OCR0A);
				}
			}
		}

		if(!(PIN_KEY & KEY1))
		{

			_delay_ms(PAUSE);

			if(!(PIN_KEY & KEY1))
			{
				if(OCR0A<230)
				{
					OCR0A++;

					
					eeprom_update_byte(ADD_PWM_VALUE, OCR0A);

				}
			}
		}


		if((OCR0A==0) || (OCR0A==230))
		{
			PORT_BUZ|=BUZ0;
			PORT_BUZ|=BUZ1;
		}
		else
		{
			PORT_BUZ&=~BUZ0;
			PORT_BUZ&=~BUZ1;
		}


		wdt_reset();		//Watchdog Reset.
	}



}
