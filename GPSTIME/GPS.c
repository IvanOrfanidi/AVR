

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

	UBRRL = (F_CPU/19200/16-1);			//BAUD RATE USART
	UBRRH = (F_CPU/19200/16-1)>>8;

	UCSRB = (1<<RXEN)|(0<<TXEN)|(0<<RXCIE)|(0<<TXCIE)|(0<<UDRIE);

	UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);	//8-N-1.

}


int gps_stat()
{

	while(gps_data!='$')
	{

	timeout=0;
		while(!(UCSRA & (1<<RXC)))
		{
			timeout++;
			if(timeout==TIMEOUT_REQ) return 2;
		}

	gps_data=UDR;

	}

	timeout=0;
	for(uint8_t i=0; i!=25; i++)
	{
		while(!(UCSRA & (1<<RXC)))
		{
				timeout++;
				if(timeout==TIMEOUT_REQ) return 2;

		}
			gps_data=UDR;


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
		while(!(UCSRA & (1<<RXC)))
		{
			timeout++;
			if(timeout==TIMEOUT_REQ) return 2;
		}

	gps_data=UDR;

	}

	for(uint8_t i=0; i<(sizeof(GPGGA)); i++)
	{
			timeout=0;
			while(!(UCSRA & (1<<RXC)))
			{
				timeout++;
				if(timeout==TIMEOUT_REQ) return 2;
			}

		gps_data=UDR;
	
		int gga_true=1;
		if(gps_data!=GPGGA[i]) gga_true=0;


		for(uint8_t n=0; n<5; n++)
		{
			timeout=0;
			while(!(UCSRA & (1<<RXC)))
			{
				timeout++;
				if(timeout==TIMEOUT_REQ) return 2;
			}

			gps_data=UDR;

		}

//////////////////////////////////////////////////////////////////////////////////////

		// GPS TIME //

		if(gga_true)
		{

			for(uint8_t n=0; n<6; n++)
			{
			
				timeout=0;
				while(!(UCSRA & (1<<RXC)))
				{
					timeout++;
					if(timeout==TIMEOUT_REQ) return 2;
				}

				gps_time[n]=UDR;

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
				while(!(UCSRA & (1<<RXC)))
				{
					timeout++;
					if(timeout==TIMEOUT_REQ) return 2;
				}

		gps_data=UDR;

		}


	if(gga_true)
	{

		for(uint8_t n=0; n<sizeof(gps_latitude); n++)
		{
			
			timeout=0;
			while(!(UCSRA & (1<<RXC)))
			{
				timeout++;
				if(timeout==TIMEOUT_REQ) return 2;
			}

			gps_latitude[n]=UDR;

		}
	}


//////////////////////////////////////////////////////////////////////////////////////

	// GPS LONGITUDE //

		for(uint8_t n=0; n<2; n++)
		{
			timeout=0;
			while(!(UCSRA & (1<<RXC)))
				{
					timeout++;
					if(timeout==TIMEOUT_REQ) return 2;
				}

		gps_data=UDR;

		}


		if(gga_true)
		{

			for(uint8_t n=0; n<sizeof(gps_longitude); n++)
			{
			
				timeout=0;
				while(!(UCSRA & (1<<RXC)))
				{
					timeout++;
					if(timeout==TIMEOUT_REQ) return 2;
				}

				gps_longitude[n]=UDR;

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
