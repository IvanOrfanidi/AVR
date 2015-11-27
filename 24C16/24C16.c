
	#include "24C16.h"

	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>

	#define SPEED_TWI 0x30

void TWIOpen(void)
{	
	TWBR = SPEED_TWI;
	TWCR=(1<<TWEN);

}
 
uint8_t EEWriteByte(uint8_t addr_twi, uint16_t address, uint8_t data)
{	do	{
		//Put Start Condition on TWI Bus
		TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
 
		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));
 
		if((TWSR & 0xF8) != 0x08)
			return 0;
 
		//Now write SLA+W
		//EEPROM @ 00h
		TWDR=addr_twi|0;	
 
		TWCR=(1<<TWINT)|(1<<TWEN);
 
		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));
 
	}while((TWSR & 0xF8) != 0x18);
 
 /*
	//Now write ADDRH
	TWDR=(address>>8);
 
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	if((TWSR & 0xF8) != 0x28)
		return FALSE;
*/ 
	//Now write ADDRL
	TWDR=(address);
 
	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x28)
		return 0;
 
	//Now write DATA
	TWDR=(data);
 
	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x28)
		return 0;
 
	//Put Stop Condition on bus
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
 
	//Wait for STOP to finish
	while(TWCR & (1<<TWSTO));
 
	//Wait untill Writing is complete
	volatile uint16_t i=0;
	for(; i<1000; i++)
		asm   ("nop");

	//Return TRUE
	return 1;
}
 
uint8_t EEReadByte(uint8_t addr_twi, uint16_t address)
{	uint8_t data;
//Initiate a Dummy Write Sequence to start Random Read
	do	{
		//Put Start Condition on TWI Bus
		TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
 
		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));
 
		//Check status
		if((TWSR & 0xF8) != 0x08);
 
		//Now write SLA+W
		//EEPROM @ 00h
		TWDR=addr_twi|0;	
 
		//Initiate Transfer
		TWCR=(1<<TWINT)|(1<<TWEN);
 
		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));
 
	}while((TWSR & 0xF8) != 0x18);

/* 
	//Now write ADDRH
	TWDR=(address>>8);
 
	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x28)
	return FALSE;
*/ 
	//Now write ADDRL
	TWDR=(address);
 
	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x28)
	return 0;
 
	//*************************DUMMY WRITE SEQUENCE END **********************
 
	//Put Start Condition on TWI Bus
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x10)
	return 0;
 
	//Now write SLA+R
	//EEPROM @ 00h
	TWDR=addr_twi|1;	
 
	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x40)
	return 0;
 
	//Now enable Reception of data by clearing TWINT
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	while(!(TWCR & (1<<TWINT)));
 
	if((TWSR & 0xF8) != 0x58)
	return 0;
 
	data=TWDR;
 
	//Put Stop Condition on bus
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
 
	//Wait for STOP to finish
	while(TWCR & (1<<TWSTO));
	//Return TRUE
	return data;
}
