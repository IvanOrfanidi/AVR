

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
	#include "TWI.h"

	#define BUZ (1<<PC0)		//Pin OUTPUT Buzzer.
	#define WP_EEP (1<<PC2)		//Pin OUTPUT Write Protect Serial EEPROM I2C.

	#define LED_GREEN (1<<PB0)	//Pin OUTPUT Led Green.
	#define LED_YELLOW (1<<PD3)	//Pin OUTPUT Led Yellow.

	#define KEY1 (1<<PB2)		//Pin INPUT Key1.
	#define KEY0 (1<<PB1)		//Pin INPUT Key2.
	#define IN_RTC (1<<PD2)		//Pin INPUT Interrupt RTC.
	#define T_OUT (1<<PC3)		//Pin INPUT Thermostat Output Signal.


	#define PORT_LED_GREEN PORTB
	#define PORT_LED_YELLOW PORTD

	#define PIN_KEY PINB
	
	#define SLAVE_ADDR_EEP 0b10100000

	#define ADDR_SEC 	0x02
	#define ADDR_MIN 	0x03
	#define ADDR_HOUR 	0x04
      

void init()
{
	// Port C initialization //
	DDRC=(BUZ)|(WP_EEP);
	PORTC=(T_OUT);

		// Port D initialization //
	DDRD=(LED_YELLOW);
	PORTD=(LED_YELLOW)|(IN_RTC);


	// Port B initialization //
	DDRB=(LED_GREEN);
	PORTB=(KEY0)|(KEY1);

	// Init TWI //


}


int main()
{

	init();

	usart0_init();

	TWIOpen(0x40);

	sei();


	while(1)
	{
		
		if(!(PIN_KEY & KEY0))
		{
			PORT_LED_GREEN|=LED_GREEN;

			for(uint16_t i=0; i<256; i++)
			{
				usart0_write(TWIReadByte(SLAVE_ADDR_EEP, 1, i));
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
				TWIWriteByte(SLAVE_ADDR_EEP, 1, i, count);
				count--;
			}

			PORT_LED_YELLOW&= ~LED_YELLOW;
		}
	}


}
