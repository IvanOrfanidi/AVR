
//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <avr/eeprom.h>
	#include <util/delay.h>

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>



	#define true      1
	#define false     0
	#define bool      _Bool


	#define BUZ		(1<<PD7)
	#define PORT_BUZ	PORTD

	#define LED1	(1<<PA0)
	#define LED2	(1<<PA1)
	#define LED3	(1<<PA2)
	#define LED4	(1<<PA3)

	#define BUT1	(1<<PA4)
	#define BUT2	(1<<PA5)
	#define BUT3	(1<<PA6)
	#define BUT4	(1<<PA7)

	#define PORT_OUT	PORTA
	#define PIN_IN		PINA


	#define LED_ERR	(1<<PB0)
	#define LED_RUN (1<<PB1)
	#define CS_SPI_ENC28J60		(1<<PB2)
	#define PORT_LED		PORTB
	
	#define PC_CONNECT		(PIND & (1<<PD6))

	#define CTS				(1<<PD5)		//Port OUTPUT.
	#define DTR				(1<<PD4)		//Port INPUT.
	#define RTS				(1<<PD3)		//Port INPUT.
	#define PIN_RS232		PIND
	#define PORT_RS232		PORTD


void init(void)
{

   DDRA = LED1 | LED2 | LED3 | LED4;         // all output high
   PORTA = BUT1 | BUT2 | BUT3 | BUT4;

   DDRB = LED_ERR | LED_RUN | CS_SPI_ENC28J60;
   PORTB = 0xFF;
   //PORTB&= ~(LED_ERR);

   DDRC = 0x00;
   PORTC = 0xFF;

   DDRD = CTS | BUZ;
   PORTD = 0xFF;
   PORTD&= ~(BUZ);

   DDRD &= ~(0x04);     // INT 0 = PD2 = input with pull up
   PORTD |= 0x04;
   MCUCR &= 0xFE;       // INT 0 sense on falling edge
   MCUCR |= 0x02;
   GICR |= 0x40;        // enable interrupt on INT 0


}

int main (void)
{
	init();


		

	while(1)
	{
		



	}


}
