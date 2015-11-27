

// Standard Input/Output functions.
	#include <stdio.h>

	#define CCLK_GET	for(uint8_t n=0; n<sizeof(at_get_time); n++) usart0_write(at_get_time[n]);
	#define CCLK_SET	for(uint8_t n=0; n<sizeof(at_set_time); n++) usart0_write(at_set_time[n]);
	#define CCLK_SET_END	for(uint8_t n=0; n<sizeof(at_set_time_end); n++) usart0_write(at_set_time_end[n]);
	
	char csec[2];
	char chours[2];
	char cmin[2];

	char cmonth[2];
	char cday[2];
	char cyear[2];

	char data_time[17];

	uint8_t GetTimeSIM();
	uint8_t SetTimeSIM();

