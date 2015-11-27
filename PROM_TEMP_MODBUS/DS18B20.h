
#ifndef DS18B20_H
   #define DS18B20_H

// Input/Output functions.
	#include <avr/io.h>
	#include <util/delay.h>
	#include <avr/pgmspace.h>
	#include <avr/interrupt.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>


	#define PORT_DS18B20 	PORTC
	#define PIN_DS18B20 	PINC
	#define DS18B20			PC0
	#define DDR_DS18B20		DDRC

	int DS18B20_Start_Converts(void);
	int DS18B20_Temperature(char *ptrTemperature);



enum {

	OK_CONNECT=0, ERROR_CONNECT
};

#endif
