

	#include "RTC.h"
	#include "USART.h"

	const char at_get_time[]={'A','T','+','C','C','L','K','?', 0x0D, 0x0A};
	const char at_set_time[]={'A','T','+','C','C','L','K','=', '"'};
	const char at_set_time_end[]={'+','0','0','"', 0x0D, 0x0A};


uint8_t GetTimeSIM()
{
	
	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	
	CCLK_GET;

	while(usart0_read()!=0x22)
	{
		usart0_clear_rx_buffer();
		while(!(usart0_rx_len()));

	}

	while(usart0_rx_len()!=sizeof(data_time));

	for(uint8_t t0; t0<sizeof(data_time); t0++) data_time[t0]=usart0_read();

	cyear[0]=data_time[0];
	cyear[1]=data_time[1];

	cmonth[0]=data_time[3];
	cmonth[1]=data_time[4];

	cday[0]=data_time[6];
	cday[1]=data_time[7];

	chours[0]=data_time[9];
	chours[1]=data_time[10];

	cmin[0]=data_time[12];
	cmin[1]=data_time[13];
	
	return 0;
}


uint8_t SetTimeSIM()
{

	cmonth[1]= '1';
	cday[1]= '1';
	cyear[1]='2';
	cmonth[0]= '1';
	cday[0]= '1';
	cyear[0]='1';

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
