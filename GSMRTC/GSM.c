

	#include "GSM.h"


uint8_t usart0_wait(char *pData)
{
	uint8_t len=0;

	while(pData[len]!=0) len++;
	

	uint16_t count_timeout=0;

	for(uint8_t i=0; i<len; i++)
	{
		while(usart0_rx_len())
		{
			if(count_timeout==USART0_TIMEOUT) return 2;
			count_timeout++;
		}
		if(pData[i]!=usart0_read()) return 1;		
	}



return 0;
}

