



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

	#include "GPS.h"
	#include "USART.h"

	#define LED_RUN 	(1<<PB4)
	#define J1 			(1<<PC3)
	#define INT			(1<<PD2)
	#define STAT_GPS	(1<<PA6)


	

void init(void)
{
	// Port A initialization.
	DDRA=0x00;
	PORTA=STAT_GPS;
	/////////////////////////////////////////////////////////////////////

	// Port B initialization.
	DDRB=LED_RUN;
	PORTB=0x00;
	/////////////////////////////////////////////////////////////////////

	// Port C initialization.
	DDRC=0x00;
	PORTC=J1;
	/////////////////////////////////////////////////////////////////////

	// Port D initialization.
	DDRD=0x00;
	PORTD=INT;
	/////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////

		WDTCR=(0<<WDCE)|(0<<WDE);
		asm volatile ("wdr");
		WDTCR=(0<<WDCE)|(0<<WDE)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0);


	////////////////////////////////////////////////////////////////////

}

int main()
{

	init();

	usart0_init();

	gps_init();

	while(1)
	{

		uint8_t data_valid=gps_stat();

		if(!data_valid)
		{
			while(gps_status());
			
			sei();
			for(uint8_t i=0; i<sizeof(gps_latitude); i++)
				usart0_write(gps_latitude[i]);

			usart0_write(0x0D);
			usart0_write(0x0A);
			while(usart0_busy_wait());
			usart0_clear_tx_buffer();
			cli();

		}
		
	}





}
