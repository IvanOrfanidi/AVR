


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
	
	#include "GPS.h"
	#include "USART.h"

	#define PORT_LED PORTD
	#define LED_RUN (1<<PD7)

	#define PORT_DISP PORTB
	#define DISP_EN (1<<PB0)
	

void init()
{

	DDRA=0x00;
	PORTA=0x00;

	DDRB=DISP_EN;
	PORTB=DISP_EN;

	DDRC=0x00;
	PORTC=0x00;

	DDRD=LED_RUN;
	PORTD=0x00;

	DDRE=0x00;
	PORTE=0x00;



	//////////////////////////////////////////////

	//Timer1 initialization.

	////////////////////////////////////////////////////////////////////


	//WATCHDOG initialization.

	wdt_reset();

	wdt_enable(WDTO_2S);

	////////////////////////////////////////////////////////////////////

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int Time_Zone_Update(void)
{
			if(gps_time[0]=='0' && gps_time[1]=='0')
			{
				gps_time[0]='0';
				gps_time[1]='4';
				return 0;

			}

			if(gps_time[0]=='0' && gps_time[1]=='1')
			{
				gps_time[0]='0';
				gps_time[1]='5';
				return 0;
			}


			if(gps_time[0]=='0' && gps_time[1]=='2')
			{
				gps_time[0]='0';
				gps_time[1]='6';
				return 0;
			}


			if(gps_time[0]=='0' && gps_time[1]=='3')
			{
				gps_time[0]='0';
				gps_time[1]='7';
				return 0;
			}

			if(gps_time[0]=='0' && gps_time[1]=='4')
			{
				gps_time[0]='0';
				gps_time[1]='8';
				return 0;
			}

			if(gps_time[0]=='0' && gps_time[1]=='5')
			{
				gps_time[0]='0';
				gps_time[1]='9';
				return 0;
			}


			if(gps_time[0]=='0' && gps_time[1]=='6')
			{
				gps_time[0]='1';
				gps_time[1]='0';
				return 0;
			}

			if(gps_time[0]=='0' && gps_time[1]=='7')
			{
				gps_time[0]='1';
				gps_time[1]='1';
				return 0;
			}

			if(gps_time[0]=='0' && gps_time[1]=='8')
			{
				gps_time[0]='1';
				gps_time[1]='2';
				return 0;
			}

			if(gps_time[0]=='0' && gps_time[1]=='9')
			{
				gps_time[0]='1';
				gps_time[1]='3';
				return 0;
			}

			if(gps_time[0]=='1' && gps_time[1]=='0')
			{
				gps_time[0]='1';
				gps_time[1]='4';
				return 0;
			}
			if(gps_time[0]=='1' && gps_time[1]=='1')
			{
				gps_time[0]='1';
				gps_time[1]='5';
				return 0;
			}
			if(gps_time[0]=='1' && gps_time[1]=='2')
			{
				gps_time[0]='1';
				gps_time[1]='6';
				return 0;
			}

			if(gps_time[0]=='1' && gps_time[1]=='3')
			{
				gps_time[0]='1';
				gps_time[1]='7';
				return 0;
			}
			if(gps_time[0]=='1' && gps_time[1]=='4')
			{
				gps_time[0]='1';
				gps_time[1]='8';
				return 0;
			}
			if(gps_time[0]=='1' && gps_time[1]=='5')
			{
				gps_time[0]='1';
				gps_time[1]='9';
				return 0;
			}
			if(gps_time[0]=='1' && gps_time[1]=='6')
			{
				gps_time[0]='2';
				gps_time[1]='0';
				return 0;
			}
			if(gps_time[0]=='1' && gps_time[1]=='7')
			{
				gps_time[0]='2';
				gps_time[1]='1';
				return 0;
			}

			if(gps_time[0]=='1' && gps_time[1]=='8')
			{
				gps_time[0]='2';
				gps_time[1]='2';
				return 0;
			}
			if(gps_time[0]=='1' && gps_time[1]=='9')
			{
				gps_time[0]='2';
				gps_time[1]='3';
				return 0;
			}

			if(gps_time[0]=='2' && gps_time[1]=='0')
			{
				gps_time[0]='0';
				gps_time[1]='0';
				return 0;
			}

			if(gps_time[0]=='2' && gps_time[1]=='1')
			{
				gps_time[0]='0';
				gps_time[1]='1';
				return 0;
			}
			if(gps_time[0]=='2' && gps_time[1]=='2')
			{
				gps_time[0]='0';
				gps_time[1]='2';
				return 0;
			}
			if(gps_time[0]=='2' && gps_time[1]=='3')
			{
				gps_time[0]='0';
				gps_time[1]='3';
			}

	return 0;

}

////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	
	init();

	gps_init();

	usart1_init();

	_delay_ms(1000);
	
	sei();

	while(1)
	{

		wdt_reset();

		uint8_t data_valid=gps_stat();

		if(!(data_valid))
		{
			cli();

			PORT_LED|=LED_RUN;

			while(gps_status());

			Time_Zone_Update();

			sei();

			usart1_clear_tx_buffer();
			usart1_clear_rx_buffer();



			// Transmit TIME // 
			usart1_write(0x01);
			_delay_ms(USART1_DELAY_MS);

			for(uint8_t i=0; i<sizeof(gps_time); i++)
			{
				usart1_write(gps_time[i]);
				_delay_ms(USART1_DELAY_MS);
			}
			//////////////////

			/*
			// Transmit LATITUDE // 
			usart1_write(0x02);
			_delay_ms(USART1_DELAY_MS);

			for(uint8_t i=0; i<sizeof(gps_latitude); i++)
			{
				usart1_write(gps_latitude[i]);
				_delay_ms(USART1_DELAY_MS);
			}
			//////////////////////

			// Transmit LONGITUDE // 
			usart1_write(0x03);
			_delay_ms(USART1_DELAY_MS);

			for(uint8_t i=0; i<sizeof(gps_longitude); i++)
			{
				usart1_write(gps_longitude[i]);
				_delay_ms(USART1_DELAY_MS);
			}
			//////////////////////
			*/


			PORT_LED&=~LED_RUN;
			_delay_ms(1000);


		}	
		
	}
}

