

	#include "ATCOMMANDS.h"

void at(void)
{
	const char at[]={'A', 'T', 0x0D, 0x0A};

	for(uint8_t n=0; n<sizeof(at); n++)
	{
		usart0_write(at[n]);	//AT.
		_delay_ms(USART_DELAY);
	}
}

int while_usart(char *pDataUsart)
{
	uint8_t Len=0;
	while(pDataUsart[Len]!=0x00) Len++;
	
	char temp[Len];

	for(uint8_t i=0; i<Len; i++)
	{
		while(!(usart0_rx_len()));
		
		temp[i]=usart0_read();

	}

	for(uint8_t i=0; i<Len; i++) 
		if(temp[i]!=pDataUsart[i]) return 1;

	return 0;

}

