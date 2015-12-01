


#include "main.h"
#include "USART.h"

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
        #include <ctype.h>
        #include <stdint.h>

	uint8_t rx_status0;
	uint8_t tx_status0;

	uint8_t rx_rd_counter_index0;
	uint8_t rx_counter_index0;
	char rx_buffer0[RX_BUFFER_SIZE0];

	uint8_t tx_wr_counter_index0;
	uint8_t tx_counter_index0;
	char tx_buffer0[TX_BUFFER_SIZE0];


void usart0_write_str(char *pData_Usart0)
{
	while(usart0_busy_wait());

	uint16_t i=0;

	while(pData_Usart0[i]!=0)
	{
		usart0_write(pData_Usart0[i]);

		if(pData_Usart0[i]=='\n')
		{
			_delay_ms(USART0_DELAY_MS);
			usart0_write('\r');
		}

		i++;

		_delay_ms(USART0_DELAY_MS);
	}
	
}


// USART INITIALIZATION BAUD RATE//
void usart0_init()
{

	if(u2x0)
	{

			UBRRL = (uint16_t)((F_CPU/BAUD_RATE0/16-1)*2);			//BAUD RATE USART
			UBRRH = (uint16_t)((F_CPU/BAUD_RATE0/16-1)*2)>>8;

	}

	else
		{
			UBRRL = (uint16_t)(F_CPU/BAUD_RATE0/16-1);			//BAUD RATE USART
			UBRRH = (uint16_t)(F_CPU/BAUD_RATE0/16-1)>>8;
		}

	uint8_t status=0;

	#if RX_BUFFER_SIZE0

		UCSRB=(1<<RXEN)|(1<<RXCIE);
		
	#endif

	#if TX_BUFFER_SIZE0

		UCSRB|=(1<<TXEN);

	#endif

	status=(1<<URSEL);

	//SELECT CHARTER SIZE //
		if(CHARTER_SIZE0==9)
		{
			UCSRB|=(1<<UCSZ2);
			 status|=(1<<UCSZ1);
			  status|=(1<<UCSZ0);

		}

		if(CHARTER_SIZE0==8)
		{
			UCSRB&=~(1<<UCSZ2);
			 status|=(1<<UCSZ1);
			  status|=(1<<UCSZ0);

		}


		if(CHARTER_SIZE0==7)
		{
			UCSRB&=~(1<<UCSZ2);
			 status|=(1<<UCSZ1);
			  status&=~(1<<UCSZ0);

		}

		


//SELECT PARITY MODE //

	if((PARITY_MODE0=='O') | (PARITY_MODE0=='o'))
		{
			status|=	(1<<UPM0);
			status|= (1<<UPM1);

		}

	if((PARITY_MODE0=='E') | (PARITY_MODE0=='e'))
		{
			status&=	~(1<<UPM0);
			status|= (1<<UPM1);
			

		}
	
	if((PARITY_MODE0=='N') | (PARITY_MODE0=='n'))
		{
			status&=	~(1<<UPM0);
			status&= ~(1<<UPM1);

		}

	
//SELECT STOP BIT //

	if(STOP_BIT0==1)
	{
		status&=~(1<<USBS);

	}

	if(STOP_BIT0==2)
	{
		status|=(1<<USBS);

	}



	UCSRC=status;

}

////////////////////////////////////////////////////////////////////////////////////////

void usart0_write(uint8_t byte)
{

	if(tx_counter_index0==TX_BUFFER_SIZE0) tx_counter_index0=0;

	tx_buffer0[tx_counter_index0]=byte;

	tx_counter_index0++;


	UCSRB |= (1<<UDRIE);
}


#pragma vector=USART_UDRE_vect

__interrupt void USART_UDRE(void)
{

	if(tx_wr_counter_index0==TX_BUFFER_SIZE0) tx_wr_counter_index0=0;

	if(tx_wr_counter_index0==tx_counter_index0)
	{
		UCSRB &=~(1<<UDRIE);

	}
		else
		{
			UDR=tx_buffer0[tx_wr_counter_index0];
			tx_wr_counter_index0++;
		}
	
}

#pragma vector=USART_RXC_vect
__interrupt void USART_RXC(void)
{
	
	if(rx_counter_index0==RX_BUFFER_SIZE0) rx_counter_index0=0;

	rx_buffer0[rx_counter_index0]=UDR;
	rx_counter_index0++;

	rx_status0=0;
	
}

uint8_t usart0_read()
{

	if(rx_rd_counter_index0==RX_BUFFER_SIZE0) rx_rd_counter_index0=0;

	if(rx_rd_counter_index0==rx_counter_index0)
	{
		rx_status0=3;

		return 0;
	}
		else
		{

			uint8_t byte=rx_buffer0[rx_rd_counter_index0];
			rx_rd_counter_index0++;

			rx_status0=2;

			return byte;
		}

}


uint8_t usart0_rx_len()
{

		uint16_t len = rx_counter_index0 - rx_rd_counter_index0;
		return len;
}


uint8_t rx_error0()
{

rx_status0=1;

return 1;

}




void usart0_clear_tx_buffer()
{
	tx_wr_counter_index0=0;
	tx_counter_index0=0;

}


void usart0_clear_rx_buffer()
{
	rx_rd_counter_index0=0;
	rx_counter_index0=0;

}


uint8_t usart0_busy_wait(void)
{
	return (UCSRB & (1<<UDRIE));
}



