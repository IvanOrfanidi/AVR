

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>

	#include "TWI.h"


	void GetDateTimeRTC(char *pDateTime);
	uint8_t SetDateTimeRTC(const char *pDateTime);

	void GetTimeRTC(char *pTime);
	uint8_t SetTimeRTC(const char *pTime);


	#define SLAVE_ADDR_RTC 0b10100000


	#define ADDR_SEC 0x02
	#define ADDR_MIN 0x03
	#define ADDR_HOUR 0x04
      

	#define ADDR_DAY 0x05
	#define ADDR_MON 0x06
	#define ADDR_YEUR 0x10

	enum{
		OK=0
	};
