


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
	#include "SPI.h"


	#define TIMEOUT_GPS	50

	#define KEY0 		PA0
	#define PIN_KEY 	PINA
	#define LED_RED 	PB2
	#define LED_GREEN 	PB1
	#define LED_YELLOW	PB0
	#define PORT_LED	PORTB

	#define BUZ			PD4		//Pin Buzzer.
	#define PORT_BUZ	PORTD	

	uint8_t m;

	unsigned int timeout_gps;



void init(void)
{
	
	// Port A initialization.
	DDRA=(0<<KEY0);
	PORTA=(1<<KEY0);
	/////////////////////////////////////////////////////////////////////

	// Port B initialization.
	DDRB=(1<<LED_GREEN)|(1<<LED_YELLOW)|(1<<LED_RED);
	PORTB=(0<<LED_GREEN)|(0<<LED_YELLOW)|(0<<LED_RED);
	/////////////////////////////////////////////////////////////////////

	// Port C initialization.
	DDRC=0x00;
	PORTC=0x00;
	/////////////////////////////////////////////////////////////////////

	// Port D initialization.
	DDRD=(1<<BUZ);
	PORTD=(0<<BUZ);
	/////////////////////////////////////////////////////////////////////

	// Timer0 initialization.

	TCCR0=(0<<CS02)|(0<<CS01)|(0<<CS00);

	TIMSK|=_BV(TOIE0);
	////////////////////////////////////////////////////////////////////

		WDTCR=(1<<WDTOE)|(1<<WDE);
		asm volatile ("wdr");
		WDTCR=(1<<WDTOE)|(1<<WDE)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0);


	////////////////////////////////////////////////////////////////////

}

int main(){
	
	init();

	led_init();

	gps_init();

	spi_master_init();

	while(1)
	{
		asm volatile ("wdr");
		int data_valid=gps_stat();

		if(data_valid==0)
		{
			timeout_gps=0;
			PORT_LED&=~(1<<LED_YELLOW);
			PORT_LED|=(1<<LED_GREEN);
			PORT_LED&=~(1<<LED_RED);

			spi_time();
			//spi_positioning();
			while(gps_status())	asm volatile ("wdr");

		}
		
		if(data_valid==1)
		{
			timeout_gps=0;
			PORT_LED|=(1<<LED_YELLOW);
			PORT_LED&=~(1<<LED_GREEN);
			PORT_LED&=~(1<<LED_RED);

		}

		if(data_valid==2)
		{
			if(timeout_gps>=TIMEOUT_GPS)
			{
				PORT_LED&=~(1<<LED_YELLOW);
				PORT_LED&=~(1<<LED_GREEN);
				PORT_LED|=(1<<LED_RED);

				if(timeout_gps==TIMEOUT_GPS)
				{
					PORT_BUZ=(1<<BUZ);
					_delay_ms(1000);
					PORT_BUZ=(0<<BUZ);

				}

				if(timeout_gps==TIMEOUT_GPS*2) timeout_gps=TIMEOUT_GPS+1;

			}
			timeout_gps++;
		
		}


	}

}


int led_init()
{

	PORTB=(1<<LED_RED);
	if(!(PIN_KEY & (1<<KEY0))) PORT_BUZ=(1<<BUZ);

	_delay_ms(500);

	PORTB=(0<<LED_RED);
	PORT_BUZ=(0<<BUZ);

return 0;
}


void spi_time()
{

	
	for(m=0; m<sizeof(gps_time); m++)
	{
		SPDR=gps_time[m];
		asm volatile ("wdr");
		while(!(SPSR & (1<<SPIF)));
	}

}




void spi_positioning()
{
	for(m=0; m<sizeof(gps_latitude); m++)
	{
		SPDR=gps_latitude[m];

		while(!(SPSR & (1<<SPIF)));	
	}


		for(m=0; m<sizeof(gps_longitude); m++)
	{
		SPDR=gps_time[m];

		while(!(SPSR & (1<<SPIF)));	
	}

}
