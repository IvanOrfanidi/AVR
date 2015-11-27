
	#include "TWI.h"

	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>

void TWIOpen(uint8_t baudrate)
{	
	TWBR = baudrate;
	TWCR=(1<<TWEN);

}
 
uint8_t TWIWriteByte(uint8_t slave_address, uint8_t word, uint16_t address, uint8_t data)
{	
do	{
		//Put Start Condition on TWI Bus
		TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
 
		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));
 
		if((TWSR & 0xF8) != 0x08)
			return FALSE;
 
		//Now write SLA+W
		//EEPROM @ 00h
		TWDR=slave_address|0;	
 
		TWCR=(1<<TWINT)|(1<<TWEN);
 
		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));
 
	}

	while((TWSR & 0xF8) != 0x18);

if(word==2)
{
	//Now write ADDRH
	TWDR=(address>>8);
 
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	if((TWSR & 0xF8) != 0x28)
		return FALSE;

}
 
	//Now write ADDRL
	TWDR=(address);
 
	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x28)
		return FALSE;
 
	//Now write DATA
	TWDR=(data);
 
	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x28)
		return FALSE;
 
	//Put Stop Condition on bus
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
 
	//Wait for STOP to finish
	while(TWCR & (1<<TWSTO));
 
	//Wait untill Writing is complete
	_delay_ms(12);
 
	//Return TRUE
	return TRUE;
}
 

uint8_t TWIReadByte(uint8_t slave_address, uint8_t word, uint16_t address)
{	
uint8_t data;
//Initiate a Dummy Write Sequence to start Random Read
	do	{
		//Put Start Condition on TWI Bus
		TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
 
		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));
 
		//Check status
		if((TWSR & 0xF8) != 0x08) return FALSE;
 
		//Now write SLA+W
		//EEPROM @ 00h
		TWDR=slave_address|0;	
 
		//Initiate Transfer
		TWCR=(1<<TWINT)|(1<<TWEN);
 
		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));
 
	}

	while((TWSR & 0xF8) != 0x18);

if(word==2)
{
	//Now write ADDRH
	TWDR=(address>>8);
 
	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x28)
	return FALSE;
}


	//Now write ADDRL
	TWDR=(address);
 
	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x28)
	return FALSE;
 
	//*************************DUMMY WRITE SEQUENCE END **********************
 
	//Put Start Condition on TWI Bus
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x10)
	return FALSE;
 
	//Now write SLA+R
	//EEPROM @ 00h
	TWDR=slave_address|1;	
 
	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x40)
	return FALSE;
 
	//Now enable Reception of data by clearing TWINT
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	while(!(TWCR & (1<<TWINT)));
 
	if((TWSR & 0xF8) != 0x58)
	return FALSE;
 
	data=TWDR;
 
	//Put Stop Condition on bus
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
 
	//Wait for STOP to finish
	while(TWCR & (1<<TWSTO));
	//Return TRUE
	return data;
}
