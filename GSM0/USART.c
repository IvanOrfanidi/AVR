

#include "USART.h"
#include <util/delay.h>

	uint8_t rx_rd_counter_index0;
	uint8_t rx_counter_index0;
	char rx_buffer0[RX_BUFFER_SIZE0];

	uint8_t tx_wr_counter_index0;
	uint8_t tx_counter_index0;
	char tx_buffer0[TX_BUFFER_SIZE0];

	#if defined (__AVR_ATmega162__)

		uint8_t rx_rd_counter_index1;
		uint8_t rx_counter_index1;
		char rx_buffer1[RX_BUFFER_SIZE1];

		uint8_t tx_wr_counter_index1;
		uint8_t tx_counter_index1;
		char tx_buffer1[TX_BUFFER_SIZE1];

	#endif


uint8_t status;


#if defined (__AVR_ATmega48__) || (__AVR_ATmega168__) || (__AVR_ATmega64__)	//If ATmega 48/64/168

// USART INITIALIZATION //
void usart0_init()
{

	if(u2x0)
	{

			UBRR0L = ((F_CPU/BAUD_RATE0/16-1)*2);			//BAUD RATE USART
			UBRR0H = ((F_CPU/BAUD_RATE0/16-1)*2)>>8;

	}

	else
		{
			UBRR0L = (F_CPU/BAUD_RATE0/16-1);			//BAUD RATE USART
			UBRR0H = (F_CPU/BAUD_RATE0/16-1)>>8;
		}


	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(0<<TXCIE0)|(0<<UDRIE0);


status=(0<<UMSEL01)|(0<<UMSEL00);


//SELECT CHARTER SIZE //
		if(CHARTER_SIZE0==9)
		{
			UCSR0B|=_BV(UCSZ02);
			 status|=_BV(UCSZ01);
			  status|=_BV(UCSZ00);

		}

		if(CHARTER_SIZE0==8)
		{
			UCSR0B&=~_BV(UCSZ02);
			 status|=_BV(UCSZ01);
			  status|=_BV(UCSZ00);

		}


		if(CHARTER_SIZE0==7)
		{
			UCSR0B&=~_BV(UCSZ02);
			 status|=_BV(UCSZ01);
			  status&=~_BV(UCSZ00);

		}

		
		if(CHARTER_SIZE0==6)
		{
			UCSR0B&=~_BV(UCSZ02);
			 status&=~_BV(UCSZ01);
			  status|=_BV(UCSZ00);

		}

		if(CHARTER_SIZE0==5)
		{
			UCSR0B&=~_BV(UCSZ02);
			 status&=~_BV(UCSZ01);
			  status&=~_BV(UCSZ00);

		}


//SELECT PARITY MODE //

	if((PARITY_MODE0=='O') | (PARITY_MODE0=='o'))
		{
			status|=	_BV(UPM00);
			status|= _BV(UPM01);

		}

	if((PARITY_MODE0=='E') | (PARITY_MODE0=='e'))
		{
			status&=	~_BV(UPM00);
			status|= _BV(UPM01);
			

		}
	
	if((PARITY_MODE0=='N') | (PARITY_MODE0=='n'))
		{
			status&=	~_BV(UPM00);
			status&= ~_BV(UPM01);

		}

	
//SELECT STOP BIT //

	if(STOP_BIT0==1)
	{
		status&=~_BV(USBS0);

	}

	if(STOP_BIT0==2)
	{
		status|=_BV(USBS0);

	}


	UCSR0C=status;



}

////////////////////////////////////////////////////////////////////////////////////////


void usart0_write(uint8_t byte)
{

	if(tx_counter_index0==TX_BUFFER_SIZE0) tx_counter_index0=0;

	tx_buffer0[tx_counter_index0]=byte;

	tx_counter_index0++;

	UCSR0B |= (1<<UDRIE0);
}

ISR(USART_UDRE_vect)
{

	if(tx_wr_counter_index0==TX_BUFFER_SIZE0) tx_wr_counter_index0=0;

	if(tx_wr_counter_index0==tx_counter_index0)
	{
		UCSR0B &=~(1<<UDRIE0);
	}
		else
		{
			UDR0=tx_buffer0[tx_wr_counter_index0];
			tx_wr_counter_index0++;
		}	
}


ISR(USART_RXC_vect)
{
	
	if(!(UCSR0A & ((1<<DOR0)|(1<<FE0)|(1<<UPE0))))
	{
		if(rx_counter_index0==RX_BUFFER_SIZE0) rx_counter_index0=0;

		rx_buffer0[rx_counter_index0]=UDR0;
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

uint8_t len = rx_counter_index0 - rx_rd_counter_index0;
	
	return len;

}


uint8_t rx_error0()
{

rx_status0=1;

return 1;

}

//////////////////////////////////////////////////////////////////////////////////




void usart0_clear_rx_buffer()
{
	rx_rd_counter_index0=0;
	rx_counter_index0=0;
}

#endif



#if defined (__AVR_ATmega8__) || (__AVR_ATmega16__) || (__AVR_ATmega32__)

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
	
	if(!(UCSRA & ((1<<DOR0)|(1<<FE0)|(1<<UPE0))))
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



#endif


#if defined (__AVR_ATmega162__)


// USART INITIALIZATION //
void usart0_init()
{

	if(u2x0)
	{

			UBRR0L = ((F_CPU/BAUD_RATE0/16-1)*2);			//BAUD RATE USART
			UBRR0H = ((F_CPU/BAUD_RATE0/16-1)*2)>>8;

	}

	else
		{
			UBRR0L = (F_CPU/BAUD_RATE0/16-1);			//BAUD RATE USART
			UBRR0H = (F_CPU/BAUD_RATE0/16-1)>>8;
		}


	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(0<<TXCIE0)|(0<<UDRIE0);


status=(1<<URSEL0);


//SELECT CHARTER SIZE //
		if(CHARTER_SIZE0==9)
		{
			UCSR0B|=_BV(UCSZ02);
			 status|=_BV(UCSZ01);
			  status|=_BV(UCSZ00);

		}

		if(CHARTER_SIZE0==8)
		{
			UCSR0B&=~_BV(UCSZ02);
			 status|=_BV(UCSZ01);
			  status|=_BV(UCSZ00);

		}


		if(CHARTER_SIZE0==7)
		{
			UCSR0B&=~_BV(UCSZ02);
			 status|=_BV(UCSZ01);
			  status&=~_BV(UCSZ00);

		}

		
		if(CHARTER_SIZE0==6)
		{
			UCSR0B&=~_BV(UCSZ02);
			 status&=~_BV(UCSZ01);
			  status|=_BV(UCSZ00);

		}

		if(CHARTER_SIZE0==5)
		{
			UCSR0B&=~_BV(UCSZ02);
			 status&=~_BV(UCSZ01);
			  status&=~_BV(UCSZ00);

		}


//SELECT PARITY MODE //

	if((PARITY_MODE0=='O') | (PARITY_MODE0=='o'))
		{
			status|=	_BV(UPM00);
			status|= _BV(UPM01);

		}

	if((PARITY_MODE0=='E') | (PARITY_MODE0=='e'))
		{
			status&=	~_BV(UPM00);
			status|= _BV(UPM01);
			

		}
	
	if((PARITY_MODE0=='N') | (PARITY_MODE0=='n'))
		{
			status&=	~_BV(UPM00);
			status&= ~_BV(UPM01);

		}

	
//SELECT STOP BIT //

	if(STOP_BIT0==1)
	{
		status&=~_BV(USBS0);

	}

	if(STOP_BIT0==2)
	{
		status|=_BV(USBS0);

	}


		UCSR0C=((1<<URSEL0) | (1<<UCSZ01) | (1<<UCSZ00));


}

////////////////////////////////////////////////////////////////////////////////////////

void usart1_init()
{

	if(u2x1)
	{

			UBRR1L = ((F_CPU/BAUD_RATE1/16-1)*2);			//BAUD RATE USART
			UBRR1H = ((F_CPU/BAUD_RATE1/16-1)*2)>>8;

	}

	else
		{
			UBRR1L = (F_CPU/BAUD_RATE1/16-1);			//BAUD RATE USART
			UBRR1H = (F_CPU/BAUD_RATE1/16-1)>>8;
		}


	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1)|(0<<TXCIE1)|(0<<UDRIE1);


status=(1<<URSEL1);


//SELECT CHARTER SIZE //
		if(CHARTER_SIZE1==9)
		{
			UCSR1B|=_BV(UCSZ12);
			 status|=_BV(UCSZ11);
			  status|=_BV(UCSZ10);

		}

		if(CHARTER_SIZE1==8)
		{
			UCSR1B&=~_BV(UCSZ12);
			 status|=_BV(UCSZ11);
			  status|=_BV(UCSZ10);

		}


		if(CHARTER_SIZE1==7)
		{
			UCSR1B&=~_BV(UCSZ12);
			 status|=_BV(UCSZ11);
			  status&=~_BV(UCSZ10);

		}

		
		if(CHARTER_SIZE1==6)
		{
			UCSR1B&=~_BV(UCSZ12);
			 status&=~_BV(UCSZ11);
			  status|=_BV(UCSZ10);

		}

		if(CHARTER_SIZE0==5)
		{
			UCSR1B&=~_BV(UCSZ12);
			 status&=~_BV(UCSZ11);
			  status&=~_BV(UCSZ10);

		}


//SELECT PARITY MODE //

	if((PARITY_MODE1=='O') | (PARITY_MODE1=='o'))
		{
			status|=	_BV(UPM10);
			status|= _BV(UPM11);

		}

	if((PARITY_MODE1=='E') | (PARITY_MODE1=='e'))
		{
			status&=	~_BV(UPM10);
			status|= _BV(UPM11);
			

		}
	
	if((PARITY_MODE1=='N') | (PARITY_MODE1=='n'))
		{
			status&=	~_BV(UPM10);
			status&= ~_BV(UPM11);

		}

	
//SELECT STOP BIT //

	if(STOP_BIT1==1)
	{
		status&=~_BV(USBS1);

	}

	if(STOP_BIT1==2)
	{
		status|=_BV(USBS1);

	}


		UCSR1C=status;


}

////////////////////////////////////////////////////////////////////////////////////

void usart0_write(uint8_t byte)
{

	if(tx_counter_index0==TX_BUFFER_SIZE0) tx_counter_index0=0;

	tx_buffer0[tx_counter_index0]=byte;

	tx_counter_index0++;

	UCSR0B |= (1<<UDRIE0);
}

ISR(USART0_UDRE_vect)
{

	if(tx_wr_counter_index0==TX_BUFFER_SIZE0) tx_wr_counter_index0=0;

	if(tx_wr_counter_index0==tx_counter_index0)
	{
		UCSR0B &=~(1<<UDRIE0);
	}
		else
		{
			UDR0=tx_buffer0[tx_wr_counter_index0];
			tx_wr_counter_index0++;
		}
	
}


ISR(USART0_RXC_vect)
{
	
	if(!(UCSR0A & ((1<<DOR0)|(1<<FE0)|(1<<UPE0))))
	{
		if(rx_counter_index0==RX_BUFFER_SIZE0) rx_counter_index0=0;

		rx_buffer0[rx_counter_index0]=UDR0;
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

uint8_t len = rx_counter_index0 - rx_rd_counter_index0;
	
	return len;

}


uint8_t rx_error0()
{

rx_status0=1;

return 1;

}

//////////////////////////////////////////////////////////////////////////////////

void usart1_write(uint8_t byte)
{

	if(tx_counter_index1==TX_BUFFER_SIZE1) tx_counter_index1=0;

	tx_buffer1[tx_counter_index1]=byte;

	tx_counter_index1++;

	UCSR1B |= (1<<UDRIE1);
}

ISR(USART1_UDRE_vect)
{

	if(tx_wr_counter_index1==TX_BUFFER_SIZE1) tx_wr_counter_index1=0;

	if(tx_wr_counter_index1==tx_counter_index1)
	{
		UCSR1B &=~(1<<UDRIE1);
	}
		else
		{
			UDR1=tx_buffer1[tx_wr_counter_index1];
			tx_wr_counter_index1++;
		}
	
}


ISR(USART1_RXC_vect)
{
	
	if(!(UCSR1A & ((1<<DOR0)|(1<<FE0)|(1<<UPE0))))
	{
		if(rx_counter_index1==RX_BUFFER_SIZE1) rx_counter_index1=0;

		rx_buffer1[rx_counter_index1]=UDR1;
		rx_counter_index1++;

		rx_status1=0;
	}

		else
		{
			rx_error1();
		}

}

uint8_t usart1_read()
{

	if(rx_rd_counter_index1==RX_BUFFER_SIZE1) rx_rd_counter_index1=0;

	if(rx_rd_counter_index1==rx_counter_index1)
	{
		rx_status1=3;

		return 0;
	}
		else
		{

			uint8_t byte=rx_buffer1[rx_rd_counter_index1];
			rx_rd_counter_index1++;

			rx_status1=2;

			return byte;
		}

}


uint8_t usart1_rx_len()
{

uint8_t len = rx_counter_index1 - rx_rd_counter_index1;
	
	return len;

}


uint8_t rx_error1()
{

rx_status1=1;

return 1;

}



void usart0_clear_tx_buffer()
{
	tx_counter_index0=0;
	tx_wr_counter_index0=0;

}

void usart1_clear_tx_buffer()
{
	tx_wr_counter_index1=0;
	tx_counter_index1=0;

}


void usart0_clear_rx_buffer()
{
	rx_rd_counter_index0=0;
	rx_counter_index0=0;

}

void usart1_clear_rx_buffer()
{
	rx_rd_counter_index1=0;
	rx_counter_index1=0;

}


#endif
