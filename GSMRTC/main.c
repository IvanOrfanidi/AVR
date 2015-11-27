

//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/eeprom.h>
	#include <avr/pgmspace.h>
	#include <util/delay.h>


// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

	#include "USART.h"
	#include "GSMRTC.h"
	#include "GSM.h"

	#define  FT232RL_RST	(1<<PB2)		//Port OUTPUT.
	#define  DCD			PA4				//Port INPUT.
	#define  DTR			(1<<PA5)		//Port OUTPUT.
	#define  CTS			PA6				//Port INPUT.
	#define  RTS			(1<<PA7)		//Port OUTPUT.

	#define RELAY1			(1<<PC7)
	#define RELAY2			(1<<PC6)
	#define PORT_OUT		PORTC


	#define  PWR_KEY		PA2				//Port OUTPUT(-_-).
	#define  LED			PD7				//Port OUTPUT.
	#define  PORT_LED		PORTD
	#define  PIN_KEY		PIND
	#define	 KEY0			PD3



void init(void){

// Port A initialization

DDRA=(1<<PA7)|(0<<PA6)|(1<<PA5)|(0<<PA4)|(0<<PA3)|(1<<PA2)|(0<<PA1)|(0<<PA0);

PORTA=0xFF & (0<<PWR_KEY);

//////////////////////////////////////////////////////////////////////////////

PORTB=(1<<PB7);
DDRB=0x00;


PORTC=0x00;
DDRC=(RELAY1)|(RELAY2);

DDRD=(1<<LED)|(1<<PD4);
PORTD=(1<<LED);


//Analog Comparator initialization.

	ACSR|=_BV(ACD);		//Disable Comparator.

/////////////////////////////////////////////////////////////////////

//Init Timer0//

TIMSK|=(1<<TOIE0);

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

//Init Timer2//

TIMSK|=(1<<TOIE2);

/////////////////////////////////////////////////////////////////////

}

int main()
{
	init();

	usart0_init();

	sei();

	_delay_ms(1000);
	char ate="ATE0\n";
	char at="AT\n";
	char ok="OK";

//	usart0_write_str(ate);

	_delay_ms(1000);

	Begin0:
	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	usart0_write_str(at);

	while(usart0_wait(ok))
	{
		_delay_ms(1000);
		_delay_ms(1000);
		goto Begin0;
	}


	PORT_LED&=~(1<<LED);

	while(1)
	{
	
			
	}

}
