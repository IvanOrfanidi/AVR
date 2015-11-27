


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
	#include <math.h>

	#include "24C64.h"

	#define SLAVE_ADDR_EEP 0b10100000


	// Выбор режима загрузчика. !!! Или BOOT_SIMPLE, или BOOT_WAIT.

	#define BOOT_SIMPLE		0	// Вход по кнопке.
	#define BOOT_WAIT		1	// Вход по ожиданию.

	////////////////////////////////////////////////


	// Выдержка для BOOT_WAIT mode в секундах, при BOOT_WAIT=0 игнорируется.
	#define WAIT_VALUE 5

	////////////////////////////////////////////////


	// Выбор порта для кнопки входа в загрузчик. //

	#define INPORT		PORTB
	#define INLDDR		DDRB
	#define INLPIN		PINB
	#define INLPNUM		PINB1

	////////////////////////////////////////////////


	// Выбор порта для индикатора работы загрузчика.

	#define ENABLE_BOOT_LED		0
	#define OUTPORT			PORTB
	#define OUTDDR			DDRB
	#define OUTPNUM			PINB0

	////////////////////////////////////////////////

	
	#define LED_GREEN PD7
	#define LED_YELLOW PB0

	#define KEY0 (1<<PB1)
	#define KEY1 (1<<PB2)

	#define BUZ			(1<<PC0)
	#define PORT_BUZ	PORTC

	#define WP_EEP (1<<PC2)		//Pin OUTPUT Write Protect Serial EEPROM I2C.


	#define PORT_LED_GREEN PORTD
	#define PORT_LED_YELLOW PORTB

	#define PIN_KEY PINB


void init(){

// Port B initialization //

	DDRB=(1<<LED_YELLOW);

	PORTB=0xFF;

	PORTB&=~(1<<LED_YELLOW);

///////////////////////////////////////////////////////////////////

// Port C initialization //

	DDRC=BUZ | WP_EEP;

	PORTC=0;

///////////////////////////////////////////////////////////////////

// Port D initialization //

	DDRD=(1<<LED_GREEN);

	PORTD=0xFF;

	PORTD&=~(1<<LED_GREEN);

///////////////////////////////////////////////////////////////////

}


int main(){

	init();

	EEOpen();

	sei();


	while(1)
	{


		if(!(PIN_KEY & KEY0))
		{
			PORT_BUZ|=BUZ;
			
			uint8_t temp;

			for(uint16_t i=0; i<255; i++)
			{
				temp=EEReadByte(SLAVE_ADDR_EEP, i);
				_delay_ms(1);
			}

			PORT_BUZ&=~BUZ;

		}


		if(!(PIN_KEY & KEY1))
		{

			PORT_BUZ|=BUZ;

			for(uint16_t i=0; i<255; i++)
			{
				EEWriteByte(SLAVE_ADDR_EEP, i,i);
			}

			PORT_BUZ&=~BUZ;
		}


	}


}
