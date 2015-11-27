

	#include "DALLAS.h"	

	uint8_t bit;

uint8_t dallas(uint8_t command){


	uint8_t temp=SREG;

		cli();
	
		DDRB|=(DALLAS);

		DALLAS_PORT&=~DALLAS;

		RESET				//Output Reset.

		DALLAS_PORT|=DALLAS;

		//Delay input Presence.//
		for(uint16_t i=0; i<DELAY_PRESENCE; i++)
		{
			if(!(DALLAS_PIN & (DALLAS)))
			{
							 
				for(uint8_t t0=0; t0<PAUSE; t0++);	//PAUSE//

					//Output command//
				for (uint8_t t0=0; t0<8; t0++)
				{
					if(command & (1<<t0))
					{
						DALLAS_PORT&=~DALLAS;
							ONE_COMMAND
						DALLAS_PORT|=DALLAS;
							ZERO_COMMAND
					}
						else
						{
							DALLAS_PORT&=~DALLAS;
								ZERO_COMMAND
							DALLAS_PORT|=DALLAS;
								ONE_COMMAND
						}

					}


		
				DALLAS_PORT|=DALLAS;	//Dalas in HIGH.
		
				for(uint8_t t2=0; t2<DELAY_ONE; t2++)
				{
					asm volatile ("nop");
					if(!(DALLAS_PIN & (DALLAS))) bit++;
				}
		
				DALLAS_PORT&=~DALLAS;	//Dalas in LOW.
			
				for(uint8_t t2=0; t2<DELAY_ZERO; t2++) 	asm volatile ("nop");
	

	
				//Read code//
				for(uint8_t t0=0; t0<8; t0++)
				{
					for(uint8_t t1=0; t1<8; t1++)
					{
						DALLAS_PORT|=DALLAS;	//Dalas in HIGH.
		
						for(uint8_t t2=0; t2<DELAY_ONE; t2++)
						{
							asm volatile ("nop");
							if(!(DALLAS_PIN & (DALLAS))) bit++;
						}
		
						DALLAS_PORT&=~DALLAS;	//Dalas in LOW.
			
						for(uint8_t t2=0; t2<DELAY_ZERO; t2++) 	asm volatile ("nop");
		
						if(bit>ONE)
						{
							data_dallas[t0]&=~(1<<t1);	//0
						}
							else
							{
								data_dallas[t0]|=(1<<t1);	//1
							}

							bit=0;
						}
		
					}

		    	uint8_t crc = 0;
				uint8_t data[8];

				for (uint8_t t2= 0; t2<8; t2++) data[t2]=data_dallas[t2];
			

				for (uint8_t t2= 0; t2<7; t2++)
				{
					crc = _crc_ibutton_update(crc, data[t2]);
				}

				if(crc==data_dallas[7])
				{
			
					SREG=temp;
					return 1;
				}
					else
					{
						SREG=temp;
						return 0;
					}



		}
	}


	SREG=temp;
return 0;	//Exit if not presence.
}



