

	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>
	#include <util/delay.h>

	#include <stdio.h>


	//œ¿–¿Ã≈“–€ GPRS —Œ≈ƒ»Õ≈Õ»ﬂ//
	#define ADDR_EEP_APN 				0x010C
	#define ADDR_EEP_USER_NAME 			0x0121
	#define ADDR_EEP_USER_PAS 			0x012C
	#define ADDR_EEP_USER_PAS_END		0x0137
	#define ADDR_EEP_DOMAIN_NAME1 		0x0137
	#define ADDR_EEP_DOMAIN_NAME_END1 	0x0155
	#define ADDR_EEP_DOMAIN_NAME2 		0x0156
	#define ADDR_EEP_DOMAIN_NAME_END2 	0x0174

	
	//////////////////////////////////////////////////////////////////////////////////

	uint8_t isec;
	uint8_t ihours;
	uint8_t ihours_ntp;
	uint8_t imin;

	uint8_t GetTimeNTP(uint8_t domain_name);

