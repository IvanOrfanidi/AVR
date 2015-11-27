

//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>


// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <avr/wdt.h>
	#include "USART.h"
	#include "24c64.h"



	#define SLAVE_ADD_TWI 0b10100000
	
	#define LED_GREEN PD7
	#define LED_YELLOW PB0

	#define KEY0 PB1
	#define KEY1 PB2

	#define BUZ			(1<<PC0)
	#define PORT_BUZ	PORTC


	#define PORT_LED_GREEN PORTD
	#define PORT_LED_YELLOW PORTB

	#define PIN_KEY PINB

	uint16_t n;

	#define FOREVER for(;;)


void init(){

		cli();
		wdt_reset();
		wdt_enable(WDTO_8S);

// Port B initialization //

	DDRB=(1<<LED_YELLOW);

	PORTB=0xFF;

	PORTB&=~(1<<LED_YELLOW);

///////////////////////////////////////////////////////////////////

// Port C initialization //

	DDRC=0xFF;

	PORTC=0xFF;

///////////////////////////////////////////////////////////////////

// Port D initialization //

	DDRD=(1<<LED_GREEN);

	PORTD=0xFF;

	PORTD&=~(1<<LED_GREEN);

///////////////////////////////////////////////////////////////////

}


int main(){

	init();

	usart0_init();

	PORT_BUZ&=~BUZ;

	sei();


	FOREVER{

		asm volatile ("wdr");

	}


return 0;
}


