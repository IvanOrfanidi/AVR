

//1-фил, 2-бел/чер, 3-крас, 4-сер, 5-син, 6-черн, 7-зел.//


//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <avr/eeprom.h>

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>

	#include "lcd.h"
	#include "PCF8583.h"
	#include "TWI.h"
	#include "GPS.h"

	#define UPE0 2
	#define DOR0 3
	#define FE0 4

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

	#define SDA (1<<PC1)
	#define SCL (1<<PC0)


	#define START_TIMER1	TCCR1B=(1<<CS12)|(0<<CS11)|(0<<CS10);
	#define STOP_TIMER1		TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);

	#define ADDR_EEP_TIME_ALARM 0x0010
	char temp_eep[16]EEMEM="";
	char time_alarm_eep[5]EEMEM={'1','8',':','0','0'};

	char time_gps_valid[]="       TIME GPS ";
	char time_gps_invalid[]="       TIME RTC  ";

	char alarm_time[5];	

	uint8_t alarm;

void init(void)
{

	// Port A initialization //

	DDRA=0xF7;

	PORTA= 0x00;

	///////////////////////////
	
	// Port B initialization //

	DDRB=0x00;

	PORTB= BUT1 | BUT2 | BUT3 | BUT4;

	///////////////////////////

	// Port C initialization //

	DDRC=0x00;

	PORTC= SDA | SCL;

	///////////////////////////

	// Port C initialization //

	DDRD=LCD_LIGHT | BUZ;

	PORTD= RTC_INT | LCD_LIGHT | BUT_SEL;

	///////////////////////////


	//Analog Comparator initialization.

	ACSR|=_BV(ACD);		//Disable Comparator.

	////////////////////////////////////////////////////////////////////


	//Timer1 initialization.

	START_TIMER1;

	OCR1A=3125;

	TIMSK=(1<<OCIE1A);	//Interrupt Timer1 COMPA.
	////////////////////////////////////////////////////////////////////


	//WATCHDOG initialization.

	WDTCR=(1<<WDTOE)|(1<<WDE);									//Enable WATCHDOG.

	WDTCR=(1<<WDTOE)|(1<<WDE)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0);	//WATCHDOG clock 2100ms.


	////////////////////////////////////////////////////////////////////

}

void reset_cordinate(void)
{
	for(uint8_t i=0; i<sizeof(gps_latitude); i++)
	{
		gps_latitude[i]=0x20;
		gps_longitude[i]=0x20;

	}
}

void init_alarm_time(void)
{
	for(uint8_t i=0; i<sizeof(alarm_time); i++)
		alarm_time[i]=eeprom_read_byte((uint8_t *)ADDR_EEP_TIME_ALARM+i);
}



int main()
{
	init();

	reset_cordinate();

	init_alarm_time();

	gps_init();

	TWIOpen(0x40);

		//LCD initialisation 
	LCD_Ini(); 					     
	// delay_ms(2);  		
	SEND_CMD(DISP_ON);
	// delay_ms(10);  						
	SEND_CMD(CLR_DISP); 
	// write

	uint8_t timeout_reset_koordinat=0;
	uint8_t data_valid;

	sei();

	while(1)
	{

		data_valid=gps_stat();

		if(data_valid==0)
		{
			cli();

			timeout_reset_koordinat=0;

			while(gps_status());

			Time_Zone_Update();

			char temp_time[8];

			temp_time[0]=gps_time[0];
			temp_time[1]=gps_time[1];
			temp_time[2]=':';
			temp_time[3]=gps_time[2];
			temp_time[4]=gps_time[3];
			temp_time[5]=':';
			temp_time[6]=gps_time[4];
			temp_time[7]=gps_time[5];

			SetTimeRTC(temp_time);

			sei();
		}

		else

		{
			
			if(timeout_reset_koordinat==250)
				reset_cordinate(); 
				else
					timeout_reset_koordinat++;
		}
		
		if(!(PIN_BUT & BUT1))
		{
			STOP_TIMER1;

			
			SEND_CMD(CLR_DISP);

			SEND_CHAR(0x20);
			for(uint8_t i=0; i<sizeof(gps_latitude); i++)
				SEND_CHAR(gps_latitude[i]);

			SEND_CMD(DD_RAM_ADDR2);

			SEND_CHAR(0x20);
			for(uint8_t i=0; i<sizeof(gps_longitude); i++)
				SEND_CHAR(gps_longitude[i]);
			
			while(!(PIN_BUT & BUT1)) asm volatile ("wdr");
			_delay_ms(1000);

			SEND_CMD(CLR_DISP);

			START_TIMER1;
		}

		if(!(PIN_BUT & BUT2))
		{
			STOP_TIMER1;
			SEND_CMD(CLR_DISP);

			SEND_CMD(DD_RAM_ADDR);

			SEND_CHAR(0x20);

			for(uint8_t i=0; i<sizeof(alarm_time); i++)
				SEND_CHAR(alarm_time[i]);

			while(!(PIN_BUT & BUT2)) asm volatile ("wdr");
			_delay_ms(1000);

			SEND_CMD(CLR_DISP);

			START_TIMER1;
		}
		
		if(!(PIN_BUT_SEL & BUT_SEL))
		{
			STOP_TIMER1;

	set_alarm_config:

			SEND_CMD(CLR_DISP);

			SEND_CMD(DD_RAM_ADDR);

			SEND_CHAR(0x20);

			for(uint8_t i=0; i<sizeof(alarm_time); i++)
				SEND_CHAR(alarm_time[i]);

			for(uint8_t timeout=50; timeout>0; timeout--)
			{
				asm volatile ("wdr");
				
				_delay_ms(300);
				
				if(!(PIN_BUT & BUT1))
				{
					_delay_ms(50);

					if(PIN_BUT & BUT1) goto set_alarm_config;

					alarm_time[0]++;
					if(alarm_time[0]>'2') alarm_time[0]='0';
						
					goto set_alarm_config;
					
				}

				if(!(PIN_BUT & BUT2))
				{
					_delay_ms(50);

					if(PIN_BUT & BUT2) goto set_alarm_config;

					alarm_time[1]++;
					if(alarm_time[1]>'9') alarm_time[1]='0';
						
					goto set_alarm_config;
					
				}

				if(!(PIN_BUT & BUT3))
				{
					_delay_ms(50);

					if(PIN_BUT & BUT3) goto set_alarm_config;

					alarm_time[3]++;
					if(alarm_time[3]>'5') alarm_time[3]='0';
						
					goto set_alarm_config;
					
				}

				if(!(PIN_BUT & BUT4))
				{
					_delay_ms(50);

					if(PIN_BUT & BUT4) goto set_alarm_config;

					alarm_time[4]++;
					if(alarm_time[4]>'9') alarm_time[4]='0';
						
					goto set_alarm_config;
					
				}

				
			}



		START_TIMER1;

		}
	

	}
}

int Alarm(const char *pData_Time)
{
	for(uint8_t i=0; i<sizeof(alarm_time); i++)
		if(pData_Time[i]!=alarm_time[i]) return 0;

	return 1;
}


int Time_Zone_Update()
{
			if(gps_time[0]=='0' && gps_time[1]=='0')
			{
				gps_time[0]='0';
				gps_time[1]='4';
				return 0;

			}

			if(gps_time[0]=='0' && gps_time[1]=='1')
			{
				gps_time[0]='0';
				gps_time[1]='5';
				return 0;
			}


			if(gps_time[0]=='0' && gps_time[1]=='2')
			{
				gps_time[0]='0';
				gps_time[1]='6';
				return 0;
			}


			if(gps_time[0]=='0' && gps_time[1]=='3')
			{
				gps_time[0]='0';
				gps_time[1]='7';
				return 0;
			}

			if(gps_time[0]=='0' && gps_time[1]=='4')
			{
				gps_time[0]='0';
				gps_time[1]='8';
				return 0;
			}

			if(gps_time[0]=='0' && gps_time[1]=='5')
			{
				gps_time[0]='0';
				gps_time[1]='9';
				return 0;
			}


			if(gps_time[0]=='0' && gps_time[1]=='6')
			{
				gps_time[0]='1';
				gps_time[1]='0';
				return 0;
			}

			if(gps_time[0]=='0' && gps_time[1]=='7')
			{
				gps_time[0]='1';
				gps_time[1]='1';
				return 0;
			}

			if(gps_time[0]=='0' && gps_time[1]=='8')
			{
				gps_time[0]='1';
				gps_time[1]='2';
				return 0;
			}

			if(gps_time[0]=='0' && gps_time[1]=='9')
			{
				gps_time[0]='1';
				gps_time[1]='3';
				return 0;
			}

			if(gps_time[0]=='1' && gps_time[1]=='0')
			{
				gps_time[0]='1';
				gps_time[1]='4';
				return 0;
			}
			if(gps_time[0]=='1' && gps_time[1]=='1')
			{
				gps_time[0]='1';
				gps_time[1]='5';
				return 0;
			}
			if(gps_time[0]=='1' && gps_time[1]=='2')
			{
				gps_time[0]='1';
				gps_time[1]='6';
				return 0;
			}

			if(gps_time[0]=='1' && gps_time[1]=='3')
			{
				gps_time[0]='1';
				gps_time[1]='7';
				return 0;
			}
			if(gps_time[0]=='1' && gps_time[1]=='4')
			{
				gps_time[0]='1';
				gps_time[1]='8';
				return 0;
			}
			if(gps_time[0]=='1' && gps_time[1]=='5')
			{
				gps_time[0]='1';
				gps_time[1]='9';
				return 0;
			}
			if(gps_time[0]=='1' && gps_time[1]=='6')
			{
				gps_time[0]='2';
				gps_time[1]='0';
				return 0;
			}
			if(gps_time[0]=='1' && gps_time[1]=='7')
			{
				gps_time[0]='2';
				gps_time[1]='1';
				return 0;
			}

			if(gps_time[0]=='1' && gps_time[1]=='8')
			{
				gps_time[0]='2';
				gps_time[1]='2';
				return 0;
			}
			if(gps_time[0]=='1' && gps_time[1]=='9')
			{
				gps_time[0]='2';
				gps_time[1]='3';
				return 0;
			}

			if(gps_time[0]=='2' && gps_time[1]=='0')
			{
				gps_time[0]='0';
				gps_time[1]='0';
				return 0;
			}

			if(gps_time[0]=='2' && gps_time[1]=='1')
			{
				gps_time[0]='0';
				gps_time[1]='1';
				return 0;
			}
			if(gps_time[0]=='2' && gps_time[1]=='2')
			{
				gps_time[0]='0';
				gps_time[1]='2';
				return 0;
			}
			if(gps_time[0]=='2' && gps_time[1]=='3')
			{
				gps_time[0]='0';
				gps_time[1]='3';
			}

	return 0;

}



ISR(TIMER1_COMPA_vect)
{
	TCNT1=0x0000;
	
	asm volatile ("wdr");

	char temp_gps_time[8];

	GetTimeRTC(temp_gps_time);

	SEND_CMD(DD_RAM_ADDR);

	SEND_CHAR(0x20);
	for(uint8_t i=0; i<sizeof(temp_gps_time); i++)
		SEND_CHAR(temp_gps_time[i]);

	SEND_CMD(DD_RAM_ADDR2);

	if(gps_latitude[2]==0x20)
	{
		for(uint8_t i=0; i<sizeof(time_gps_invalid); i++)
			SEND_CHAR(time_gps_invalid[i]);
	}

	else
	{
		for(uint8_t i=0; i<sizeof(time_gps_valid); i++)
			SEND_CHAR(time_gps_valid[i]);
	}


	if(Alarm(temp_gps_time))
	{
		if(PIN_BUT!=0xFF) alarm=0;

		if(alarm) PORT_OUT|=BUZ; else PORT_OUT&=~BUZ;
	}
	else
	{
		PORT_OUT&=~BUZ;
		alarm=1;
	}
}











