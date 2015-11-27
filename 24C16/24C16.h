#ifdef	__24C16
#define __24C16

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

//	#define false 0
//	#define true 1

	uint8_t EEWriteByte(uint8_t addr_twi, uint16_t address,uint8_t data);
	uint8_t EEReadByte(uint8_t addr_twi, uint16_t address);

	void TWIOpen(void);

#endif

