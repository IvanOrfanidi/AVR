#ifdef	__24CXX
#define __24CXX

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

	uint8_t EEWriteByte(uint8_t addr_twi, uint16_t address,uint8_t data);
	uint8_t EEReadByte(uint8_t addr_twi, uint16_t address);

	void EEOpen(void);

#endif
