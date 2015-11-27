

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

//	#define SLAVE_ADDR_EEP 0b10101110
	#define SLAVE_ADDR_EEP 0b10100000

	#define false 0
	#define true 1

	uint8_t EEWriteByte(uint16_t address,uint8_t data);
	uint8_t EEReadByte(uint16_t address);

	void EEOpen();
