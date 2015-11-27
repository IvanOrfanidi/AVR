

//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>

	#include "GPS.h"
	
	char gps_data;
	uint8_t num;
	uint8_t len;

	int rx_count;

	uint16_t timeout;

	int gga_true;

	const char GPGGA[6]={'G','P','G','G','A',','};


void gps_init()
{


// USART initialization.

	// Communication Parameters: 8 Data, 1 Stop, No Parity.
	// USART Receiver: On.
	// USART Transmitter: On.
	// USART Interrupt RX: On.
	// USART Interrupt TX: Off.
	// USART Interrupt UD: Off.
	// USART Mode: Asynchronous.
	// USART Baud rate: 19200

	UBRR0L = (F_CPU/19200/16-1);			//BAUD RATE USART
	UBRR0H = (F_CPU/19200/16-1)>>8;

	UCSR0B = (1<<RXEN0)|(0<<TXEN0)|(0<<RXCIE0)|(0<<TXCIE0)|(0<<UDRIE0);

	UCSR0C=(1<<URSEL0)|(1<<UCSZ01)|(1<<UCSZ00);	//8-N-1.

}

///////////////////////////////////////////////////////////////////

int gps_stat()
{

	while(gps_data!='$')
	{

	timeout=0;
		while(!(UCSR0A & (1<<RXC0)))
		{
			timeout++;
			if(timeout==TIMEOUT_REQ) return 2;
		}

	gps_data=UDR0;

	}

	timeout=0;
	for(uint8_t i=0; i!=25; i++)
	{
		while(!(UCSR0A & (1<<RXC0)))
		{
				timeout++;
				if(timeout==TIMEOUT_REQ) return 2;

		}
			gps_data=UDR0;


		if((i>16))
		{
			if(gps_data=='A')
			{

				rx_count=0;



				return 0;
			}

			if(gps_data=='V')
			{

				return 1;
			}

		}
	}


return 2;
}


int gps_status()
{

	while(gps_data!='$')
	{

	timeout=0;
		while(!(UCSR0A & (1<<RXC0)))
		{
			timeout++;
			if(timeout==TIMEOUT_REQ) return 2;
		}

	gps_data=UDR0;

	}

	for(uint8_t i=0; i<(sizeof(GPGGA)); i++)
	{
			timeout=0;
			while(!(UCSR0A & (1<<RXC0)))
			{
				timeout++;
				if(timeout==TIMEOUT_REQ) return 2;
			}

		gps_data=UDR0;
	
		int gga_true=1;
		if(gps_data!=GPGGA[i]) gga_true=0;


		for(uint8_t n=0; n<5; n++)
		{
			timeout=0;
			while(!(UCSR0A & (1<<RXC0)))
			{
				timeout++;
				if(timeout==TIMEOUT_REQ) return 2;
			}

			gps_data=UDR0;

		}

//////////////////////////////////////////////////////////////////////////////////////

		// GPS TIME //

		if(gga_true)
		{

			for(uint8_t n=0; n<6; n++)
			{
			
				timeout=0;
				while(!(UCSR0A & (1<<RXC0)))
				{
					timeout++;
					if(timeout==TIMEOUT_REQ) return 2;
				}

				gps_time[n]=UDR0;

				uint8_t gps_time_int[sizeof(gps_time)];

				for(i=0; i<sizeof(gps_time); i++) gps_time_int[i]=(gps_time[i]-0x30);
				
	
				time[0]=(gps_time_int[0]*10)+(gps_time_int[1]);
				time[1]=(gps_time_int[2]*10)+(gps_time_int[3]);
				time[2]=(gps_time_int[4]*10)+(gps_time_int[5]);


		 }

//////////////////////////////////////////////////////////////////////////////////////

	// GPS LATITUDE //

		for(uint8_t n=0; n<5; n++)
		{
				timeout=0;
				while(!(UCSR0A & (1<<RXC0)))
				{
					timeout++;
					if(timeout==TIMEOUT_REQ) return 2;
				}

		gps_data=UDR0;

		}


	if(gga_true)
	{

		for(uint8_t n=0; n<sizeof(gps_latitude); n++)
		{
			
			timeout=0;
			while(!(UCSR0A & (1<<RXC0)))
			{
				timeout++;
				if(timeout==TIMEOUT_REQ) return 2;
			}

			gps_latitude[n]=UDR0;

		}
	}


//////////////////////////////////////////////////////////////////////////////////////

	// GPS LONGITUDE //

		for(uint8_t n=0; n<2; n++)
		{
			timeout=0;
			while(!(UCSR0A & (1<<RXC0)))
				{
					timeout++;
					if(timeout==TIMEOUT_REQ) return 2;
				}

		gps_data=UDR0;

		}


		if(gga_true)
		{

			for(uint8_t n=0; n<sizeof(gps_longitude); n++)
			{
			
				timeout=0;
				while(!(UCSR0A & (1<<RXC0)))
				{
					timeout++;
					if(timeout==TIMEOUT_REQ) return 2;
				}

				gps_longitude[n]=UDR0;

			}



		return 0;

		}

		else

		{
			return 2;
		}
	


	}



return 2;

}

return 2;

}
