
	#include "24c16.h"

	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>

void EEOpen(uint8_t baudrate)
{	
	TWBR = baudrate;
	TWCR=(1<<TWEN);

}
 
uint8_t EEWriteByte(uint8_t address,uint8_t data)
{	do	{
		//Put Start Condition on TWI Bus
		TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
 
		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));
 
		if((TWSR & 0xF8) != 0x08)
			return false;
 
		//Now write SLA+W
		//EEPROM @ 00h
		TWDR=SLAVE_ADD_TWI|0;	
 
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
		return false;
 
	//Now write DATA
	TWDR=(data);
 
	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x28)
		return false;
 
	//Put Stop Condition on bus
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
 
	//Wait for STOP to finish
	while(TWCR & (1<<TWSTO));
 
	//Wait untill Writing is complete
	_delay_ms(12);
 
	//Return TRUE
	return true;
}
 
uint8_t EEReadByte(uint8_t address)
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
		TWDR=SLAVE_ADD_TWI|0;	
 
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
	return false;
 
	//*************************DUMMY WRITE SEQUENCE END **********************
 
	//Put Start Condition on TWI Bus
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x10)
	return false;
 
	//Now write SLA+R
	//EEPROM @ 00h
	TWDR=SLAVE_ADD_TWI|1;	
 
	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));
 
	//Check status
	if((TWSR & 0xF8) != 0x40)
	return false;
 
	//Now enable Reception of data by clearing TWINT
	TWCR=(1<<TWINT)|(1<<TWEN);
 
	while(!(TWCR & (1<<TWINT)));
 
	if((TWSR & 0xF8) != 0x58)
	return false;
 
	data=TWDR;
 
	//Put Stop Condition on bus
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
 
	//Wait for STOP to finish
	while(TWCR & (1<<TWSTO));
	//Return TRUE
	return data;
}
