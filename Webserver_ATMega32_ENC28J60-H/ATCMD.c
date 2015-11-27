

	#include "ATCMD.h"
	#include "USART.h"

	#include <avr/eeprom.h>

	#define true      1
	#define false     0
	#define bool      _Bool

	char ok[]={'O', 'K', 0x0D, 0x0A};
	char err[]={'E', 'R', 'R', 'O', 'R', 0x0D, 0x0A};

	char at[]={'A','T'};
	char at_ver[]={'A','T','+','V','E','R','?'};
	char at_rst[]={'A','T','+','R','S','T'};
	char at_ip_get[]={'A','T','+','I','P','?'};
	char at_buz1[]={'A','T','+','B','U','Z','=','1'};
	char at_buz0[]={'A','T','+','B','U','Z','=','0'};
	char at_rel1[]={'A','T','+','R','E','L','=','1'};
	char at_rel0[]={'A','T','+','R','E','L','=','0'};
	char at_buz[]={'A','T','+','B','U','Z','?'};
	char at_rel[]={'A','T','+','R','E','L','?'};
	char at_mac[]={'A','T','+','M','A','C','?'};
	char at_led[]={'A','T','+','L','E','D','?'};
	char at_led1[]={'A','T','+','L','E','D','=','1'};
	char at_led0[]={'A','T','+','L','E','D','=','0'};

	char at_ip_set[]={'A','T','+','I','P','=','X','X','X','.','X','X','X','.','X','X','X','.','X','X','X'};


	uint8_t index_temp_buf=0;
	char temp_buf[BUF_SIZE_AT];

static void (*jump_to_app)(void) = 0x0000;


void ATrst(void)
{
	for(uint8_t i=0; i<sizeof(ok); i++) usart0_write(ok[i]);
	_delay_ms(50);
	jump_to_app();
}

void ATGetIP(void)
{
//	usart0_write(0x0D); 	usart0_write(0x0A);
	for(uint8_t i=2; i<(sizeof(at_ip_get)-1); i++) usart0_write(at_ip_get[i]);
	usart0_write(':'); 	usart0_write(' ');


	for(uint16_t i=0; i<12; i++)
	{
		if((i==3) || (i==6) || (i==9))
		{
			usart0_write('.');
		}
		
		usart0_write(eeprom_read_byte(i+ADDR_MYIP_EEP));
	}


	usart0_write(0x0D); 	usart0_write(0x0A);

}

void ATSetIP(void)
{
	bool err_ip=false;

	for(uint8_t i=6; i<sizeof(at_ip_set); i++)
	{
		
		if((i==9) || (i==13) || (i==17))
		{
			if(temp_buf[i]!='.')	err_ip=true;
		}
		else
		{
			if((temp_buf[i]<'0') || (temp_buf[i]>'9')) err_ip=true;
		}


	}

	//Проверяем коректность введенного IP.
	if(err_ip)	
	{
		//Если IP не коректен, то выводим сообщение и выходим.
		for(uint8_t i=0; i<sizeof(err); i++)	
			usart0_write(err[i]);		
	}
	else
	{
		//Если IP  коректен, то выводим сообщение и записуем его в EEPROM.

		uint16_t n=ADDR_MYIP_EEP;

		//Write New My IP.
		for(uint8_t i=6; i<sizeof(at_ip_set); i++)
		{
			if((i==9) || (i==13) || (i==17))
			{
			}
			else
			{
				eeprom_update_byte(n, temp_buf[i]);
				n++;
			}

		}


		for(uint8_t i=0; i<sizeof(ok); i++) 
			usart0_write(ok[i]);
	}
		
}

uint8_t atcommand(void)
{
	uint8_t Len = usart0_rx_len();

	if(!(Len))
		return NOT;

	uint8_t rx_byte=usart0_read();

	usart0_write(rx_byte);

	if(rx_byte==BACKSPASE)
	{
		if(index_temp_buf) index_temp_buf--;
		return DEC;

	}
	else
	{
		temp_buf[index_temp_buf]=rx_byte;
		index_temp_buf++;
	}
	
	if(rx_byte==ENTER)
	{

		if(index_temp_buf<=1)
		{
			index_temp_buf=0;
			return NOT;
		}
		usart0_clear_rx_buffer();
		usart0_clear_tx_buffer();
	
		bool arr_at_ok[15];

		for(uint8_t i=0; i<sizeof(arr_at_ok); i++)
			arr_at_ok[i]=true;

		for(uint8_t i=0; i<(index_temp_buf-1); i++)
		{
			if((temp_buf[i]!=at[i]) || (sizeof(at)!=(index_temp_buf-1)))
				arr_at_ok[0]=false;	

			if((temp_buf[i]!=at_ver[i]) || (sizeof(at_ver)!=(index_temp_buf-1)))
				arr_at_ok[1]=false;	

			if((temp_buf[i]!=at_rst[i]) || (sizeof(at_rst)!=(index_temp_buf-1)))
				arr_at_ok[2]=false;	

			if((temp_buf[i]!=at_ip_get[i]) || (sizeof(at_ip_get)!=(index_temp_buf-1)))
				arr_at_ok[3]=false;
				
			if((temp_buf[i]!=at_buz1[i]) || (sizeof(at_buz1)!=(index_temp_buf-1)))
				arr_at_ok[4]=false;
				
			if((temp_buf[i]!=at_buz0[i]) || (sizeof(at_buz0)!=(index_temp_buf-1)))
				arr_at_ok[5]=false;	
				
			if((temp_buf[i]!=at_rel1[i]) || (sizeof(at_rel1)!=(index_temp_buf-1)))
				arr_at_ok[6]=false;
				
			if((temp_buf[i]!=at_rel0[i]) || (sizeof(at_rel0)!=(index_temp_buf-1)))
				arr_at_ok[7]=false;
				
			if((temp_buf[i]!=at_buz[i]) || (sizeof(at_buz)!=(index_temp_buf-1)))
				arr_at_ok[8]=false;
				
			if((temp_buf[i]!=at_rel[i]) || (sizeof(at_rel)!=(index_temp_buf-1)))
				arr_at_ok[9]=false;


			if(i<6)
			{	
				if((temp_buf[i]!=at_ip_set[i]) || (sizeof(at_ip_set)!=(index_temp_buf-1)))
					arr_at_ok[10]=false;
			}

			if((temp_buf[i]!=at_mac[i]) || (sizeof(at_mac)!=(index_temp_buf-1)))
				arr_at_ok[11]=false;

			if((temp_buf[i]!=at_led[i]) || (sizeof(at_led)!=(index_temp_buf-1)))
				arr_at_ok[12]=false;

			if((temp_buf[i]!=at_led1[i]) || (sizeof(at_led1)!=(index_temp_buf-1)))
				arr_at_ok[13]=false;

			if((temp_buf[i]!=at_led0[i]) || (sizeof(at_led0)!=(index_temp_buf-1)))
				arr_at_ok[14]=false;
		}

		uint8_t req=0xFF;

		for(uint8_t i=0; i<sizeof(arr_at_ok); i++)
			if(arr_at_ok[i])
				req=i;

		index_temp_buf=0;


		usart0_write(0x0D); 	usart0_write(0x0A);

		switch (req)
		{
			case 0:
				for(uint8_t i=0; i<sizeof(ok); i++) usart0_write(ok[i]);
			return 255;

			case 1:
				for(uint8_t i=2; i<(sizeof(at_ver)-1); i++) usart0_write(at_ver[i]);
			return 254;

			case 2:
				ATrst();
			return 253;

			case 3:
				ATGetIP();
			return 252;

			case 4:
				for(uint8_t i=0; i<sizeof(ok); i++) usart0_write(ok[i]);
			return 251;

			case 5:
				for(uint8_t i=0; i<sizeof(ok); i++) usart0_write(ok[i]);
			return 250;

			case 6:
				for(uint8_t i=0; i<sizeof(ok); i++) usart0_write(ok[i]);
			return 249;

			case 7:
				for(uint8_t i=0; i<sizeof(ok); i++) usart0_write(ok[i]);
			return 248;

			case 8:
				for(uint8_t i=2; i<(sizeof(at_buz)-1); i++) usart0_write(at_buz[i]);
			return 247;

			case 9:
				for(uint8_t i=2; i<(sizeof(at_rel)-1); i++) usart0_write(at_rel[i]);
			return 246;

			case 10:
				ATSetIP();
			return 245;

			case 11:
				for(uint8_t i=2; i<(sizeof(at_mac)-1); i++) usart0_write(at_mac[i]);
			return 244;

			case 12:
				for(uint8_t i=2; i<(sizeof(at_led)-1); i++) usart0_write(at_led[i]);
			return 243;

			case 13:
				for(uint8_t i=0; i<sizeof(ok); i++) usart0_write(ok[i]);
			return 242;

			case 14:
				for(uint8_t i=0; i<sizeof(ok); i++) usart0_write(ok[i]);
			return 241;

			case 255:
				for(uint8_t i=0; i<sizeof(err); i++) usart0_write(err[i]);
			return ERROR;
		}

	
	}


	return OK;


}

