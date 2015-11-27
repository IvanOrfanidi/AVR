

	#define false 0
	#define true 1
 
	#include <ctype.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>


	#define SLAVE_ADD_TWI 0b10100100

	uint8_t EEReadByte(uint8_t address);
	uint8_t EEWriteByte(uint8_t address,uint8_t data);

	void EEOpen(uint8_t baudrate);
