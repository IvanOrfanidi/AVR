

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

// User functions.
	#include "USART.h"
	#include "GPS.h"
	#include "MODBUS_SLAVE.h"

////////////////////////////////////////////////

	#define PORT_LED PORTD
	#define LED_RUN (1<<PD6)
	#define LED_PWR (1<<PD5)


	#define BAUDRATE0			PC1
	#define BAUDRATE1			PC0
	#define PIN_BAUDRATE 		PINC

	#define PIN_SEL_SLAVE_ADDR 	PINA


	uint8_t back_len = 0;
	uint8_t back_len1 = 0;

	#define TIMEOUT_GPS		5000000

	uint8_t slave_add;
	uint32_t timeout_gps_conect=TIMEOUT_GPS;

	char time[8];


//////////////////////////////////////////////

void init(void)
{

	DDRA=0x00;
	PORTA=0xFF;

	DDRB= (1<<PB1);
	PORTB=0xFF;

	DDRC=0x00;
	PORTC=0xFF;

	DDRD= LED_PWR | LED_RUN;
	PORTD= LED_PWR;

	DDRE=0x00;
	PORTE=0xFF;

	//////////////////////////////////////////////

	
	//Analog Comparator initialization.

	ACSR|=(1<<ACD);		//Disable Comparator.

	//////////////////////////////////////////////

	///Timer0 initialization.

	OCR1A=0x02FF; //100


	TIMSK|=(1<<OCIE1A);	

	//////////////////////////////////////////////

	///Timer3 initialization.

	OCR3A=300; //300


	ETIMSK|=(1<<OCIE3A);

	//////////////////////////////////////////////

	//WATCHDOG initialization.

	wdt_reset();								
//	wdt_disable();
	wdt_enable(WDTO_2S);

	////////////////////////////////////////////////////////////////////

}

//////////////////////////////////////////////////

uint8_t select_slave_addr(void)
{
	return ~PIN_SEL_SLAVE_ADDR;
}

//////////////////////////////////////////////////

uint8_t select_baud_rate(void)
{

	if((PIN_BAUDRATE & (1<<BAUDRATE0)) && (PIN_BAUDRATE & (1<<BAUDRATE1))) return 1;			//Baudrate 2400	

	if((!(PIN_BAUDRATE & (1<<BAUDRATE0))) && (PIN_BAUDRATE & (1<<BAUDRATE1))) return 2;		//Baudrate 4800

	if((PIN_BAUDRATE & (1<<BAUDRATE0)) && (!(PIN_BAUDRATE & (1<<BAUDRATE1)))) return 3;		//Baudrate 9600

	if((!(PIN_BAUDRATE & (1<<BAUDRATE0))) && (!(PIN_BAUDRATE & (1<<BAUDRATE1)))) return 5;	//Baudrate 19200

	return 0;
}

//////////////////////////////////////////////////

void init_time_0xffff(void)
{
	for(uint8_t i=1; i<7; i++)
		registers[i]=0xFFFF;
}

//////////////////////////////////////////////////

int main(void)
{

	init();

	usart0_init();							//Init USART 19200 8-N-1.
	usart1_mod_init(select_baud_rate());

	init_time_0xffff();			//All time[] = 0xFFFF.

	slave_add=select_slave_addr();

	PORTB&=~(1<<PB1);			//PORT_RS485|=(1<<RE_DE);

	sei();						//Enable Interrupt.

	while(1)
	{

		//”словие отвечающие за обработку MODBUS протокола.
		if(usart1_rx_len()!=back_len1)	
		{
			back_len1=usart1_rx_len();
			TCNT3=0x0000; 
			TCCR3B=(1<<CS32)|(0<<CS31)|(0<<CS30);
		}

		//”словие отвечающие за обработку GPS протокола.
		if(usart0_rx_len()!=back_len)
		{
			back_len=usart0_rx_len();
			TCNT1=0x0000;
			TCCR1B=(1<<CS12)|(0<<CS11)|(0<<CS10);
		}

		//”словие отвечающее за time[] = 0xFFFF.
		if(timeout_gps_conect)
			timeout_gps_conect--;
			else
				init_time_0xffff();


		wdt_reset();	//Watchdog Reset.

	
	}// End while(1).

}// End main().

/// INTERRUPT ///

// ѕрерывание отвечающие за обработку MODBUS протокола.
ISR(TIMER3_COMPA_vect)
{

	TCCR3B=(0<<CS32)|(0<<CS31)|(0<<CS30);
	TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);
	TCNT3=0x0000;

	back_len1=0;

	sei();

	if(!(modbus_slave(slave_add)))
	{
		PORT_LED|=LED_RUN;
	}
	else
	{
		PORT_LED&=~LED_RUN;
	}


	TCCR1B=(1<<CS12)|(0<<CS11)|(0<<CS10);

}

// ѕрерывание отвечающие за обработку GPS протокола.
ISR(TIMER1_COMPA_vect)
{

	TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);
	TCCR3B=(0<<CS32)|(0<<CS31)|(0<<CS30);
	TCNT1=0x0000;

	uint8_t len_buf=usart0_rx_len();

	char data[len_buf];

	for(uint8_t i=0; i<sizeof(data); i++)
		data[i]=usart0_read();

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();
	back_len=0;
		
	if(!(gps_status(data, len_buf)))
	{
		gps_time(time, data, len_buf);

		registers[1]=time[0];
		registers[2]=time[1];
		registers[3]=time[3];
		registers[4]=time[4];
		registers[5]=time[6];
		registers[6]=time[7];

		timeout_gps_conect = TIMEOUT_GPS;

	}

	TCCR3B=(1<<CS32)|(0<<CS31)|(0<<CS30);
}

