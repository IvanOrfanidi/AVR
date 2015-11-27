
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>
	#include <util/delay.h>

	#include <stdio.h>



 	#define ADDR_EEP_CONFIG_U1 0x0177
 	#define ADDR_EEP_CONFIG_U2 0x017C

 	#define ADDR_EEP_CONFIG_S1 0x0181
 	#define ADDR_EEP_CONFIG_S2 0x0184

 	#define ADDR_EEP_CONFIG_OUT_U1 0x0187
 	#define ADDR_EEP_CONFIG_OUT_U2 0x018B

 	#define ADDR_EEP_CONFIG_TIME_US 0x018F
 	#define ADDR_EEP_CONFIG_ALARM_TIME_US 0x0193

