


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

	#include "lcd.h"
	#include "PCF8583.h"
	#include "TWI.h"
	#include "USART.h"

	#define BUT1 (1<<PB0)
	#define BUT2 (1<<PB1)
	#define BUT3 (1<<PB2)
	#define BUT4 (1<<PB3)

	#define BUT_SEL (1<<PD3)

	#define PIN_BUT PINB

	#define PIN_BUT_SEL PIND
	#define PORT_OUT PORTD

	
	#define LCD_LIGHT (1<<PD7)
	#define BUZ (1<<PD5)
	#define RTC_INT (1<<PD6)

	#define SDA (1<<PC1)
	#define SCL (1<<PC0)


	#define START_TIMER1	TCCR1B=(1<<CS12)|(0<<CS11)|(0<<CS10);
	#define STOP_TIMER1		TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);

	char GPRMC[]={'G','P','R','M','C'};

void init(void)
{

	// Port A initialization //

	DDRA=0xF7;

	PORTA= 0x00;

	///////////////////////////
	
	// Port B initialization //

	DDRB=0x00;

	PORTB= BUT1 | BUT2 | BUT3 | BUT4;

	///////////////////////////

	// Port C initialization //

	DDRC=0x00;

	PORTC= SDA | SCL;

	///////////////////////////

	// Port C initialization //

	DDRD=LCD_LIGHT | BUZ;

	PORTD= RTC_INT | LCD_LIGHT | BUT_SEL;

	///////////////////////////


	//Analog Comparator initialization.

	ACSR|=_BV(ACD);		//Disable Comparator.

	////////////////////////////////////////////////////////////////////


	//Timer1 initialization.

	START_TIMER1;

	OCR1A=31250;

	TIMSK=(1<<OCIE1A);	//Interrupt Timer1 COMPA.
	////////////////////////////////////////////////////////////////////


	//WATCHDOG initialization.

//	WDTCR=(1<<WDTOE)|(1<<WDE);									//Enable WATCHDOG.

//	WDTCR=(1<<WDTOE)|(1<<WDE)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0);	//WATCHDOG clock 2100ms.


	////////////////////////////////////////////////////////////////////

}

main()
{
	init();
	
	usart0_init();

	TWIOpen(0x40);

		//LCD initialisation 
	LCD_Ini(); 					     
	// delay_ms(2);  		
	SEND_CMD(DISP_ON);
	// delay_ms(10);  						
	SEND_CMD(CLR_DISP); 

	sei();
	

	while(1)
	{
		
		
	}
}



ISR(TIMER1_COMPA_vect)
{
	sei();

	TCNT1=0x0000;

	uint8_t temp_byte;

	while(usart0_rx_len())
	{
		temp_byte=usart0_read();
		if(temp_byte=='$')
		{
			uint8_t gprmc_true=1;
			for(uint8_t i=0; i<sizeof(GPRMC); i++)
				if(GPRMC!=usart0_read()) gprmc_true=0;
			if(gprmc_true)
			{
				for(uint8_t i=13; i>0; i--)
					temp_byte=usart0_read();
			}
				
		}
	}

	SEND_CMD(DD_RAM_ADDR);
	SEND_CHAR(temp_byte);
	

}
