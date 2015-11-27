



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

	uint8_t back_len;

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


	///Timer0 initialization.

	TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);	//Timer2 clock off.

	OCR1A=0x02FF; //100


	TIMSK|=(1<<OCIE1A);	


//////////////////////////////////////////////



//////////////////////////////////////////////

}

////////////////////////////////////////////////////////////////////////////////////////////

int main(){
	
	init();

	usart0_init();

	usart1_init();

	sei();

	while(1)
	{
		
		if(usart1_rx_len()!=back_len)
		{
			back_len=usart1_rx_len();
			TCNT1=0x0000;
			TCCR1B=(1<<CS12)|(0<<CS11)|(0<<CS10);
		}


	}
}

ISR(TIMER1_COMPA_vect)
{
	
	TCNT1=0x0000;
	TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);

	uint8_t len_buf=usart1_rx_len();

	char data[len_buf];

	for(uint8_t i=0; i<sizeof(data); i++)
		data[i]=usart1_read();

	usart1_clear_tx_buffer();
	usart1_clear_rx_buffer();
	back_len=0;
	
	sei();
	
	if(!(gps_status(data, len_buf)))
	{
		char time[8];

	
		if(!(gps_time(time, data, len_buf)))
		{
			for(uint8_t i=0; i<sizeof(time); i++)
				usart0_write(time[i]);

			usart0_write(0x0D);
			usart0_write(0x0A);
		}
	



		uint8_t usart0_busy_wait(void);
		_delay_ms(100);

		usart0_clear_tx_buffer();
		usart0_clear_rx_buffer();

	}


}



