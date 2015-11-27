



//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <avr/eeprom.h>
	#include <util/crc16.h>


// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>

	#include "lcd.h"
	#include "PCF8583.h"
	#include "TWI.h"

	#define BUT1 (1<<PB0)
	#define BUT2 (1<<PB1)
	#define BUT3 (1<<PB2)
	#define BUT4 (1<<PB3)

	#define BUT_SEL (1<<PD3)

	#define PIN_BUT PINB

	#define PIN_BUT_SEL PIND
	#define PORT_OUT PORTD

	
	#define LCD_LIGHT (1<<PD7)
	#define BUZ (1<<PD5)
	#define RTC_INT (1<<PD6)

	#define TIMEOUT_GPS_SEC			300

	#define SDA (1<<PC1)
	#define SCL (1<<PC0)

	char msg0[] = {0x01, 0x03, 0x06};

	char msg_gps[]="TIME GPS";
	char msg_rtc[]="TIME RTC";

	//#define SYM_GPS_NO	214	
	//#define SYM_GPS_YES	222
	#define SYM_ALARM	237
	#define SYM_ENTER	216

	uint8_t sym_visib0;
	uint8_t sym_visib1;

	uint16_t timeout_gps;

	struct alarm
	{
		char time[5];
		uint8_t active;
		uint8_t buz;

	};


void init(void)
{

	// Port A initialization //

	DDRA = 0xF7;

	PORTA = 0x00;

	///////////////////////////
	
	// Port B initialization //

	DDRB = 0x00;

	PORTB = BUT1 | BUT2 | BUT3 | BUT4;

	///////////////////////////

	// Port C initialization //

	DDRC = 0x00;

	PORTC = SDA | SCL;

	///////////////////////////

	// Port C initialization //

	DDRD = LCD_LIGHT | BUZ;

	PORTD = RTC_INT | LCD_LIGHT | BUT_SEL;

	///////////////////////////


	//Analog Comparator initialization.

	ACSR|=(1<<ACD);		//Disable Comparator.

	////////////////////////////////////////////////////////////////////

	//Timer0 initialization.

	
	////////////////////////////////////////////////////////////////////

	//Timer1 initialization.

	TCCR1B = (1<<CS12)|(0<<CS11)|(1<<CS10);

	OCR1A = 3125; //3125 6250

	TIMSK = (1<<OCIE1A);	//Interrupt Timer1 COMPA.

	////////////////////////////////////////////////////////////////////

	//Init USART.

	UBRRL = (uint8_t)(F_CPU/9600/16-1);			//BAUD RATE USART
	UBRRH = (uint8_t)(F_CPU/9600/16-1)>>8;

	UCSRB = (1<<RXEN);

	UCSRC|= (1<<UCSZ1) | (1<<UCSZ0) | (1<<URSEL);

	////////////////////////////////////////////////////////////////////

	//WATCHDOG initialization.

	WDTCR = (1<<WDTOE)|(1<<WDE);									//Enable WATCHDOG.

	WDTCR = (1<<WDTOE)|(1<<WDE)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0);	//WATCHDOG clock 2100ms.

	////////////////////////////////////////////////////////////////////

}

int main()
{

	init();
	
	TWIOpen(0x60);

	//LCD initialisation 
	LCD_Ini(); 					     
	// delay_ms(2);  		
	SEND_CMD(DISP_ON);
	// delay_ms(10);  						
	SEND_CMD(CLR_DISP); 

	SEND_STR("GPS TIME  DEVICE");

	asm volatile ("wdr");
	_delay_ms(1000);
	asm volatile ("wdr");
	SEND_CMD(CLR_DISP); 


	char Data_RX[256];
	uint8_t count_index = 0;
	char *pData;	


	sym_visib0=219;
	sym_visib1=220;

	SEND_CMD(CLR_DISP); 
			
	timeout_gps=0xFFFE;

	//for(uint8_t i=230; i<246; i++)
	//		SEND_CHAR(i);


	//while(1) asm volatile ("wdr");

	sei();

	while(1)
	{
		

		while(!(UCSRA & (1<<RXC)));

		Data_RX[count_index] = UDR;	
		count_index++;


		pData = memmem(Data_RX, count_index, msg0, sizeof(msg0));
		
		if(pData)
		{
			count_index = 0;
			uint8_t Data_Time[11];

			for(uint8_t i=0; i<sizeof(Data_Time); i++)
				Data_Time[i] = pData[i];
			

			
			
			uint16_t modbus_crc=0xFFFF;

			for(uint8_t i=0; i<sizeof(Data_Time); i++)
				modbus_crc=_crc16_update(modbus_crc, Data_Time[i]);

			for(uint8_t i=3; i<8; i++)
				if((Data_Time[i]>0x39)  || (Data_Time[i]<0x30)) modbus_crc = 1;

			if(!(modbus_crc))
			{

				char time_gps[8];

				time_gps[0] = Data_Time[3];
				time_gps[1] = Data_Time[4];
				time_gps[2] = ':';
				time_gps[3] = Data_Time[5];
				time_gps[4] = Data_Time[6];
				time_gps[5] = ':';
				time_gps[6] = Data_Time[7];
				time_gps[7] = Data_Time[8];
				
				Time_Zone_Update(time_gps);

				SetTimeRTC(time_gps);
				//for(uint8_t i=0; i<sizeof(time_gps); i++)
				//	SEND_CHAR(time_gps[i]);

				timeout_gps=0;
				
				cli();

				
				SEND_CMD(DD_RAM_ADDR2+6);
				
				SEND_STR(msg_gps);

				sei();
			}
			
		}
	}


}


int Time_Zone_Update(char *time_modbus)
{
			if(time_modbus[0]=='0' && time_modbus[1]=='0')
			{
				time_modbus[0]='0';
				time_modbus[1]='4';
				return 0;

			}

			if(time_modbus[0]=='0' && time_modbus[1]=='1')
			{
				time_modbus[0]='0';
				time_modbus[1]='5';
				return 0;
			}


			if(time_modbus[0]=='0' && time_modbus[1]=='2')
			{
				time_modbus[0]='0';
				time_modbus[1]='6';
				return 0;
			}


			if(time_modbus[0]=='0' && time_modbus[1]=='3')
			{
				time_modbus[0]='0';
				time_modbus[1]='7';
				return 0;
			}

			if(time_modbus[0]=='0' && time_modbus[1]=='4')
			{
				time_modbus[0]='0';
				time_modbus[1]='8';
				return 0;
			}

			if(time_modbus[0]=='0' && time_modbus[1]=='5')
			{
				time_modbus[0]='0';
				time_modbus[1]='9';
				return 0;
			}


			if(time_modbus[0]=='0' && time_modbus[1]=='6')
			{
				time_modbus[0]='1';
				time_modbus[1]='0';
				return 0;
			}

			if(time_modbus[0]=='0' && time_modbus[1]=='7')
			{
				time_modbus[0]='1';
				time_modbus[1]='1';
				return 0;
			}

			if(time_modbus[0]=='0' && time_modbus[1]=='8')
			{
				time_modbus[0]='1';
				time_modbus[1]='2';
				return 0;
			}

			if(time_modbus[0]=='0' && time_modbus[1]=='9')
			{
				time_modbus[0]='1';
				time_modbus[1]='3';
				return 0;
			}

			if(time_modbus[0]=='1' && time_modbus[1]=='0')
			{
				time_modbus[0]='1';
				time_modbus[1]='4';
				return 0;
			}
			if(time_modbus[0]=='1' && time_modbus[1]=='1')
			{
				time_modbus[0]='1';
				time_modbus[1]='5';
				return 0;
			}
			if(time_modbus[0]=='1' && time_modbus[1]=='2')
			{
				time_modbus[0]='1';
				time_modbus[1]='6';
				return 0;
			}

			if(time_modbus[0]=='1' && time_modbus[1]=='3')
			{
				time_modbus[0]='1';
				time_modbus[1]='7';
				return 0;
			}
			if(time_modbus[0]=='1' && time_modbus[1]=='4')
			{
				time_modbus[0]='1';
				time_modbus[1]='8';
				return 0;
			}
			if(time_modbus[0]=='1' && time_modbus[1]=='5')
			{
				time_modbus[0]='1';
				time_modbus[1]='9';
				return 0;
			}
			if(time_modbus[0]=='1' && time_modbus[1]=='6')
			{
				time_modbus[0]='2';
				time_modbus[1]='0';
				return 0;
			}
			if(time_modbus[0]=='1' && time_modbus[1]=='7')
			{
				time_modbus[0]='2';
				time_modbus[1]='1';
				return 0;
			}

			if(time_modbus[0]=='1' && time_modbus[1]=='8')
			{
				time_modbus[0]='2';
				time_modbus[1]='2';
				return 0;
			}
			if(time_modbus[0]=='1' && time_modbus[1]=='9')
			{
				time_modbus[0]='2';
				time_modbus[1]='3';
				return 0;
			}

			if(time_modbus[0]=='2' && time_modbus[1]=='0')
			{
				time_modbus[0]='0';
				time_modbus[1]='0';
				return 0;
			}

			if(time_modbus[0]=='2' && time_modbus[1]=='1')
			{
				time_modbus[0]='0';
				time_modbus[1]='1';
				return 0;
			}
			if(time_modbus[0]=='2' && time_modbus[1]=='2')
			{
				time_modbus[0]='0';
				time_modbus[1]='2';
				return 0;
			}
			if(time_modbus[0]=='2' && time_modbus[1]=='3')
			{
				time_modbus[0]='0';
				time_modbus[1]='3';
			}

	return 0;

}






/////////////////////////////////////////////////////////////////

//// INTERRUPT ////


ISR(TIMER1_COMPA_vect)
{
		TCNT1=0x0000;

		asm volatile ("wdr");

		char time_rtc[8];

		GetTimeRTC(time_rtc);

		char time_reset[]="01:01:01";

		uint8_t res=1;

		for(uint8_t i=0; i<sizeof(time_rtc); i++)
			if(time_rtc[i]!=time_reset[i])	res=0;
				
		if(res)
			while(1);			

		SEND_CMD(DD_RAM_ADDR);

		SEND_CHAR(0x20);
		SEND_CHAR(0x20);

		for(uint8_t i=0; i<sizeof(time_rtc); i++)
			SEND_CHAR(time_rtc[i]);
		
		if(sym_visib0==202)
		{
			sym_visib0=203;
			sym_visib1=202;
		}
		else
		{
			sym_visib0=202;
			sym_visib1=203;
		}


		SEND_CMD(DD_RAM_ADDR2+5);
			SEND_CHAR(sym_visib0);
		SEND_CMD(DD_RAM_ADDR2+14);
			SEND_CHAR(sym_visib1);

		timeout_gps++;

		if(timeout_gps>(TIMEOUT_GPS_SEC*2))
		{
			SEND_CMD(DD_RAM_ADDR2+6);
				
			SEND_STR(msg_rtc);
			timeout_gps=0xFFFE;
		}
}

///////////////////////////////////////////////////
