


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
	#include "MODBUS_SLAVE.h"



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

	uint8_t index;

	char temp_buf[20];
	char led_on[]="AT+LED=1";
	char led_off[]="AT+LED=0";
	char led_stat[]="AT+LED?";

	char ok[]={0x0D, 0x0A,'O', 'K', 0x0D, 0x0A};
	char error[]={0x0D, 0x0A, 'E', 'R', 'R', 'O', 'R', 0x0D, 0x0A};


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

	OCR1A=28800;


	TIMSK=(1<<OCIE1A);	//Interrupt Timer1 COMPA.

}



int main()
{
	init();
	
	usart0_init();

	slave_id=0x01;

	sei();


	while(1)
	{

		if(usart0_rx_len())
		{
			usart0_clear_tx_buffer();

			uint8_t temp=usart0_read();
			
			if(temp==0x08)
			{
				if(index) index--;
			}
			else
			{
				temp_buf[index]=temp;
				index++;
			}
			
			if(temp==0x0D)
			{
				uint8_t led_on_ok=1;
				uint8_t led_off_ok=1;

				for(uint8_t i=0; i<(index-1); i++)
				{
					if(temp_buf[i]!=led_on[i]) led_on_ok=0;
					if(temp_buf[i]!=led_off[i]) led_off_ok=0;
				}

				if(led_on_ok)
				{
					PORT_LED_GREEN|=(LED_GREEN);

					for(uint8_t i=0; i<sizeof(ok); i++) usart0_write(ok[i]);
				}

				if(led_off_ok)
				{
					PORT_LED_GREEN&=~(LED_GREEN);

					for(uint8_t i=0; i<sizeof(ok); i++) usart0_write(ok[i]);
				}

				if(!(led_on_ok) && !(led_off_ok))
				{
					for(uint8_t i=0; i<sizeof(error); i++) usart0_write(error[i]);
				}

				index=0;
			}

			
			usart0_write(temp);

			usart0_clear_rx_buffer();

		}

	}


}



ISR(TIMER1_COMPA_vect)				//Interrupt 1c.
{
	TCNT1=0;
	if(PORT_LED_YELLOW & (LED_YELLOW)) PORT_LED_YELLOW&=~(LED_YELLOW); else  PORT_LED_YELLOW|=(LED_YELLOW);

	usart0_clear_tx_buffer();

	char msg[]="Hello word!";

	for(uint8_t i=0; i<sizeof(msg); i++) usart0_write(msg[i]);

}
