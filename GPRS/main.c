


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
	#include "GPRS.h"
	#include "ATCOMMANDS.h"


	#define  STATUS_GSM		(1<<PB3)		//Pin OUTPUT.
	#define  FT232RL_RST	(1<<PB2)		//Pin OUTPUT.
	#define	 EXT_INT1		(1<<PB0)

	#define  PWR_KEY		(1<<PA2)		//Pin OUTPUT(-_-).
	#define  DCD			(1<<PA4)		//Pin INPUT.
	#define  DTR			(1<<PA5)		//Pin OUTPUT.
	#define  CTS			(1<<PA6)		//Pin INPUT.
	#define  RTS			(1<<PA7)		//Pin OUTPUT.

	#define RELAY1			(1<<PC7)
	#define RELAY2			(1<<PC6)
	#define PORT_OUT		PORTC

	#define	 RI				(1<<PD6)
	#define  USB_PRESENT	(1<<PD5)
	#define	 EXT_INT0		(1<<PD2)
	#define	 BUZ			(1<<PD4)
	#define  LED			(1<<PD7)		//Pin OUTPUT.
	#define	 KEY0			(1<<PD3)		//Pin INPUT.
	#define  PORT_LED		PORTD			//Port LED.
	#define  PIN_KEY		PIND			//Pins KEY0.


void init(void){

	// Port A initialization

	DDRA=(PWR_KEY)|(DTR)|(RTS);

	PORTA=(DCD)|(CTS);

	//////////////////////////////////////////////////////////////////////////////

	// Port B initialization

	DDRB=(FT232RL_RST);
	PORTB=(EXT_INT1) | (STATUS_GSM) | (FT232RL_RST);

	//////////////////////////////////////////////////////////////////////////////

	// Port C initialization

	DDRC=(RELAY1) | (RELAY2);
	PORTC=0x00;

	//////////////////////////////////////////////////////////////////////////////

	// Port D initialization

	DDRD=(LED) | (BUZ);
	PORTD=(LED) | (KEY0) | (EXT_INT0) | (RI) | (USB_PRESENT);

	//////////////////////////////////////////////////////////////////////////////

	//Analog Comparator initialization.

	ACSR|=_BV(ACD);		//Disable Comparator.

	/////////////////////////////////////////////////////////////////////

}

int main()
{

	init();

	usart0_init();
	
//	_delay_ms(1000);

	sei();

	
	
	while(1)
	{
		at();

		char ok[]="OK";
 
		if(while_usart(ok)==0) PORT_LED&=~(LED); else PORT_LED|=LED;

		_delay_ms(1000);

		void usart0_clear_tx_buffer();
		void usart0_clear_rx_buffer();

	}

}

