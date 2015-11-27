

#include "MODBUS_SLAVE_TCP_UDP.h"



///////////////////////PROTOCOL MODBUS TCP or UDP /////////////////////////////////////////////////

#ifdef MODBUS_TCP_UDP

int modbus_slave_status(char *ptrData_tx, int len_data_rx, int slave_id, char *ptrLen_data_tx)
{

	char Data[len_data_rx-6];
	

	for(uint8_t i=0; i<sizeof(Data); i++)
		Data[i]=ptrData_tx[i+6];

	if(Data[0]!=slave_id) return NOT_SLAVE_ID;	//1-NOT SLAVE ADDRESS.

	uint16_t add_reg;

	add_reg=8<<Data[2];
	add_reg=Data[3];

	uint16_t quant_reg;
	quant_reg=8<<Data[4];
	quant_reg=Data[5];

	uint8_t err_addr_reg = 0;

	if(((Data[1]==COMMAND_HOLDING_REG) || (Data[1]==COMMAND_INPUT_REG)) &&
	  ((add_reg<ADDRES_REG_READ) || (quant_reg>QUANTITY_REG_READ) ||
	  (add_reg+quant_reg)>(ADDRES_REG_READ+QUANTITY_REG_READ)))
	{
		err_addr_reg = 1;
	}
	
	if((Data[1]==COMMAND_WRITE_REG) && (((add_reg<ADDRES_REG_WRITE) ||
	  (quant_reg>QUANTITY_REG_WRITE) || (quant_reg>QUANTITY_REG_WRITE) ||
	  (add_reg+quant_reg)>(ADDRES_REG_WRITE+QUANTITY_REG_WRITE))))
	{
		err_addr_reg = 1;
	}
	
	if(err_addr_reg)	// ERROR ADDRESS REGISTRS
	{
		ptrLen_data_tx[0] = 9;
		return ERROR_ADDR_REG;
	}

	
	if((Data[1]==COMMAND_HOLDING_REG) || (Data[1]==COMMAND_INPUT_REG))
	{
		ptrLen_data_tx[0] = 9+quant_reg*2;
		return OK;

	}

	if(Data[1]==COMMAND_WRITE_REG)
	{
		ptrLen_data_tx[0] = 12;
		return OK;
	}


 // ERROR FUNCTION //


	ptrLen_data_tx[0] = 9;
return ERROR_FUNCTION;		///Ilegal Function///


}

//////////////////////////////////////////////////////////////////////////


void modbus_slave_tcp_udp(char *ptrData_rx, char *ptrData_tx)
{
	
	for(uint8_t i=0; i<8; i++)
	{
			ptrData_tx[i] = ptrData_rx[i];
	}
	
	uint16_t add_reg;

	add_reg=8<<ptrData_rx[8];
	add_reg=ptrData_rx[9];

	uint16_t quant_reg;
	quant_reg=8<<ptrData_rx[10];
	quant_reg=ptrData_rx[11];


	uint8_t err_addr_reg = 0;

	if(((ptrData_rx[7]==COMMAND_HOLDING_REG) || (ptrData_rx[7]==COMMAND_INPUT_REG)) &&
	  ((add_reg<ADDRES_REG_READ) || (quant_reg>QUANTITY_REG_READ) ||
	  (add_reg+quant_reg)>(ADDRES_REG_READ+QUANTITY_REG_READ)))
	{
		err_addr_reg = 1;
	}
	
	if((ptrData_rx[7]==COMMAND_WRITE_REG) && (((add_reg<ADDRES_REG_WRITE) ||
	  (quant_reg>QUANTITY_REG_WRITE) || (quant_reg>QUANTITY_REG_WRITE) ||
	  (add_reg+quant_reg)>(ADDRES_REG_WRITE+QUANTITY_REG_WRITE))))
	{
		err_addr_reg = 2;
	}
	
	if(err_addr_reg==1)	// ERROR ADDRESS REGISTRS READ
	{
		ptrData_tx[7] = 0x83;
		ptrData_tx[8] = 0x02;
	}

	if(err_addr_reg==2)	// ERROR ADDRESS REGISTRS WRITE
	{
		
		ptrData_tx[7] = 0x90;
		ptrData_tx[8] = 0x02;
	}

	if(!(err_addr_reg))
	{
		if(( ptrData_rx[7]==COMMAND_HOLDING_REG ) || ( ptrData_rx[7]==COMMAND_INPUT_REG ))	//Function Read.
		{


			ptrData_tx[8] = quant_reg*2;
		
			uint16_t size_modbus_udp = ptrData_tx[8]+3;

			ptrData_tx[5] = size_modbus_udp;
			ptrData_tx[4] = size_modbus_udp>>8;

			uint16_t n=add_reg;

			for(uint16_t i=0; n<(quant_reg+add_reg); i+=2)
			{
				ptrData_tx[i+10]=registers[n];
				ptrData_tx[i+9]=registers[n]>>8;
				n++;
			}

		}

		if( ptrData_rx[7]==COMMAND_WRITE_REG )											//Function Write.
		{
		
			uint8_t n=13;
			for(uint16_t i=add_reg; i<(add_reg+quant_reg); i++)
			{
				registers[i]=ptrData_rx[n]*256;
				registers[i]|=ptrData_rx[n+1];

				n=n+2;
			}

		
			for(uint8_t i=6; i<12; i++)
			{
				ptrData_tx[i] = ptrData_rx[i];
			}
		
			ptrData_tx[5] = 0x06;
			ptrData_tx[4] = 0x00;
		}

		///Ilegal Function///
		if(!(( ptrData_rx[7]==COMMAND_HOLDING_REG ) || ( ptrData_rx[7]==COMMAND_INPUT_REG ) || ( ptrData_rx[7]==COMMAND_WRITE_REG )))
		{

			ptrData_tx[7] = 0x83;
			ptrData_tx[8] = 0x01;
		}
	}

}

#endif

///////////////////////PROTOCOL MODBUS RTU TCP or UDP /////////////////////////////////////////////////

#ifdef MODBUS_RTU_TCP_UDP

int modbus_slave_status(char *ptrData_rx, int len_data_rx, int slave_id, char *ptrLen_data_tx)
{

	if(ptrData_rx[0]!=slave_id) return NOT_SLAVE_ID;	//1-NOT SLAVE ADDRESS.


	uint16_t modbus_crc=0xFFFF;

	for(uint16_t i=0; i<len_data_rx; i++)
		modbus_crc=_crc16_update(modbus_crc, ptrData_rx[i]);

	if(modbus_crc)	return ERROR_CRC; // Error CRC

	
	uint16_t add_reg;

	add_reg=8<<ptrData_rx[2];
	add_reg=ptrData_rx[3];

	uint16_t quant_reg;
	quant_reg=8<<ptrData_rx[4];
	quant_reg=ptrData_rx[5];

	uint8_t err_addr_reg = 0;

	if(((ptrData_rx[1]==COMMAND_HOLDING_REG) || (ptrData_rx[1]==COMMAND_INPUT_REG)) &&
	  ((add_reg<ADDRES_REG_READ) || (quant_reg>QUANTITY_REG_READ) ||
	  (add_reg+quant_reg)>(ADDRES_REG_READ+QUANTITY_REG_READ)))
	{
		err_addr_reg = 1;
	}
	
	if((ptrData_rx[1]==COMMAND_WRITE_REG) && (((add_reg<ADDRES_REG_WRITE) ||
	  (quant_reg>QUANTITY_REG_WRITE) || (quant_reg>QUANTITY_REG_WRITE) ||
	  (add_reg+quant_reg)>(ADDRES_REG_WRITE+QUANTITY_REG_WRITE))))
	{
		err_addr_reg = 1;
	}
	
	if(err_addr_reg)	// ERROR ADDRESS REGISTRS
	{
		ptrLen_data_tx[0] = 5;
		return ERROR_ADDR_REG;
	}

	
	if((ptrData_rx[1]==COMMAND_HOLDING_REG) || (ptrData_rx[1]==COMMAND_INPUT_REG))
	{
		ptrLen_data_tx[0] = 5+quant_reg*2;
		return OK;

	}

	if(ptrData_rx[1]==COMMAND_WRITE_REG)
	{
		ptrLen_data_tx[0] = 8;
		return OK;
	}


 // ERROR FUNCTION //


	ptrLen_data_tx[0] = 5;

return ERROR_FUNCTION;		///Ilegal Function///

}

//////////////////////////////////////////////////////////////////////////

void modbus_slave_tcp_udp(char *ptrData_rx, char *ptrData_tx)
{

	ptrData_tx[0] = ptrData_rx[0];
	
	uint16_t add_reg;

	add_reg=8<<ptrData_rx[2];
	add_reg=ptrData_rx[3];

	uint16_t quant_reg;
	quant_reg=8<<ptrData_rx[4];
	quant_reg=ptrData_rx[5];


	uint8_t err_addr_reg = 0;

	if(((ptrData_rx[1]==COMMAND_HOLDING_REG) || (ptrData_rx[1]==COMMAND_INPUT_REG)) &&
	  ((add_reg<ADDRES_REG_READ) || (quant_reg>QUANTITY_REG_READ) ||
	  (add_reg+quant_reg)>(ADDRES_REG_READ+QUANTITY_REG_READ)))
	{
		err_addr_reg = 1;
	}
	
	if((ptrData_rx[1]==COMMAND_WRITE_REG) && (((add_reg<ADDRES_REG_WRITE) ||
	  (quant_reg>QUANTITY_REG_WRITE) || (quant_reg>QUANTITY_REG_WRITE) ||
	  (add_reg+quant_reg)>(ADDRES_REG_WRITE+QUANTITY_REG_WRITE))))
	{
		err_addr_reg = 2;
	}
	
	if(err_addr_reg==1)	// ERROR ADDRESS REGISTRS READ
	{
		ptrData_tx[1] = 0x83;
		ptrData_tx[2] = 0x02;
	}

	if(err_addr_reg==2)	// ERROR ADDRESS REGISTRS WRITE
	{
		
		ptrData_tx[1] = 0x90;
		ptrData_tx[2] = 0x02;
	}

	if(err_addr_reg)
	{
		uint16_t modbus_crc=0xFFFF;
		for(uint16_t i=0; i<3; i++)
			modbus_crc=_crc16_update(modbus_crc, ptrData_tx[i]);
			
		ptrData_tx[3]=modbus_crc;
		ptrData_tx[4]=modbus_crc>>8;
	}

	
	if(!(err_addr_reg))
	{
		if(( ptrData_rx[1]==COMMAND_HOLDING_REG ) || ( ptrData_rx[1]==COMMAND_INPUT_REG ))	//Function Read.
		{
			ptrData_tx[1]=ptrData_rx[1];

			ptrData_tx[2]=quant_reg*2;

			uint16_t n=add_reg;
			for(uint16_t i=0; n<(quant_reg+add_reg); i+=2)
			{
				ptrData_tx[i+4]=registers[n];
				ptrData_tx[i+3]=registers[n]>>8;

				n++;
			}

			uint16_t modbus_crc=0xFFFF;
			for(uint16_t i=0; i<(3+quant_reg*2); i++)
				modbus_crc=_crc16_update(modbus_crc, ptrData_tx[i]);
	

	
			ptrData_tx[(3+quant_reg*2)]=modbus_crc;
			ptrData_tx[(4+quant_reg*2)]=modbus_crc>>8;


		}


		if( ptrData_rx[1]==COMMAND_WRITE_REG )											//Function Write.
		{
			uint8_t n=7;
			for(uint16_t i=add_reg; i<(add_reg+quant_reg); i++)
			{
				registers[i]=ptrData_rx[n]*256;
				registers[i]|=ptrData_rx[n+1];

				n=n+2;
			}

			for(uint16_t i=0; i<6; i++)
			{
				ptrData_tx[i]=ptrData_rx[i];	
			}

			uint16_t modbus_crc=0xFFFF;
			for(uint16_t i=0; i<6; i++)
			modbus_crc=_crc16_update(modbus_crc, ptrData_tx[i]);
		

			ptrData_tx[6]=modbus_crc;
			ptrData_tx[7]=modbus_crc>>8;


		}


		///Ilegal Function///
		if(!(( ptrData_rx[1]==COMMAND_HOLDING_REG ) || ( ptrData_rx[1]==COMMAND_INPUT_REG ) || ( ptrData_rx[1]==COMMAND_WRITE_REG )))
		{
			ptrData_tx[1] = 0x83;
			ptrData_tx[2] = 0x01;

			uint16_t modbus_crc=0xFFFF;
			for(uint16_t i=0; i<3; i++)
				modbus_crc=_crc16_update(modbus_crc, ptrData_tx[i]);
			
			ptrData_tx[3]=modbus_crc;
			ptrData_tx[4]=modbus_crc>>8;
		}
	}


}

#endif
