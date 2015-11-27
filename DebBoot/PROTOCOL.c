

	#include "PROTOCOL.h"

	uint8_t device_id;

#if defined (__AVR_ATmega88__) || (__AVR_ATmega88P__) || (__AVR_ATmega168__) || (__AVR_ATmega168P__)	//If ATmega 48/64/168

	#define USART_DATA	UDR0

uint8_t usart_wait_rx(void)
{
	if(UCSR0A & (1<<RXC0))

		return 0;

		else

			return 1;
}

#endif

#if defined (__AVR_ATmega8__) || (__AVR_ATmega16__) || (__AVR_ATmega32__)

	#define USART_DATA	UDR

#endif

#if defined (__AVR_ATmega162__) || (__AVR_ATmega128__)
	
	#if(SELECT_USART)

		#define USART_DATA	UDR0

	#endif

	#if(!(SELECT_USART))

		#define USART_DATA	UDR1

	#endif


#endif




void protocol_init_usart(uint8_t boot_id)
{
	device_id=boot_id;

#if defined (__AVR_ATmega88__) || (__AVR_ATmega88P__) || (__AVR_ATmega168__) || (__AVR_ATmega168P__)	//If ATmega 48/64/168

	UBRR0L = (F_CPU/BAUD_RATE/16-1);			//
	UBRR0H = (F_CPU/BAUD_RATE/16-1)>>8;
	
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);

	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);

#endif

#if defined (__AVR_ATmega8__) || (__AVR_ATmega16__) || (__AVR_ATmega32__)

	UBRRL = (F_CPU/BAUD_RATE/16-1);			
	UBRRH = (F_CPU/BAUD_RATE/16-1)>>8;
	
	UCSRB = (1<<RXEN) | (1<<TXEN);

	UCSRC = (1<<UCSZ1) | (1<<UCSZ0);

#endif

#if defined (__AVR_ATmega162__)
	
	#if(SELECT_USART)

		UBRR1L = (F_CPU/BAUD_RATE/16-1);			
		UBRR1H = (F_CPU/BAUD_RATE/16-1)>>8;
	
		UCSR1B = (1<<RXEN1) | (1<<TXEN1);

		UCSR1C = (1<<UCSZ11) | (1<<UCSZ10) | (1<<URSEL1);


	#endif

	#if(!(SELECT_USART))

		UBRR0L = (F_CPU/BAUD_RATE/16-1);			
		UBRR0H = (F_CPU/BAUD_RATE/16-1)>>8;
	
		UCSR0B = (1<<RXEN0) | (1<<TXEN0);

		UCSR0C = (1<<UCSZ01) | (1<<UCSZ00) | (1<<URSEL0);


	#endif


#endif

#if defined (__AVR_ATmega128__)

	#if(SELECT_USART)

		UBRR1L = (F_CPU/BAUD_RATE/16-1);			
		UBRR1H = (F_CPU/BAUD_RATE/16-1)>>8;
	
		UCSR1B = (1<<RXEN1) | (1<<TXEN1);

		UCSR1C = (1<<UCSZ11) | (1<<UCSZ10);


	#endif

	#if(!(SELECT_USART))

		UBRR0L = (F_CPU/BAUD_RATE/16-1);			
		UBRR0H = (F_CPU/BAUD_RATE/16-1)>>8;
	
		UCSR0B = (1<<RXEN0) | (1<<TXEN0);

		UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);


	#endif


#endif

}
