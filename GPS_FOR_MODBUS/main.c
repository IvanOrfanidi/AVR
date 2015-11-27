

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
	#include "MODBUS_SLAVE.h"

	#define PORT_LED PORTD
	#define LED_RUN (1<<PD6)

	uint8_t back_len;
	uint8_t back_len1;

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

	///Timer0 initialization.

	OCR1A=0x02FF; //100


	TIMSK|=(1<<OCIE1A);	


	///Timer3 initialization.

	OCR3A=300; //300


	ETIMSK|=(1<<OCIE3A);	


	//////////////////////////////////////////////

	//WATCHDOG initialization.

	WDTCR=(1<<WDCE)|(1<<WDE);									//Enable WATCHDOG.

	WDTCR=(1<<WDCE)|(1<<WDE)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0);	//WATCHDOG clock 2100ms.


	////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////

}

////////////////////////////////////////////////////////////////////////////////////////////

int main(){
	
	init();

	usart0_init();

	usart1_init();

	for(uint8_t i=1; i<6; i++)
		registers[i]=0;

	sei();

	while(1)
	{


	
		if(usart1_rx_len()!=back_len)
		{
			back_len=usart1_rx_len();
			TCNT1=0x0000;
			TCCR1B=(1<<CS12)|(0<<CS11)|(0<<CS10);
		}

		if(usart0_rx_len()!=back_len1)
		{
			back_len1=usart0_rx_len();
			TCNT3=0x0000;
			TCCR3B=(1<<CS32)|(0<<CS31)|(0<<CS30);
		}
	
	}
}

ISR(TIMER1_COMPA_vect)
{
	
	TCNT1=0x0000;
	TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);
	TCCR3B=(0<<CS32)|(0<<CS31)|(0<<CS30);

	uint8_t len_buf=usart1_rx_len();

	char data[len_buf];

	for(uint8_t i=0; i<sizeof(data); i++)
		data[i]=usart1_read();

	usart1_clear_tx_buffer();
	usart1_clear_rx_buffer();
	back_len=0;
		
	if(!(gps_status(data, len_buf)))
	{
		char time[8];
		gps_time(time, data, len_buf);

		registers[1]=time[0];
		registers[2]=time[1];
		registers[3]=time[3];
		registers[4]=time[4];
		registers[5]=time[6];
		registers[6]=time[7];

	}

	TCCR3B=(1<<CS32)|(0<<CS31)|(0<<CS30);
}

ISR(TIMER3_COMPA_vect)
{

	TCNT3=0x0000;
	TCCR3B=(0<<CS32)|(0<<CS31)|(0<<CS30);
	TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);

	back_len1=0;

	sei();
	if(!(modbus_slave(1)))
	{
		PORT_LED|=LED_RUN;
		asm volatile ("wdr");
	}
	else
	{
		PORT_LED&=~LED_RUN;
	}



	TCCR1B=(1<<CS12)|(0<<CS11)|(0<<CS10);
}

