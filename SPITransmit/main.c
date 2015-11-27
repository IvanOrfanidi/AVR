

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


	#define LED_GREEN PD3
	#define LED_YELLOW PD2
	#define BUZ PD5
	#define PORT_LED PORTD
	#define PORT_BUZ PORTD


	#define SS_PIN PB2

	#define BAUD_RATE 4800

#include "D:\AVR\Programs\GPS\SPI.h"

	uint8_t i;

void init()
{

// Port A initialization.
DDRA=(0<<PA0)|(0<<PA1)|(0<<PA2)|(0<<PA3)|(0<<PA4)|(0<<PA5)|(0<<PA6)|(0<<PA7);
PORTA=(0<<PA0)|(0<<PA1)|(0<<PA2)|(1<<PA3)|(1<<PA4)|(1<<PA5)|(1<<PA6)|(1<<PA7);
/////////////////////////////////////////////////////////////////////

// Port B initialization.
DDRB=(1<<PB0)|(1<<PB1)|(1<<SS_PIN)|(1<<PB3)|(1<<PB4)|(1<<PB5)|(1<<PB6)|(1<<PB7);
PORTB=(0<<PB0)|(0<<PB1)|(1<<SS_PIN)|(0<<PB3)|(0<<PB4)|(0<<PB5)|(0<<PB6)|(0<<PB7);
/////////////////////////////////////////////////////////////////////

// Port D initialization.
DDRD=(0<<PD0)|(0<<PD1)|(1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD6)|(0<<PD7);
PORTD=(0<<PD0)|(0<<PD1)|(0<<PD2)|(0<<PD3)|(0<<PD4)|(0<<PD5)|(0<<PD6)|(0<<PD7);
/////////////////////////////////////////////////////////////////////

//Analog Comparator initialization.

	ACSR|=_BV(ACD);		//Disable Comparator.

/////////////////////////////////////////////////////////////////////

}



int main(){

	init();

	PORT_LED|=(1<<LED_YELLOW);

	PORT_LED|=(1<<LED_GREEN);
//	PORT_BUZ|=(1<<BUZ);

	while(1)
	{
	}

}

void led_init()
{
	

}



