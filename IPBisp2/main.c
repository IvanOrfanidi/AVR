



//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <avr/wdt.h>
	#include <avr/eeprom.h>

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>

	#include <util/crc16.h>

	#include "USART.h"


	#define PWM				(1<<PD6)

	#define DEVICE_UPRS	0x31	//For Device UPRS
	#define DEVICE_UDPS	0x32	//For Device UDPS

	#define LED_GO_ON	0x31
	#define LED_GO_OFF	0x32

	#define ADD_PWM_VALUE 0x0001

const uint8_t array_crc8[]={
 0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
 157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
 35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
 190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
 70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
 219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
 101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
 248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
 140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
 17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
 175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
 50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
 202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
 87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
 233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
 116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
 };


void init()
{

	// Port B initialization //

	DDRB=0x00;

	PORTB=0xFF;

	///////////////////////////


	// Port D initialization //

	DDRD=(1<<PD6);

	PORTD=0x00;

	///////////////////////////

	// Port C initialization //

	DDRC=0x00;

	PORTC=0x00;

	///////////////////////////

	//Timer0initialization.

	TCCR0B=(0<<CS02)|(0<<CS01)|(1<<CS00);	//Timer1 clock on.

	OCR0A=eeprom_read_byte(ADD_PWM_VALUE);

	TCCR0A=(1<<COM0A1)|(1<<COM0A0)|(1<<WGM00);
	/////////////////////////////////////////////////////////////////////


	//Watchdog initialization.

	//!!!WDTON fuse set to “0“ means programmed and “1“ means unprogrammed!!!//

	cli();
	wdt_reset();		//Watchdog Reset.
	wdt_enable(WDTO_8S);

	////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////////////////

}

uint8_t select_slave_id(void)
{
	return ~((PINB) | (PINC<<6));
}


int main()
{


	init();

	usart0_init();


	usart0_write(0x30);
	_delay_ms(1);
	usart0_write(0x30);
	_delay_ms(1);
	usart0_write(0x30);

	uint8_t my_slave_id = select_slave_id();


	sei();
	
	//PORTD|=PWM;

	while(1)
	{
		
		if(usart0_rx_len())
		{

			wdt_reset();		//Watchdog Reset.

			_delay_ms(10);

			uint8_t Len=usart0_rx_len();
			uint8_t rx_buffer[Len];

			for(uint8_t i=0; i<Len; i++)
				rx_buffer[i]=usart0_read();

			if((rx_buffer[3]==_crc8_update(rx_buffer, (sizeof(rx_buffer)-1))) & (my_slave_id==rx_buffer[0]))
			{
				//Если девайс УПРС.
				if(rx_buffer[1]==DEVICE_UPRS)
				{
					if((rx_buffer[2]==LED_GO_OFF) & (OCR0A<255))
					{
						OCR0A++;
					}

					if((rx_buffer[2]==LED_GO_ON) & (OCR0A>0))
					{
						OCR0A--;


					}

					eeprom_update_byte(ADD_PWM_VALUE, OCR0A);	//Пишем значние ШИМ в EEPROM, для востановления его при новом включении.
					
				}
				//////////////////////////////////////////


				//Если девайс УДПС.
				if(rx_buffer[1]==DEVICE_UDPS)
				{
					uint8_t temp_pwm;

					if(OCR0A<2)
						temp_pwm=255;
					
					if((OCR0A<64) && (OCR0A>1))
						temp_pwm=1;


					if((OCR0A<128) && (OCR0A>64))
						temp_pwm=63;

					if((OCR0A<192) && (OCR0A>128))
						temp_pwm=127;

					if((OCR0A<=255) && (OCR0A>192))
						temp_pwm=191;



					OCR0A=temp_pwm;

					eeprom_update_byte(ADD_PWM_VALUE, OCR0A);	//Пишем значние ШИМ в EEPROM, для востановления его при новом включении.
									
				}
				//////////////////////////////////////////

				
			}
	
		}


		wdt_reset();		//Watchdog Reset.

	}
}

int _crc8_update(char *pTxBuffer, int Len)
{
	uint8_t crc8=0;
	
	for (uint8_t i=0; i<Len; i++)
	{
		crc8=crc8 ^ pTxBuffer[i];
		crc8=array_crc8[crc8];
	}

return crc8;			
}
