

	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/pgmspace.h>
	#include <util/delay.h>
	#include <util/crc16.h>

	// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>


	#define DELAY_PRESENCE 5000 	//5000 for 8MHz
	#define RESET_PULSE 200			//200 for 8MHz
	#define PAUSE 255				//255 for 8MHz
	#define RESET 			for(uint16_t n=0; n<RESET_PULSE; n++) asm volatile ("nop");

	#define ONE_PULSE 30	//14 for <=8MHz, 20 for 11.0592MHz, 30 for 14.7456MHz,
	#define ONE_COMMAND 	for(uint8_t n=0; n<ONE_PULSE; n++) asm volatile ("nop");

	#define ZERO_PULSE 60	//36 for <=8MHz, 40 for 11.0592MHz, 60 for 14.7456MHz,
	#define ZERO_COMMAND	for(uint8_t n=0; n<ZERO_PULSE; n++) asm volatile ("nop");

	#define DELAY_ONE	30	//10 or 11 for <=8MHz, 14 for 11.0592MHz, 30 for 14.7456MHz,
	#define DELAY_ZERO	18	//1 or 2 for <=8MHz, 7 for 11.0592MHz, 18 for 14.7456MHz,
			
	#define ONE	2


	#define DALLAS_PORT PORTB
	#define DALLAS_PIN PINB
	#define DALLAS (1<<PB2)


	uint8_t dallas(char *pData_Dallas, uint8_t command);




