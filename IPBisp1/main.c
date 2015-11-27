

//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <avr/wdt.h>
	#include <avr/eeprom.h>

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>

	#include "USART.h"
	#include "MODBUS_SLAVE.h"

	#define PWM				(1<<PD6)

	#define DEVICE_ID 0x0101

	#define ADD_PWM_VALUE 0x0001

	#define TIME_SYMBOL 600

	#define TIMEOUT_RESET 1500

	uint8_t usart_byte;
	uint16_t count_start_timer;

	uint16_t time_symbol;

	
	uint8_t slave_id;

void init()
{

	// Port B initialization //

	DDRB=0x00;

	PORTB=0xFF;

	///////////////////////////


	// Port D initialization //

	DDRD=(1<<PD6) | (1<<PD2);

	PORTD=0x00;

	///////////////////////////

	// Port C initialization //

	DDRC=0x00;

	PORTC=(1<<PC1) | (1<<PC0);

	///////////////////////////

	//Timer0 initialization.

	TCCR0B=(0<<CS02)|(1<<CS01)|(0<<CS00);	//Timer1 clock on.

	OCR0A=eeprom_read_byte(ADD_PWM_VALUE);

	TCCR0A=(1<<COM0A1)|(1<<COM0A0)|(1<<WGM00);
	/////////////////////////////////////////////////////////////////////

	//Timer1 initialization.
	
	OCR1A=20;

	TIMSK1|=(1<<OCIE1A);	

	/////////////////////////////////////////////////////////////////////

	//Watchdog initialization.

	//!!!WDTON fuse set to “0“ means programmed and “1“ means unprogrammed!!!//

	cli();
	wdt_reset();		//Watchdog Reset.
	wdt_enable(WDTO_8S);

	////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////////////////

}

uint8_t select_slave_id(void)
{
	return ~((PINB) | (PINC<<6));
}


int main()
{

	init();

	usart0_init();

	registers[1]=DEVICE_ID;

	slave_id=select_slave_id();

	sei();

	uint16_t counter_reset=TIMEOUT_RESET;
	uint8_t connect_modbus=1;

	while(1)
	{

		if((usart0_rx_len()) && !(TCCR1B))
		{
			counter_reset=TIMEOUT_RESET;
			connect_modbus=0;

			if(usart0_rx_len()==usart_byte)
			{
				count_start_timer++;
			}
			else
				{
					count_start_timer=0;
					usart_byte=usart0_rx_len();
				}
		
			if(count_start_timer==TIME_SYMBOL)	//800 for Baud Rate 
			{
				usart_byte=0;
				count_start_timer=0;
				TCCR1B=(1<<CS12)|(0<<CS11)|(0<<CS10);
			}

			wdt_reset();		//Watchdog Reset.
		}

		if(counter_reset)
		{
			counter_reset--;

			if(connect_modbus)
				counter_reset=TIMEOUT_RESET;

			wdt_reset();
		}
	}
}


// INTERRUPT //

ISR(TIMER1_COMPA_vect)
{
	wdt_reset();		//Watchdog Reset.

	sei();							//Enable Interrupt.

	TCCR1B=0x00; 					//Stop Timer2.
	TCNT1=0x00;						//Reset Timer2.

	registers[4]=OCR0A;

	if(!(modbus_slave(slave_id)))
	{
		if(registers[4]>200)
			registers[4]=255;

			OCR0A=registers[4];
			wdt_reset();		//Watchdog Reset.
			eeprom_update_byte(ADD_PWM_VALUE, OCR0A);
	}
	else
		{
			registers[4]=OCR0A;
		}

	wdt_reset();		//Watchdog Reset.

}

