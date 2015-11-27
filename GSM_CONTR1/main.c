


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

//User Headers
	#include "USART.h"


	// Дефайны для платы Olimex AVR-GSM //
	#define  USB_RST		(1<<PB2)		//Port OUTPUT.
	#define  STATUS			(1<<PB3)		//Port INPUT.


	#define  DCD			(1<<PA4)		//Port INPUT.
	#define  DTR			(1<<PA5)		//Port OUTPUT.
	#define  CTS			(1<<PA6)		//Port INPUT.
	#define  RTS			(1<<PA7)		//Port OUTPUT.

	#define RELAY1			(1<<PC7)
	#define RELAY2			(1<<PC6)
	#define PORT_OUT		PORTC


	#define  PWR_KEY		(1<<PA2)		//Port OUTPUT(-_-).

	#define  LED			(1<<PD7)		//Port OUTPUT.
	#define  PORT_LED		PORTD
	#define  PIN_KEY		PIND
	#define	 KEY			(1<<PD3)		//Port INPUT.
	#define	 BUZ			(1<<PD4)		//Port OUTPUT.

	#define	 USB_PRESENT	(1<<PD5)		//Port INPUT.

	#define BUZ_ON		PORTD|=BUZ;
	#define BUZ_OFF		PORTD&=~BUZ;


void init(void)
{


	// Port A initialization

	DDRA = RTS | DTR | PWR_KEY;

	PORTA = 0xFF & (0<<PWR_KEY);

	//////////////////////////////////////////////////////////////////////////////


	// Port B initialization

	DDRB = 0x00;

	PORTB = STATUS;

	//////////////////////////////////////////////////////////////////////////////


	// Port C initialization

	DDRC = RELAY1 | RELAY2;

	PORTC=0x00;

	//////////////////////////////////////////////////////////////////////////////



	// Port D initialization

	DDRD = LED | BUZ;

	PORTD = KEY | USB_PRESENT | LED;

	//////////////////////////////////////////////////////////////////////////////

}

void atv_off(void)
{
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	char atv0[]={'A','T','V', '0', 0x0D, 0x0A};

	for(uint8_t i=0; i<sizeof(atv0); i++)
		usart0_write(atv0[i]);
	
}

int at_ok()
{
		usart0_clear_rx_buffer();
		usart0_clear_tx_buffer();

		char at[]={'A', 'T', 0x0D, 0x0A};
		for(uint8_t n=0; n<sizeof(at); n++)		//AT.
			usart0_write(at[n]);				



		while(!(usart0_rx_len()));

		if(usart0_read()=='0') return 0;
		
		usart0_clear_rx_buffer();
		return 1;

}

int main()
{

	init();

	usart0_init();

	sei();



	_delay_ms(1000);
	_delay_ms(1000);

	atv_off();	//Отключаем словесный формат ответа. Теперь будут приходить коды ответа.

	_delay_ms(1000);


	while(at_ok())			//Проверяем готовность модуля командой AT.
	{
		PORTA|=(1<<PWR_KEY);
		_delay_ms(2000);
		PORTA&=(0<<PWR_KEY);
		_delay_ms(5000);
		PORTA|=(1<<PWR_KEY);

		atv_off();	//Отключаем словесный формат ответа. Теперь будут приходить коды ответа.

		_delay_ms(1000);

		at_ok();
									
		
	}



	BUZ_ON;	while(1);

	while(1)
	{
	}
}
