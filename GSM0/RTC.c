

	#include "RTC.h"
	#include "USART.h"
	#include <util/delay.h>

	
	const char at_get_time[]={'A','T','+','C','C','L','K','?', 0x0D, 0x0A};
	const char at_set_time[]={'A','T','+','C','C','L','K','=', '"'};
	const char at_set_time_end[]={'+','0','0','"', 0x0D, 0x0A};


uint8_t GetTimeSIM()
{
	
	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	
	CCLK_GET;


	char temp_data_time[17];
	uint8_t byte;

	while(1)
	{
		while(!usart0_rx_len());
		byte=usart0_read();
		if(byte=='"') break;
	}



	for(uint8_t i=0; i<sizeof(temp_data_time); i++)
	{
		while(!usart0_rx_len());
		temp_data_time[i]=usart0_read();
	}


	cyear[0]=temp_data_time[0];
	cyear[1]=temp_data_time[1];

	cmonth[0]=temp_data_time[3];
	cmonth[1]=temp_data_time[4];

	cday[0]=temp_data_time[6];
	cday[1]=temp_data_time[7];

	chours[0]=temp_data_time[9];
	chours[1]=temp_data_time[10];

	cmin[0]=temp_data_time[12];
	cmin[1]=temp_data_time[13];

	csec[0]=temp_data_time[15];
	csec[1]=temp_data_time[16];

	data_time[0]=0x20;
	data_time[1]=cyear[0];
	data_time[2]=cyear[1];
	data_time[3]='/';
	data_time[4]=cmonth[0];
	data_time[5]=cmonth[1];
	data_time[6]='/';
	data_time[7]=cday[0];
	data_time[8]=cday[1];
	data_time[9]=' ';
	data_time[10]=chours[0];
	data_time[11]=chours[1];
	data_time[12]=':';
	data_time[13]=cmin[0];
	data_time[14]=cmin[1];
	data_time[15]=0x20;
	data_time[16]=0x20;

	return 0;
}


uint8_t SetTimeSIM()
{

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();
		
	for(uint8_t n=0; n<sizeof(at_set_time); n++)
	{
		usart0_write(at_set_time[n]);
		_delay_ms(10);
	}

	usart0_write(cyear[0]);
	usart0_write(cyear[1]);

	usart0_write('/');

	usart0_write(cmonth[0]);
	usart0_write(cmonth[1]);

	usart0_write('/');
	
	usart0_write(cday[0]);
	usart0_write(cday[1]);

	usart0_write(',');

	usart0_write(chours[0]);
	usart0_write(chours[1]);

	usart0_write(':');

	usart0_write(cmin[0]);
	usart0_write(cmin[1]);

	usart0_write(':');

	usart0_write(csec[0]);
	usart0_write(csec[1]);

	for(uint8_t n=0; n<sizeof(at_set_time_end); n++)
	{
		usart0_write(at_set_time_end[n]);
		_delay_ms(10);
	}
	
	uint8_t byte;
	while(1)
	{
		while(!(usart0_rx_len()));	

		byte=usart0_read();
		if(byte=='0') return 0;
		if(byte=='4') return 4;
			
	}
	_delay_ms(10);

	return 0;
}
