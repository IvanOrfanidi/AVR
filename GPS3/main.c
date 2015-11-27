




// 1-кр, 2-син, 3-сер, 4-роз, 5-фил, 6-чер, 7-бел/чер, 8-кор, 9-зел, 10-жел.//



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
	
	#include "GPS.h"
	#include "USART.h"

	#define PORT_LED PORTD
	#define LED_RUN (1<<PD6)

	uint8_t m;

	unsigned int timeout_gps;

void init()
{

	DDRA=0x00;
	PORTA=0xFF;

	DDRB=0x00;
	PORTB=0xFF;

	DDRC=0x00;
	PORTC=0xFF;

	DDRD=(1<<PD6);
	PORTD=(0<<PD6);

	DDRE=0x00;
	PORTE=0xFF;


//////////////////////////////////////////////



//////////////////////////////////////////////

}

////////////////////////////////////////////////////////////////////////////////////////////

int main(){
	
	init();

	usart0_init();

	gps_init();

	while(1)
	{
		uint8_t data_valid=gps_stat();

		if(data_valid==0)
		{
			timeout_gps=0;


			while(gps_status());
			
			usart_rx_time();

			usart_rx_positioning();

			usart0_clear_tx_buffer();


		}
		
		if(data_valid==1)
		{
			timeout_gps=0;
			PORT_LED&=~(LED_RUN);
		}

		if(data_valid==2)
		{
		
		}


	}

}




void usart_rx_time()
{

	sei();
	PORT_LED|=(LED_RUN);

	for(m=0; m<6; m++)
	{
		usart0_write(gps_time[m]);
		_delay_ms(5);
	}

		usart0_write(0x0D);
		_delay_ms(5);
		usart0_write(0x0A);
		_delay_ms(5);


	cli();

}

void usart_rx_positioning()
{		
	sei();
	PORT_LED|=(LED_RUN);

	for(m=0; m<sizeof(gps_latitude); m++)
	{
		usart0_write(gps_latitude[m]);
		_delay_ms(5);
	}

		usart0_write(0x0D);
		_delay_ms(5);
		usart0_write(0x0A);
		_delay_ms(5);


	for(m=0; m<sizeof(gps_longitude); m++)
	{
		usart0_write(gps_longitude[m]);
		_delay_ms(5);
	}

		usart0_write(0x0D);
		_delay_ms(5);
		usart0_write(0x0A);
		_delay_ms(5);


	cli();

}
