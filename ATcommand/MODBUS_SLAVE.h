
// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>



	#define COMMAND_HOLDING_REG	 0x03
	#define COMMAND_INPUT_REG	 0x04
	#define COMMAND_WRITE_REG	 0x10


	#define ADDRES_REG_READ 	1
	#define QUANTITY_REG_READ 	20

	#define ADDRES_REG_WRITE 	10
	#define QUANTITY_REG_WRITE 	10

	#define ADDRES_REG		 	1
	#define QUANTITY_REG 		20
	
	uint16_t rx_len;
	uint8_t slave_id;			// ÍÅ ÎÁÕÎÄÈÌÎ ÓÊÀÇÀÒÜ Â main ןנמדנאללו//


	char data[64];
	int16_t registers[ADDRES_REG+QUANTITY_REG];

	int modbus(); // 0-NOT SLAVE ADDRESS; 1-ERROR; 2-OK //

