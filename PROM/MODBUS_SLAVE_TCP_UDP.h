


	#include <util/delay.h>
	#include <util/crc16.h>
    #include <avr/pgmspace.h>

	#define MODBUS_TCP_UDP
//	#define MODBUS_RTU_TCP_UDP


	#define COMMAND_HOLDING_REG	 3
	#define COMMAND_INPUT_REG	 4
	#define COMMAND_WRITE_REG	 16

	#define ADDRES_REG_READ 	1	// мювюкэмши юдпея пецхярпнб времхъ //
	#define QUANTITY_REG_READ 	1	// йнкхвеярбн пецхярпнб времхъ //

	#define ADDRES_REG_WRITE 	1	// мювюкэмши юдпея пецхярпнб гюохях //
	#define QUANTITY_REG_WRITE 	1	// йнкхвеярбн пецхярпнб гюохях //

	#define ADDRES_REG		 	1
	#define QUANTITY_REG 		1
	
	
	int16_t registers[ADDRES_REG+QUANTITY_REG];	//наыемхе c main вепег щрнр люяхб!!!//


	extern int modbus_slave_status(char *ptrData_rx, int len_data_rx, int slave_id, char *ptrLen_data_tx); // 0-OK; 1-NOT SLAVE ADDRESS; 2-ERROR; 3-ERROR CRC;//
	extern void modbus_slave_tcp_udp(char *ptrData_rx, char *ptrData_tx);



enum {
	OK=0, NOT_SLAVE_ID, ERROR_ADDR_REG, ERROR_FUNCTION, ERROR_CRC
};
