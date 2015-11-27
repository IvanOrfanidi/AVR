
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>
	#include <util/delay.h>

	#include <stdio.h>



 	#define ADDR_EEP_CONFIG_U1 0x0177	//5
 	#define ADDR_EEP_CONFIG_U2 0x017C	//5

 	#define ADDR_EEP_CONFIG_S1 0x0181	//3
 	#define ADDR_EEP_CONFIG_S2 0x0184	//3

 	#define ADDR_EEP_CONFIG_OUT_U1 0x0187	//4
 	#define ADDR_EEP_CONFIG_OUT_U2 0x018B	//4

 	#define ADDR_EEP_CONFIG_TIME_US 0x018F			//4
   	#define ADDR_EEP_CONFIG_TIME_ALARM_US 0x0193	//4
   	#define ADDR_EEP_CONFIG_TIME_ALARM_VISIBLE 0x0197	//1

	#define ADDR_EEP_SAVE_CONFIG 0x0198					//30


