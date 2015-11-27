#ifndef MODBUS_SLAVE_H_
#define MODBUS_SLAVE_H_


// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>



	#define COMMAND_HOLDING_REG	 0x03
	#define COMMAND_INPUT_REG	 0x04
	#define COMMAND_WRITE_REG	 0x10


	#define ADDRES_REG_READ 	1	// ��������� ����� ��������� ������ //
	#define QUANTITY_REG_READ 	4	// ���������� ��������� ������ //

	#define ADDRES_REG_WRITE 	1	// ��������� ����� ��������� ������ //
	#define QUANTITY_REG_WRITE 	2	// ���������� ��������� ������ //

	#define ADDRES_REG		 	1
	#define QUANTITY_REG 		4

	#define BUFFER_SIZE_DATA	64
	
	uint16_t rx_len;
	uint8_t slave_id;			// �� �������� ������� � main ���������//


	uint8_t data[BUFFER_SIZE_DATA];

	int16_t registers[ADDRES_REG+QUANTITY_REG];	//������� � main ����� ���� �����!!!//






	int modbus(); // 0-OK; 1-ERROR; 2-NOT SLAVE ADDRESS; //

	#endif

