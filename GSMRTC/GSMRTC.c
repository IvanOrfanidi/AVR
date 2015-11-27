
	#include "GSMRTC.h"
	#include "USART.h"

	uint8_t error_num_usart_rtc;
	const char at_get_time[]="AT+CCLK?\n";
	const char at_set_time[]="AT+CCLK=\"";
	const char at_set_time_end[]="+00\"\n";
	

uint8_t GetTimeGSM(char *pTime)
{
	char temp_time[]="12345678901234567";

	GetDateTimeGSM(temp_time);

	for(uint8_t i=0; i<8; i++)
		pTime[i]=temp_time[i+9];

return 0;	
}


uint8_t SetTimeGSM(const char *pTime)
{
	char back_time[]="12345678901234567";

	GetDateTimeGSM(back_time);

	for(uint8_t i=10; i<17; i++)
		 back_time[i]=pTime[i-9];
	
	SetDateTimeGSM(back_time);

return 0;	
}


#if (USART_NUM_RTC && !((__AVR_ATmega162__) || (__AVR_ATmega128__)))

# error "Do not USART1! (USART_NUM_RTC=0)"
#else

#if USART_NUM_RTC

uint8_t GetDateTimeGSM(char *pDateTime)
{
		
	usart1_clear_tx_buffer();	//Clear Tx and Rx buffers.
	usart1_clear_rx_buffer();
	
	usart1_write_str(at_get_time);

	
	while(usart1_read()!=0x22)
	{
		usart1_clear_rx_buffer();	//Clear Rx buffer.
		while(!(usart1_rx_len()));

	}

	while(usart1_rx_len()!=17);

	for(uint8_t i=0; i<17; i++)
		pDateTime[i]=usart1_read();

return 0;
}

uint8_t SetDateTimeGSM(const char *pDateTime)
{
	usart1_clear_tx_buffer();	//Clear Tx and Rx buffers.
	usart1_clear_rx_buffer();

	usart1_write_str(at_set_time);

	usart1_write_str(pDateTime);

	usart1_write_str(at_set_time_end);

return 0;	
}

#else

uint8_t GetDateTimeGSM(char *pDateTime)
{
		
	usart0_clear_tx_buffer();	//Clear Tx and Rx buffers.
	usart0_clear_rx_buffer();
	
	usart0_write_str(at_get_time);

	
	while(usart0_read()!=0x22)
	{
		usart0_clear_rx_buffer();	//Clear Rx buffer.
		while(!(usart0_rx_len()));

	}

	while(usart0_rx_len()!=17);

	for(uint8_t i=0; i<17; i++)
		pDateTime[i]=usart0_read();

return 0;
}

uint8_t SetDateTimeGSM(const char *pDateTime)
{
	usart0_clear_tx_buffer();	//Clear Tx and Rx buffers.
	usart0_clear_rx_buffer();

	usart0_write_str(at_set_time);

	usart0_write_str(pDateTime);

	usart0_write_str(at_set_time_end);

return 0;	
}

#endif
#endif
