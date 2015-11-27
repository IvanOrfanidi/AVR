

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


	char temp_data_time[29];
	while(usart0_rx_len()!=sizeof(temp_data_time));


	for(uint8_t i=0; i<sizeof(temp_data_time); i++) temp_data_time[i]=usart0_read();


	cyear[0]=temp_data_time[10];
	cyear[1]=temp_data_time[11];

	cmonth[0]=temp_data_time[13];
	cmonth[1]=temp_data_time[14];

	cday[0]=temp_data_time[16];
	cday[1]=temp_data_time[17];

	chours[0]=temp_data_time[19];
	chours[1]=temp_data_time[20];

	cmin[0]=temp_data_time[22];
	cmin[1]=temp_data_time[23];

	csec[0]=temp_data_time[25];
	csec[1]=temp_data_time[26];

	data_time[0]=' ';
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
	data_time[15]=' ';
	data_time[16]=' ';

	return 0;
}


uint8_t SetTimeSIM()
{

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();
		
	CCLK_SET;

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

	CCLK_SET_END;

	return 0;
}
