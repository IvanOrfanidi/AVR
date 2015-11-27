

//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <avr/wdt.h>
	#include <avr/eeprom.h>
	#include <avr/sleep.h>

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>

	#include "lcd.h"
	#include "USART.h"

	#define EXT_PWR_IN 	(1<<PC0)

	#define PIN_PWR_CONTROL PINC
	#define EXT_PWR_TRUE (PIN_PWR_CONTROL & (EXT_PWR_IN))

	#define PORT_BL PORTB
	#define BL (1<<PB0)

	#define TIMEOUT_GPS		60		//Timeout sec.

	#define true      1
	#define false     0
	#define bool      _Bool


	uint8_t Time_STR[8] = {'0', '0', ':', '0', '0', ':', '0', '0'};

	uint8_t sec = 0;
	uint8_t min = 0;
	uint8_t hour = 0;
	
	bool Sleep_CPU = false;
	bool fTime_Update = true;


	uint16_t Timeout_GPS = 0;

	char time_gps_valid[]="       TIME GPS ";
	char time_gps_invalid[]="       TIME RTC ";

	uint8_t temp_data[RX_BUFFER_SIZE0-1];
	uint8_t temp_time[6];


void Init(void)
{

	// Port B initialization //

	DDRB= BL;

 	PORTB= BL;

	//////////////////////////

	// Port C initialization //

	DDRC= 0x00;

	PORTC= 0x00;

	//////////////////////////

	// Port D initialization //

	DDRD= 0b11111110;

	PORTD= 0x00;

	//////////////////////////

	//Analog Comparator initialization.

	ACSR|=(1<<ACD);		//Disable Comparator.

	/////////////////////////////////////////////////////////////////////

	//Timer1 initialization //



	////////////////////////////////////////////////////////////////////

	// Watchdog initialization. //

	wdt_reset();

	wdt_enable(WDTO_2S);

	////////////////////////////////////////////////////////////////////
	
}


///////////////////////////////////////////////////////////////////////////////////////


int main(void)
{

	Init();

	usart0_init();

	
	//LCD initialisation 
	LCD_Ini(); 					     
	SEND_CMD(DISP_ON);
	SEND_CMD(CLR_DISP); 

	wdt_reset();

	sei();

	while(1)
	{

				wdt_reset();

				uint8_t len = usart0_rx_len();
					
				if(len)
				{
					


					for(uint8_t i=0; i<len; i++)
						SEND_CHAR(usart0_read());
					
					usart0_clear_rx_buffer();

				
				
				}
				
		}			
				
}


