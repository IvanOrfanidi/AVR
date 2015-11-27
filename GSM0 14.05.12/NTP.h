

	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>
	#include <util/delay.h>

	#include <stdio.h>


	//ÏÀÐÀÌÅÒÐÛ GPRS ÑÎÅÄÈÍÅÍÈß//
	#define ADDR_EEP_APN 				0x010C
	#define ADDR_EEP_USER_NAME 			0x0121
	#define ADDR_EEP_USER_PAS 			0x012C
	#define ADDR_EEP_USER_PAS_END		0x0137
	#define ADDR_EEP_DOMAIN_NAME 		0x0137
	#define ADDR_EEP_DOMAIN_NAME_END 	0x0155
	#define ADDR_EEP_CONFIG_GPRS	 	0x0156

	
	//////////////////////////////////////////////////////////////////////////////////

	#define TIMEOUT		if(timeout>=TIMEOUT_MAX) timeout++; else timeout++;
	#define TIMEOUT_MAX 50000
	uint16_t timeout;

	uint8_t isec;
	uint8_t ihours;
	uint8_t ihours_ntp;
	uint8_t imin;


