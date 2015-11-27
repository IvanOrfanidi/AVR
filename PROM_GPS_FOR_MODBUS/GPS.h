

//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>


	uint8_t gps_status(char *pDataRX, uint8_t Len);

	uint8_t gps_time(char *pDataTX, char *pDataRX, uint8_t Len);

	uint8_t gps_latitude(char *pDataTX, char *pDataRX, uint8_t Len);

	uint8_t gps_longitude(char *pDataTX, char *pDataRX, uint8_t Len);
