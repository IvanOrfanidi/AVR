


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

	#include "BOOT.h"
	#include "PROTOCOL.h"

	#define LED_ERR (1<<PC0)	//Pin OUTPUT Led RED.
	#define LED_RUN (1<<PC1)	//Pin OUTPUT Led GREEN.
	#define LED1 (1<<PB0)		//Pin OUTPUT Led YELLOW.
	#define LED2 (1<<PB1)		//Pin OUTPUT Led YELLOW.
	#define LED3 (1<<PB2)		//Pin OUTPUT Led YELLOW.

	#define KEY1 (1<<PC5)			//Pin INPUT Key1.
	#define KEY2 (1<<PC4)			//Pin INPUT Key2.
	#define KEY3 (1<<PC3)			//Pin INPUT Key3.
	#define KEY4 (1<<PC2)			//Pin INPUT Key4.

	
	#define BAUDRATE1 (1<<PD7)		//Pin INPUT Baud Rate USART.
	#define BAUDRATE2 (1<<PD6)		//Pin INPUT Baud Rate USART.
	#define BAUDRATE3 (1<<PD5)		//Pin INPUT Baud Rate USART.
	#define BAUDRATE4 (1<<PD4)		//Pin INPUT Baud Rate USART.

	#define RE	(1<<PD2)	//Pin OUTPUT Receiver Output Enable.
							//A low level enables the receiver output, RO.
							//A high level places it in a high impedance state.

	#define DE	(1<<PD3)	//Pin OUTPUT Driver Output Enable.
							//A high level enables the driver differential outputs,
							//A and B. A low level places it in a high impedance state.


	#define PIN_BAUDRATE PIND
	#define	PIN_KEY PINC
	#define PORT_LED PORTB
	#define PORT_LED_INF PORTC
	#define PORT_IO PORTD
	#define PIN_SELECT_SLAVE PINB


void init()
{

	// Port B initialization //

		DDRB=(LED1)|(LED2)|(LED3);
		PORTB=0x00;

	// Port C initialization //

		DDRC=(LED_ERR)|(LED_RUN);
		PORTC=(KEY1)|(KEY2)|(KEY3)|(KEY4);

	// Port D initialization //

		DDRD=(RE)|(DE);
		PORTD=(BAUDRATE1)|(BAUDRATE2)|(BAUDRATE3)|(BAUDRATE4);

	/////////////////////////////////////////////////////////////////////


	//Analog Comparator initialization.

		ACSR|=(1<<ACD);		//Disable Comparator.

	/////////////////////////////////////////////////////////////////////


	//Watchdog initialization.

	//!!!WDTON fuse set to “0“ means programmed and “1“ means unprogrammed!!!//

	cli();
	wdt_disable();

	////////////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////////////

}

static void (*jump_to_app)(void) = 0x0000;


int main()
{
		
	protocol_init_usart(0x01);

	init();
	

	PORT_LED_INF|=LED_RUN;
	
	while(1)
	{
		wdt_reset();
		

		
	}

}



