

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

	#include "USART.h"
	#include "TWI.h"
	#include "PCF8583.h"


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



void init()
{
	// Port C initialization //
	DDRC=(BUZ)|(WP_EEP);
	PORTC=(T_OUT);

		// Port D initialization //
	DDRD=(LED_YELLOW);
	PORTD=(IN_RTC);


	// Port B initialization //
	DDRB=(LED_GREEN);
	PORTB=(KEY0)|(KEY1);
	
	//Timer1 initialization.

//	TCCR1B=(1<<CS12)|(0<<CS11)|(0<<CS10);	//Ft=Fcpu/256, Ft=28 800
	TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);	//Timer1 clock off.

	OCR1A=10000;


	TIMSK=(1<<OCIE1A);	//Interrupt Timer1 COMPA.

}



int main()
{
	init();

	usart0_init();

	TWIOpen(0x40);
	
	sei();
		
	while(1)
	{
		if(!(PIN_KEY & KEY1))
		{
			PORT_LED_GREEN|=LED_GREEN;

			char data[17];	// yy/mm/dd,hh:mm:ss

			GetDataTimeRTC(data, 17);

			for(uint8_t i=0; i<sizeof(data); i++) usart0_write(data[i]);

			usart0_write(0x0D);
			usart0_write(0x0A);

			_delay_ms(100);
			
		}

		else

		{
			PORT_LED_GREEN&=~LED_GREEN;
		}

		if(!(PIN_KEY & KEY0))
		{
			PORT_LED_YELLOW|=LED_YELLOW;

			char data[]={'1','2','/','1','1','/','1','0',',','2','1',':','2','0',':','0','0'};	// yy/mm/dd,hh:mm:ss

			SetDataTimeRTC(data);

			//char data[]={'1','8',':','0','0',':','0','0'};
			//SetTimeRTC(data);

		}

		else

		{
			PORT_LED_YELLOW&=~LED_YELLOW;
		}

		
	}

}
