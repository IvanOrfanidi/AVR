

	#include "DALLAS.h"	

	uint8_t bit;

	//////////////////

	//(0 - OK, 1 - NOT PRESENCE, 2 - ERROR CRC, 3 - LINE CLOSE)

uint8_t dallas(char *pData_Dallas, uint8_t command){

		cli();

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
							pData_Dallas[t0]&=~(1<<t1);	//0
						}
							else
							{
								pData_Dallas[t0]|=(1<<t1);	//1
							}

							bit=0;
						}
		
					}

		    	uint8_t crc = 0;
				uint8_t data_temp[8];

				for (uint8_t t2=0; t2<8; t2++)
					data_temp[t2]=pData_Dallas[t2];
			

				for (uint8_t t2=0; t2<7; t2++)
					crc = _crc_ibutton_update(crc, data_temp[t2]);

				if(crc==pData_Dallas[7])
				{
					for (uint8_t t2=0; t2<8; t2++)
						if(pData_Dallas[t2]!=0x00) return 0;
						return 3;
				}
					else
					{
						return 2;
					}



		}
	}

return 1;	//Exit if not presence.
}
