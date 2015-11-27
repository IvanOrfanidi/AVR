

#include "USART.h"

uint16_t rx_rd_counter_index0;
uint16_t rx_counter_index0;
char rx_buffer0[RX_BUFFER_SIZE0];

uint16_t tx_wr_counter_index0;
uint16_t tx_counter_index0;
char tx_buffer0[TX_BUFFER_SIZE0];


// USART INITIALIZATION //
void usart0_init()
{

	if(u2x0)
	{

			UBRRL = ((F_CPU/BAUD_RATE0/16-1)*2);			//BAUD RATE USART
			UBRRH = ((F_CPU/BAUD_RATE0/16-1)*2)>>8;

	}

	else
		{
			UBRRL = (F_CPU/BAUD_RATE0/16-1);			//BAUD RATE USART
			UBRRH = (F_CPU/BAUD_RATE0/16-1)>>8;
		}


	UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(0<<TXCIE)|(0<<UDRIE);

int status;

status=(1<<URSEL);

//SELECT CHARTER SIZE //
		if(CHARTER_SIZE0==9)
		{
			UCSRB|=_BV(UCSZ2);
			 status|=_BV(UCSZ1);
			  status|=_BV(UCSZ0);

		}

		if(CHARTER_SIZE0==8)
		{
			UCSRB&=~_BV(UCSZ2);
			 status|=_BV(UCSZ1);
			  status|=_BV(UCSZ0);

		}


		if(CHARTER_SIZE0==7)
		{
			UCSRB&=~_BV(UCSZ2);
			 status|=_BV(UCSZ1);
			  status&=~_BV(UCSZ0);

		}

		
		if(CHARTER_SIZE0==6)
		{
			UCSRB&=~_BV(UCSZ2);
			 status&=~_BV(UCSZ1);
			  status|=_BV(UCSZ0);

		}

		if(CHARTER_SIZE0==5)
		{
			UCSRB&=~_BV(UCSZ2);
			 status&=~_BV(UCSZ1);
			  status&=~_BV(UCSZ0);

		}


//SELECT PARITY MODE //

	if((PARITY_MODE0=='O') | (PARITY_MODE0=='o'))
		{
			status|=	_BV(UPM0);
			status|= _BV(UPM1);

		}

	if((PARITY_MODE0=='E') | (PARITY_MODE0=='e'))
		{
			status&=	~_BV(UPM0);
			status|= _BV(UPM1);
			

		}
	
	if((PARITY_MODE0=='N') | (PARITY_MODE0=='n'))
		{
			status&=	~_BV(UPM0);
			status&= ~_BV(UPM1);

		}

	
//SELECT STOP BIT //

	if(STOP_BIT0==1)
	{
		status&=~_BV(USBS);

	}

	if(STOP_BIT0==2)
	{
		status|=_BV(USBS);

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

ISR(USART_UDRE_vect)
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


ISR(USART_RXC_vect)
{
	
	if(!(UCSRA & ((1<<DOR)|(1<<FE)|(1<<UPE))))
	{
		if(rx_counter_index0==RX_BUFFER_SIZE0) rx_counter_index0=0;

		rx_buffer0[rx_counter_index0]=UDR;
		rx_counter_index0++;

		rx_status0=0;
	}

		else
		{
			rx_error0();
		}

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

