

//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>


// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

	#include "USART.h"
	#include "24c16.h"
//	#include "24c64.h"
	
	#define LED_GREEN (1<<PB0)
	#define LED_YELLOW (1<<PD3)

	#define KEY0 (1<<PB1)
	#define KEY1 (1<<PB2)

	#define BUZ			(1<<PC0)
	#define PORT_BUZ	 PORTC

	#define WP_EEP (1<<PC2)		//Pin OUTPUT Write Protect Serial EEPROM I2C.


	#define PORT_LED_YELLOW PORTD
	#define PORT_LED_GREEN  PORTB

	#define PIN_KEY PINB


void init(){

// Port B initialization //

	DDRB = LED_GREEN;

	PORTB = KEY0 | KEY1;

///////////////////////////////////////////////////////////////////

// Port C initialization //

	DDRC=BUZ | WP_EEP;

	PORTC=0;

///////////////////////////////////////////////////////////////////

// Port D initialization //

	DDRD = LED_YELLOW;

	PORTD=0x00;

///////////////////////////////////////////////////////////////////

}


int main(){

	init();

	usart0_init();

	EEOpen(0x40);

	sei();


	while(1)
	{

		if(!(PIN_KEY & KEY0))
		{
			PORT_LED_GREEN|=LED_GREEN;

			for(uint16_t i=0; i<256; i++)
			{
				usart0_write(EEReadByte(i));
				_delay_ms(1);
			}

			PORT_LED_GREEN&= ~LED_GREEN;

		}


		usart0_clear_tx_buffer();


		if(!(PIN_KEY & KEY1))
		{

			PORT_LED_YELLOW|=LED_YELLOW;

			uint8_t count = 0xFF;
			for(uint16_t i=0; i<256; i++)
			{
				EEWriteByte(i, count);
				count--;
			}

			PORT_LED_YELLOW&= ~LED_YELLOW;
		}


	}


return 0;
}
/*


*/
