


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


	#define SLAVE_ID_4 PB0
	#define SLAVE_ID_8 PB1
	#define SLAVE_ID_16 PB2
	#define SLAVE_ID_32 PB3
	#define SLAVE_ID_64 PB4
	#define SLAVE_ID_128 PB5
	#define XTAL1 PB6
	#define XTAL2 PB7

	#define SENSOR0 PC0
	#define SENSOR1 PC1
	#define SENSOR2 PC2
	#define LED_ERR (1<<PC4)
	
	#define LED_RUN (1<<PD3)
	#define BAUDRATE0 PD4
	#define BAUDRATE1 PD5
	#define SLAVE_ID_1 PD6
	#define SLAVE_ID_2 PD7

	#define PORT_LED_ERR PORTC

	#define PORT_LED_RUN PORTD



	#define PIN_BAUDRATE PIND

	#define PIN_SELECT_SLAVE0 PIND

	#define PIN_SELECT_SLAVE1 PINB

	#define PIN_SENSOR PINC

	#define TIME_SYMBOL 10000

	#define DEVICE_ID 0x0201	//

	uint8_t slave_add;
	uint8_t back_len;

void init()
{

	// Port B initialization //

	DDRB=(0<<SLAVE_ID_128)|(0<<SLAVE_ID_64)|(0<<SLAVE_ID_32)|(0<<SLAVE_ID_16)|(0<<SLAVE_ID_8)|(0<<SLAVE_ID_4)|(0<<XTAL1)|(0<<XTAL2);

	PORTB=(1<<SLAVE_ID_128)|(1<<SLAVE_ID_64)|(1<<SLAVE_ID_32)|(1<<SLAVE_ID_16)|(1<<SLAVE_ID_8)|(1<<SLAVE_ID_4)|(0<<XTAL1)|(0<<XTAL2);

	///////////////////////////

	// Port C initialization //
	#if(DEVICE_ID==0x0201)

		DDRC=(0<<SENSOR0)|(0<<SENSOR1)|(0<<SENSOR2)|(LED_ERR);
	
		PORTC=(0<<SENSOR0)|(1<<SENSOR1)|(1<<SENSOR2)|(LED_ERR);

	#endif
	///////////////////////////

	// Port D initialization //

	DDRD=(1<<PD2)|(LED_RUN)|(0<<BAUDRATE0)|(0<<BAUDRATE1)|(0<<SLAVE_ID_1)|(0<<SLAVE_ID_2);

	PORTD=(1<<PD2)|(!(LED_RUN))|(1<<BAUDRATE0)|(1<<BAUDRATE1)|(1<<SLAVE_ID_1)|(1<<SLAVE_ID_2);

	///////////////////////////


	//Analog Comparator initialization.

	ACSR|=(1<<ACD);		//Disable Comparator.

	/////////////////////////////////////////////////////////////////////

	//ADC initialization.

	sei();					//Enable Interrupt for ADC seting.

	#if(DEVICE_ID==0x0201)

		DIDR0=(1<<ADC0D);

	#endif

	ADCSRA=(1<<ADEN);		//Enable ADC.
	ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADATE)|(1<<ADIE);		//fclkADC=fclkIO/128.

	ADCSRB=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

	ADMUX=(0<<REFS0)|(1<<REFS1);		//Uref=AVcc.

	cli();					//Disable Interrupt.

	/////////////////////////////////////////////////////////////////////


	///Timer0 initialization.

	TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);	//Timer2 clock off.

	OCR1A=300; //100


	TIMSK1|=(1<<OCIE1A);	

	/////////////////////////////////////////////////////////////////////

	//Watchdog initialization.

	asm volatile ("wdr");	//Watchdog Reset.


	WDTCSR|= (1<<WDCE)|(1<<WDE);								//Watchdog Enabled.
	WDTCSR = (1<<WDE)|(1<<WDIE)|(1<<WDP3)|(0<<WDP2)|(0<<WDP1)|(0<<WDP0);	//Time-out 4s.

	//!!!WDTON fuse set to “0“ means programmed and “1“ means unprogrammed!!!//
	
	/////////////////////////////////////////////////////////////////////


}

int select_baud_rate(void)
{
	uint8_t baud_rate;

	if((PIN_BAUDRATE & (1<<BAUDRATE0)) && (PIN_BAUDRATE & (1<<BAUDRATE1))) baud_rate=1;			//Baudrate 2400	

	if((!(PIN_BAUDRATE & (1<<BAUDRATE0))) && (PIN_BAUDRATE & (1<<BAUDRATE1))) baud_rate=2;		//Baudrate 4800

	if((PIN_BAUDRATE & (1<<BAUDRATE0)) && (!(PIN_BAUDRATE & (1<<BAUDRATE1)))) baud_rate=3;		//Baudrate 9600

	if((!(PIN_BAUDRATE & (1<<BAUDRATE0))) && (!(PIN_BAUDRATE & (1<<BAUDRATE1)))) baud_rate=5;	//Baudrate 19200


	return baud_rate;
}


int select_slave_id(void)
{
	return ~((PIN_SELECT_SLAVE0>>6) | (PIN_SELECT_SLAVE1<<2));
}


int main()
{

	init();
	
	usart0_mod_init(select_baud_rate());

	slave_add=select_slave_id();

	sei();						//Enable Interrupt.

	PORTD&=~(1<<PD2);

	ADCSRA|=(1<<ADSC);

	registers[1]=DEVICE_ID;


	while(1)
	{

		if(usart0_rx_len()!=back_len)
		{
			PORT_LED_RUN|=LED_RUN;
			PORT_LED_ERR|=LED_ERR;
			back_len=usart0_rx_len();
			TCNT1=0x0000;
			TCCR1B=(1<<CS12)|(0<<CS11)|(0<<CS10);
		}

		


		registers[3]=(1 && (PIN_SENSOR & (1<<SENSOR1)));
		registers[4]=(1 && (PIN_SENSOR & (1<<SENSOR2)));


		asm volatile ("wdr");	//Watchdog Reset.

	}


}



// INTERRUPT //
ISR(TIMER1_COMPA_vect)
{
	
	TCNT1=0x0000;
	TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);

	sei();
	back_len=0;

	uint8_t ack=modbus_slave(slave_add);
	if(ack==2) PORT_LED_ERR&=~LED_ERR;
	PORT_LED_RUN&=~LED_RUN;
}

//Interrupt ADC//
ISR(ADC_vect)
{
	registers[2]=ADC;
}

/////////////////////////////////////////////////////////////////////

//Interrupt Watchdog Timer//
ISR(WDT_vect)
{
}

