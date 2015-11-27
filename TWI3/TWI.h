

	#include <stdio.h>


#define FALSE 0
#define TRUE 1
 
void TWIOpen(uint8_t baudrate);

uint8_t TWIWriteByte(uint8_t slave_address, uint8_t word, uint16_t address, uint8_t data);

uint8_t TWIReadByte(uint8_t slave_address, uint8_t word, uint16_t address);

