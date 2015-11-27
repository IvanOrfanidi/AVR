


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


	#define BAUDRATE1 (1<<PD7)		//Pin INPUT Baud Rate USART.
	#define BAUDRATE2 (1<<PD6)		//Pin INPUT Baud Rate USART.
	#define BAUDRATE3 (1<<PD5)		//Pin INPUT Baud Rate USART.
	#define BAUDRATE4 (1<<PD4)		//Pin INPUT Baud Rate USART.

	uint8_t baud_rate;


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
	uint8_t PIN_BAUDRATE=0b10100000;

	uint8_t baud_rate=0;

	baud_rate|=((PIN_BAUDRATE & BAUDRATE1)>>7);

	baud_rate|=((PIN_BAUDRATE & BAUDRATE2)>>5);

	baud_rate|=((PIN_BAUDRATE & BAUDRATE3)>>3);

	baud_rate|=((PIN_BAUDRATE & BAUDRATE4)>>1);

	baud_rate=((~baud_rate) & 0b00001111);

	init();
	
	usart0_init();

	slave_id=0x01;

	sei();


	while(1)
	{
		if(usart0_rx_len()) TCCR1B=(1<<CS12)|(0<<CS11)|(0<<CS10);


		if(registers[1]==0) PORT_LED_GREEN&=~(LED_GREEN); else PORT_LED_GREEN|=(LED_GREEN);
		if(registers[2]==0) PORT_LED_YELLOW&=~(LED_YELLOW); else PORT_LED_YELLOW|=(LED_YELLOW);

		if(!(PIN_KEY & KEY0)) registers[3]=1; else registers[3]=0;
		if(!(PIN_KEY & KEY1)) registers[4]=1; else registers[4]=0;
	}



}



ISR(TIMER1_COMPA_vect)				//Interrupt 300mc.
{
	TCCR1B=0x00;
	TCNT1=0x00;

	rx_len=usart0_rx_len();

	for(uint8_t i=0; i<rx_len; i++)
	{
		data[i]=usart0_read();
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();


	if(modbus()==1) PORT_LED_YELLOW|=(LED_YELLOW);	


}
