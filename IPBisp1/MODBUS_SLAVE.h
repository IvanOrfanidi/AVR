
// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <avr/io.h>
	#include <util/delay.h>
	#include <util/crc16.h>

	#include "USART.h"

	#define COMMAND_HOLDING_REG	 3
	#define COMMAND_INPUT_REG	 4
	#define COMMAND_WRITE_REG	 16

//	#define PORT_RS485 PORTD	//!
	#define RE_DE PD6			//!
	#define USART_MODBUS 0		//мНЛЕП ОНПРЮ ОН ЛНДАЮЯС.

	#define ADDRES_REG_READ 	1	// мювюкэмши юдпея пецхярпнб времхъ //
	#define QUANTITY_REG_READ 	6	// йнкхвеярбн пецхярпнб времхъ //

	#define ADDRES_REG_WRITE 	0	// мювюкэмши юдпея пецхярпнб гюохях //
	#define QUANTITY_REG_WRITE 	0	// йнкхвеярбн пецхярпнб гюохях //

	#define ADDRES_REG		 	1
	#define QUANTITY_REG 		6
	
	
	int16_t registers[ADDRES_REG+QUANTITY_REG];	//наыемхе c main вепег щрнр люяхб!!!//


	int modbus_slave(uint8_t slave_add); // 0-OK; 1-NOT SLAVE ADDRESS; 2-ERROR; 3-ERROR CRC;//

enum {
	OK=0, NOT_SLAVE_ID, ERROR_ADDR_REG, ERROR_FUNCTION, ERROR_CRC
};
