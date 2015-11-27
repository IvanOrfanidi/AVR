
// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

	#define COMMAND_HOLDING_REG	 3
	#define COMMAND_INPUT_REG	 4
	#define COMMAND_WRITE_REG	 16


	#define ADDRES_REG_READ 	1	// мювюкэмши юдпея пецхярпнб времхъ //
	#define QUANTITY_REG_READ 	4	// йнкхвеярбн пецхярпнб времхъ //

	#define ADDRES_REG_WRITE 	4	// мювюкэмши юдпея пецхярпнб гюохях //
	#define QUANTITY_REG_WRITE 	1	// йнкхвеярбн пецхярпнб гюохях //

	#define ADDRES_REG		 	1
	#define QUANTITY_REG 		4

	#define BUFFER_SIZE_DATA	64
	
	uint16_t rx_len;
	uint8_t slave_id;			// ме наундхлн сйюгюрэ б main ОПНЦПЮЛЛЕ//


	uint8_t data[BUFFER_SIZE_DATA];

	int16_t registers[ADDRES_REG+QUANTITY_REG];	//наыемхе я main вепег щрнр люяхб!!!//


	int modbus(); // 0-OK; 1-NOT SLAVE ADDRESS; 2-ERROR; 3-ERROR CRC;//

	void modbus_true();
