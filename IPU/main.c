


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

	

	#define LED_ERR (1<<PC0)	//Pin OUTPUT Led RED.
	#define LED_RUN (1<<PC1)	//Pin OUTPUT Led GREEN.
	#define LED1 (1<<PB0)		//Pin OUTPUT Led YELLOW.
	#define LED2 (1<<PB1)		//Pin OUTPUT Led YELLOW.
	#define LED3 (1<<PB2)		//Pin OUTPUT Led YELLOW.

	#define KEY1 (1<<PC5)			//Pin INPUT Key1.
	#define KEY2 (1<<PC4)			//Pin INPUT Key2.
	#define KEY3 (1<<PC3)			//Pin INPUT Key3.
	#define KEY4 (1<<PC2)			//Pin INPUT Key4.

	#define SELECT_SLAVE (1<<PB3)	//Pin select Slave address.

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

	#define TIME_SYMBOL 800

	#define DEVICE_ID 0x0101	//

	uint8_t usart_byte;
	uint16_t count_start_timer;

	uint16_t time_symbol;

void init()
{

	// Port B initialization //

		DDRB=(LED1)|(LED2)|(LED3);
		PORTB=(SELECT_SLAVE);

	// Port C initialization //

		DDRC=(LED_ERR)|(LED_RUN);
		PORTC=(KEY1)|(KEY2)|(KEY3)|(KEY4);

	// Port D initialization //

		DDRD=(RE)|(DE);
		PORTD=(BAUDRATE1)|(BAUDRATE2)|(BAUDRATE3)|(BAUDRATE4);

	/////////////////////////////////////////////////////////////////////


	//Analog Comparator initialization.

		ACSR|=_BV(ACD);		//Disable Comparator.

	/////////////////////////////////////////////////////////////////////

	//Timer2 initialization.

		TCCR2B=(0<<CS22)|(0<<CS21)|(0<<CS20);	//Timer2 clock off.

		OCR2A=10; //4000


		TIMSK2=(1<<OCIE2A);	

	/////////////////////////////////////////////////////////////////////

	//Watchdog initialization.

	//!!!WDTON fuse set to “0“ means programmed and “1“ means unprogrammed!!!//
	
	/////////////////////////////////////////////////////////////////////
}

int main()
{


	init();
	usart0_init();

	count_start_timer=0;

	usart0_init();

	slave_id=1;
	
	sei();						//Enable Interrupt.

	PORT_IO&=~(DE);
	PORT_IO&=~(RE);
	
	registers[1]=DEVICE_ID;

	while(1)
	{

		if((usart0_rx_len()) && !(TCCR2B))
		{
		
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
				TCCR2B=(1<<CS22)|(0<<CS21)|(0<<CS20);
				PORT_LED_INF|=(LED_RUN);

			}
		}
		
	}


}


void RXD()
{

	PORT_IO|=(DE);
	PORT_IO|=(RE); 

}


// INTERRUPT //

//Interrupt Timer1//
//Interrupt Timer2//
ISR(TIMER2_COMPA_vect)				//Interrupt Timer2.
{

	sei();							//Enable Interrupt.

	TCCR2B=0x00; 					//Stop Timer2.
	TCNT2=0x00;						//Reset Timer2.


	rx_len=usart0_rx_len();

	for(uint8_t i=0; i<rx_len; i++)
	{
		data[i]=usart0_read();
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();


	PORT_LED_INF&=~(LED_RUN);
	PORT_LED_INF&=~(LED_ERR);	

	uint8_t ack=modbus();

	if(ack==2) PORT_LED_INF|=(LED_ERR);

	while(usart0_busy_wait());
	_delay_ms(5);

	PORT_IO&=~(DE);
	PORT_IO&=~(RE);


}

//////////////////////////////////////////////////////////////////////////////

