

	#include <stdio.h>

	#include <util/crc16.h>


	#include "MODBUS_SLAVE.h"

	uint16_t i;
	uint8_t modbus_len;

	uint16_t add_reg;
	uint16_t quant_reg;

	uint8_t function;

// 0-NOT SLAVE ADDRESS; 1-ERROR; 2-OK //

int modbus()
{
	
	if(data[0]!=slave_id) return 0;

	uint16_t modbus_crc=0xFFFF;

	for(i=0; i<(rx_len-2); i++)
	{
		modbus_crc=_crc16_update(modbus_crc, data[i]);
	}


	uint8_t modbus_crc_l=modbus_crc>>8;
	uint8_t modbus_crc_h=modbus_crc;

	if((modbus_crc_h!=data[rx_len-2]) | (modbus_crc_l!=data[rx_len-1])) return 0; // Error CRC.

	function=data[1];
	switch(function)
	{

	case COMMAND_HOLDING_REG: return(read_holding_reg()); break;

	case COMMAND_INPUT_REG: return(read_input_reg()); break;

	case COMMAND_WRITE_REG: return(write_reg()); break;

	}
	

	//Ilegal Function///

	char data_err[5]={slave_id, 0x83, 0x01, 0xFF, 0xFF};

	modbus_crc=0xFFFF;
	for(i=0; i<(sizeof(data_err)-2); i++)
	{
		modbus_crc=_crc16_update(modbus_crc, data_err[i]);
	}

	data_err[(sizeof(data_err)-2)]=modbus_crc;
	data_err[(sizeof(data_err)-1)]=modbus_crc>>8;

	for(i=0; i<sizeof(data_err); i++)
	{
		usart0_write(data_err[i]);
	}


return 1;	
}

int read_holding_reg()
{

	add_reg=8<<data[2];
	add_reg=data[3];

	if(add_reg<ADDRES_REG_READ)
	{
		error_add_reg();
		return 1;
	}

	
	quant_reg=8<<data[4];
	quant_reg=data[5];

	if(quant_reg>QUANTITY_REG_READ)
	{
		error_add_reg();
		return 1;
	}


	if((add_reg+quant_reg)>(ADDRES_REG_READ+QUANTITY_REG_READ))
	{
		error_add_reg();
		return 1;
	}



	uint8_t data_tx[5+quant_reg*2];

	for(i=0; i<2; i++)
	{
		data_tx[i]=data[i];	
	}


	data_tx[2]=quant_reg*2;


	uint16_t n=add_reg;
	for(i=0; n<(quant_reg+add_reg); i+=2)
	{
		data_tx[i+4]=registers[n];
		data_tx[i+3]=registers[n]>>8;

		n++;
	}



	uint16_t modbus_crc=0xFFFF;
	for(i=0; i<(sizeof(data_tx)-2); i++)
	{
		modbus_crc=_crc16_update(modbus_crc, data_tx[i]);
	}

	
	data_tx[(sizeof(data_tx)-2)]=modbus_crc;
	data_tx[(sizeof(data_tx)-1)]=modbus_crc>>8;


	for(i=0; i<sizeof(data_tx); i++)
	{
		usart0_write(data_tx[i]);
	}

return 2;
}

int read_input_reg()
{

read_holding_reg();

return 2;
}

int write_reg()
{


	add_reg=8<<data[2];
	add_reg=data[3];

	if(add_reg<ADDRES_REG_WRITE)
	{
		error_add_reg();
		return 1;
	}

	
	quant_reg=8<<data[4];
	quant_reg=data[5];

	if(quant_reg>QUANTITY_REG_WRITE)
	{
		error_add_reg();
		return 1;
	}


	if((add_reg+quant_reg)>(ADDRES_REG_WRITE+QUANTITY_REG_WRITE))
	{
		error_add_reg();
		return 1;
	}
	
	uint8_t n=7;
	for(i=add_reg; i<(add_reg+quant_reg); i++)
	{
		registers[i]=data[n]*256;
		registers[i]|=data[n+1];

		n=n+2;
	}



	uint8_t data_tx[8];

	for(i=0; i<(sizeof(data_tx)-2); i++)
	{
		data_tx[i]=data[i];	
	}

	uint16_t modbus_crc=0xFFFF;
	for(i=0; i<(sizeof(data_tx)-2); i++)
	{
		modbus_crc=_crc16_update(modbus_crc, data_tx[i]);
	}

	data_tx[(sizeof(data_tx)-2)]=modbus_crc;
	data_tx[(sizeof(data_tx)-1)]=modbus_crc>>8;


	for(i=0; i<sizeof(data_tx); i++)
	{
		usart0_write(data_tx[i]);
	}


return 2;
}


void error_add_reg()
{

	char data_err[5]={slave_id, 0x83, 0x02, 0xFF, 0xFF};

	uint16_t modbus_crc=0xFFFF;
	for(i=0; i<(sizeof(data_err)-2); i++)
	{
		modbus_crc=_crc16_update(modbus_crc, data_err[i]);
	}

	data_err[(sizeof(data_err)-2)]=modbus_crc;
	data_err[(sizeof(data_err)-1)]=modbus_crc>>8;

	for(i=0; i<sizeof(data_err); i++)
	{
		usart0_write(data_err[i]);
	}
}



void error_data_value()
{
	char data_err[5]={slave_id, 0x83, 0x03, 0xFF, 0xFF};

	uint16_t modbus_crc=0xFFFF;
	for(i=0; i<(sizeof(data_err)-2); i++)
	{
		modbus_crc=_crc16_update(modbus_crc, data_err[i]);
	}

	data_err[(sizeof(data_err)-2)]=modbus_crc;
	data_err[(sizeof(data_err)-1)]=modbus_crc>>8;

	for(i=0; i<sizeof(data_err); i++)
	{
		usart0_write(data_err[i]);
	}

}
