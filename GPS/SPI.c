
//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>

	#include "SPI.h"
	
	uint8_t byte;


void spi_master_init()
{

	//Init port//

	DDRB|=(1<<SCK);
	DDRB|=(1<<MOSI);
	DDRB|=(1<<SS);

	DDRB&=~(1<<MISO);

	//////////////

	//Init FREQ//

	uint8_t status;
	uint8_t spi2x;

	//

	if(SCK_FREQ==4)
	{
		status=(0<<SPR1)|(0<<SPR0);
		spi2x=(0<<SPI2X);
	}

	if(SCK_FREQ==2)
	{
		status=(0<<SPR1)|(0<<SPR0);
		spi2x=(1<<SPI2X);
	}

	if(SCK_FREQ==16)
	{
		status=(0<<SPR1)|(1<<SPR0);
		spi2x=(0<<SPI2X);
	}

	if(SCK_FREQ==8)
	{
		status=(0<<SPR1)|(1<<SPR0);
		spi2x=(1<<SPI2X);
	}

	if(SCK_FREQ==64)
	{
		status=(1<<SPR1)|(0<<SPR0);
		spi2x=(0<<SPI2X);
	}
	if(SCK_FREQ==32)
	{
		status=(1<<SPR1)|(0<<SPR0);
		spi2x=(1<<SPI2X);
	}

	if(SCK_FREQ==128)
	{
		status=(1<<SPR1)|(1<<SPR0);
		spi2x=(0<<SPI2X);
	}

	////////////////////

	//Init MASTER//

	status|=(1<<MSTR);

	///////////////

	//Init SPI Enable//

	status|=(1<<SPE);

	///////////////

	//Init Interrupt Enable//
	if(MASTER_INTERRUPT_ENABLE)
	{
		status|=(1<<SPIE);

	}
		else
		{
			status&=~(1<<SPIE);
		}
	////////////////////////	


	//Init Polarity//

	if(CLOCK_POLARITY)
	{
		status|=(1<<CPOL);
	}
		else
		{
			status&=~(1<<CPOL);
		}

	/////////////////

	//Init Phase//

	if(CLOCK_PHASE)
	{
		status|=(1<<CPHA);
	}
		else
		{
			status&=~(1<<CPHA);
		}

	//////////////

	//Init Data Order//

	if(DATA_ORDER)
	{
		status|=(1<<DORD);
	}
		else
		{
			status&=~(1<<DORD);
		}

	//////////////////


	SPCR=status;
	SPSR=spi2x;



}

void spi_slave_init()
{
	
	//Init port//

	DDRB&=~(1<<SCK);
	DDRB&=~(1<<MOSI);
	DDRB&=~(1<<SS);

	DDRB|=(1<<MISO);

	//////////////


	uint8_t status;

	//Init SLAVE//

	status=(0<<MSTR);

	///////////////

	//Init SPI Enable//

	status|=(1<<SPE);

	///////////////

	//Init Interrupt Enable//
	if(SLAVE_INTERRUPT_ENABLE)
	{
		status|=(1<<SPIE);

	}
		else
		{
			status&=~(1<<SPIE);
		}
	////////////////////////	


	//Init Polarity//

	if(CLOCK_POLARITY)
	{
		status|=(1<<CPOL);
	}
		else
		{
			status&=~(1<<CPOL);
		}

	/////////////////

	//Init Phase//

	if(CLOCK_PHASE)
	{
		status|=(1<<CPHA);
	}
		else
		{
			status&=~(1<<CPHA);
		}

	//////////////

	//Init Data Order//

	if(DATA_ORDER)
	{
		status|=(1<<DORD);
	}
		else
		{
			status&=~(1<<DORD);
		}

	//////////////////


	SPCR=status;

}


ISR(SPI_STC_vect)
{


}



