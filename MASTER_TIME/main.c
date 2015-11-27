



//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <avr/eeprom.h>
	#include <avr/wdt.h>
	#include <util/crc16.h>


// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>
	
	#include "USART.h"

	#define PORT_LED PORTD
	#define LED_RUN (1<<PD7)

	#define PORT_DISP PORTB
	#define DISP_EN (1<<PB0)	


uint8_t msg0[]={0x01, 0x03, 0x00, 0x01, 0x00, 0x03, 0x54, 0x0B};

uint8_t Time[]={0x30, 0x30, ':', 0x30, 0x30, ':', 0x30, 0x30};


void init()
{

	DDRA=0x00;
	PORTA=0x00;

	DDRB=DISP_EN;
	PORTB=DISP_EN;

	DDRC=0x00;
	PORTC=0x00;

	DDRD=LED_RUN;
	PORTD=0x00;//LED_RUN;

	DDRE=0x00;
	PORTE=0x00;


	//////////////////////////////////////////////

	//Timer1 initialization.

	TCCR1B=(1<<CS12)|(0<<CS11)|(0<<CS10);

	OCR1A=31250;

	TIMSK|=(1<<OCIE1A);	//Interrupt Timer1 COMPA.
	////////////////////////////////////////////////////////////////////


	//WATCHDOG initialization.

	WDTCR=(0<<WDCE)|(0<<WDE);									//Enable WATCHDOG.

	WDTCR=(0<<WDCE)|(0<<WDE)|(0<<WDP2)|(0<<WDP1)|(0<<WDP0);	//WATCHDOG clock 2100ms.


	////////////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////////////
	

}

////////////////////////////////////////////////////////////////////////////////////////////




int Time_Zone_Update(char *time)
{
			if(time[0]=='0' && time[1]=='0')
			{
				time[0]='0';
				time[1]='4';
				return 0;

			}

			if(time[0]=='0' && time[1]=='1')
			{
				time[0]='0';
				time[1]='5';
				return 0;
			}


			if(time[0]=='0' && time[1]=='2')
			{
				time[0]='0';
				time[1]='6';
				return 0;
			}


			if(time[0]=='0' && time[1]=='3')
			{
				time[0]='0';
				time[1]='7';
				return 0;
			}

			if(time[0]=='0' && time[1]=='4')
			{
				time[0]='0';
				time[1]='8';
				return 0;
			}

			if(time[0]=='0' && time[1]=='5')
			{
				time[0]='0';
				time[1]='9';
				return 0;
			}


			if(time[0]=='0' && time[1]=='6')
			{
				time[0]='1';
				time[1]='0';
				return 0;
			}

			if(time[0]=='0' && time[1]=='7')
			{
				time[0]='1';
				time[1]='1';
				return 0;
			}

			if(time[0]=='0' && time[1]=='8')
			{
				time[0]='1';
				time[1]='2';
				return 0;
			}

			if(time[0]=='0' && time[1]=='9')
			{
				time[0]='1';
				time[1]='3';
				return 0;
			}

			if(time[0]=='1' && time[1]=='0')
			{
				time[0]='1';
				time[1]='4';
				return 0;
			}
			if(time[0]=='1' && time[1]=='1')
			{
				time[0]='1';
				time[1]='5';
				return 0;
			}
			if(time[0]=='1' && time[1]=='2')
			{
				time[0]='1';
				time[1]='6';
				return 0;
			}

			if(time[0]=='1' && time[1]=='3')
			{
				time[0]='1';
				time[1]='7';
				return 0;
			}
			if(time[0]=='1' && time[1]=='4')
			{
				time[0]='1';
				time[1]='8';
				return 0;
			}
			if(time[0]=='1' && time[1]=='5')
			{
				time[0]='1';
				time[1]='9';
				return 0;
			}
			if(time[0]=='1' && time[1]=='6')
			{
				time[0]='2';
				time[1]='0';
				return 0;
			}
			if(time[0]=='1' && time[1]=='7')
			{
				time[0]='2';
				time[1]='1';
				return 0;
			}

			if(time[0]=='1' && time[1]=='8')
			{
				time[0]='2';
				time[1]='2';
				return 0;
			}
			if(time[0]=='1' && time[1]=='9')
			{
				time[0]='2';
				time[1]='3';
				return 0;
			}

			if(time[0]=='2' && time[1]=='0')
			{
				time[0]='0';
				time[1]='0';
				return 0;
			}

			if(time[0]=='2' && time[1]=='1')
			{
				time[0]='0';
				time[1]='1';
				return 0;
			}
			if(time[0]=='2' && time[1]=='2')
			{
				time[0]='0';
				time[1]='2';
				return 0;
			}
			if(time[0]=='2' && time[1]=='3')
			{
				time[0]='0';
				time[1]='3';
			}

	return 0;

}

///////////////////////////////////////////////////////////////////////////////////////////////



int main()
{
	
	init();


	usart0_init();
	usart1_init();

	sei();

	while(1)
	{

		for(uint8_t i=0; i<sizeof(msg0); i++)
		{
			usart0_write(msg0[i]);
			_delay_ms(USART0_DELAY_MS);
		}
		
		_delay_ms(500);

		usart0_clear_tx_buffer();

		uint8_t Len=usart0_rx_len();

		
		if(Len)
		{
			uint8_t Data_Rx[Len];

			for(uint8_t i=0; i<Len; i++)
				Data_Rx[i]=usart0_read();

			usart0_clear_rx_buffer();
				
			uint16_t modbus_crc=0xFFFF;

			for(uint8_t i=0; i<Len; i++)
				modbus_crc=_crc16_update(modbus_crc, Data_Rx[i]);
		
			for(uint8_t i=3; i<9; i++)
				if(Data_Rx[i]==0xFF) modbus_crc=1;

				
			if(!(modbus_crc))
			{
				
				PORT_LED|=LED_RUN;

				usart1_clear_tx_buffer();

				Time[0]=Data_Rx[3];
				Time[1]=Data_Rx[4];
				Time[3]=Data_Rx[5];
				Time[4]=Data_Rx[6];
				Time[6]=Data_Rx[7];
				Time[7]=Data_Rx[8];

				Time_Zone_Update(Time);

				for(uint8_t i=0; i<sizeof(Time); i++)
				{
					usart1_write(Time[i]);

					_delay_ms(USART1_DELAY_MS);
				}

			}
			else
			{

				PORT_LED&=~LED_RUN;

			}
		}			

	}



}

////////////////////////////////////////////////////////////////////////////////////////////


////INTERRUPT////

ISR(TIMER1_COMPA_vect)
{

	TCNT1=0x0000;

	asm volatile ("wdr");



}
