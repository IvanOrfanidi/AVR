

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>



	uint8_t GetDataTimeRTC(char *pDateTime, uint8_t len);
	uint8_t SetDataTimeRTC(const char *pDateTime);

	uint8_t GetTimeRTC(char *pDateTime, uint8_t len);
	uint8_t SetTimeRTC(const char *pDateTime);


	#define SLAVE_ADDR_RTC 0b10100010


	#define ADDR_SEC 0x02
	#define ADDR_MIN 0x03
	#define ADDR_HOUR 0x04
      

	#define ADDR_DAY 0x05
	#define ADDR_MON 0x06
	#define ADDR_YEUR 0x10

