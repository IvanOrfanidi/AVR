
//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <avr/eeprom.h>
	#include <avr/wdt.h>

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>

	#include "enc28j60.h"
	#include "ip_arp_udp_tcp.h"
	#include "net.h"
	#include "USART.h"
	#include "lcd.h"
	#include "TWI.h"
	#include "PCF8583.h"
	#include "MODBUS_SLAVE_TCP_UDP.h"

	#define true      1
	#define false     0
	#define bool      _Bool

	#define LED_RUN	(1<<PD7)
	#define BUZ	(1<<PD6)

	#define PORT_INF	PORTD

	#define OUT1	(1<<PB2)
	#define OUT2	(1<<PB0)

	#define PORT_OUT	PORTB


	#define IN1	(1<<PC7)
	#define IN2	(1<<PC6)

	#define PIN_IN	PINC

	#define CS_SPI_ENC28J60	(1<<PB4)

	#define BUT2	(1<<PD5)
	#define BUT1	(1<<PB3)


	#define BUT2_ON	(!(PIND & (BUT2)))
	#define BUT1_ON	(!(PINB & (BUT1)))

	
	#define START_SYS_TIMER		TCCR1B=(1<<CS12)|(0<<CS11)|(1<<CS10)
	#define STOP_SYS_TIMER		TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10)


	#define ADDR_MYIP_EEP 0x0000
	#define EEP_ADD_BAUDRATE_RS232 0x0010

	#define EEP_ADD_MODBUS_ADDR 0x0012
	#define EEP_ADD_UDP_PORT 0x0013 // 2 byte.


//**************************************************************************************

// please modify the following two lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
static uint8_t mymac[6] = {0x00,0x2F,0x68,0x12,0xAC,0x30}; 
static uint8_t myip[4];
char myip_str[]="XXXxXXXxXXXxXXX";

uint8_t myip_eep[12]EEMEM = {'1','9','2','1','6','8','0','0','1','2','5','0'};

const char http_start[]="<a href=\"http://";

//static uint16_t mywwwport = 150; // listen port for tcp/www (max range 1-254)

#define BUFFER_SIZE 512

//// listen port for udp
//#define UDP_PORT 1200
#define TCP_PORT 80				//listen port for tcp/www (max range 1-254)

static uint8_t buf[BUFFER_SIZE+1];

bool timeUpdate = false;

bool countUp0 = true;
bool countUp1 = true;


//**************************************************************************************

uint8_t Slave_ID;
uint16_t UDP_Port;
uint8_t BaudRate;

//**************************************************************************************

uint16_t CountMenu = 0;

//**************************************************************************************


uint8_t GetCommand (char *x)
{
   if (strncmp("/ ", x, 2) == 0) return 0;
   if (strncmp("/OUT1_1", x, 7) == 0) return 1;
   if (strncmp("/OUT1_0", x, 7) == 0) return 2;
   if (strncmp("/OUT2_1", x, 7) == 0) return 3;
   if (strncmp("/OUT2_0", x, 7) == 0) return 4;
   return 0;
}


//**************************************************************************************
uint16_t PrintWebpage (uint8_t *buf)
{


   uint16_t plen;
   plen = Fill_tcp_data_p(buf, 0, PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
   plen = Fill_tcp_data_p(buf, plen, PSTR("<center><h1>Welcome to WWW.Home #2!!!<hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("Author Ivan Orfanidi.<hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("SYSTEM TIME "));

   char temp_time[8];

   GetTimeRTC(temp_time);
   plen = Fill_tcp_data_len(buf,plen,temp_time, sizeof(temp_time));
   plen = Fill_tcp_data_p(buf,plen,PSTR("<hr>"));

	char text_http[]="\">REFRESH</a><hr>";

	plen = Fill_tcp_data(buf,plen,http_start);
	plen = Fill_tcp_data(buf,plen,myip_str);
	plen = Fill_tcp_data(buf,plen,text_http);


   if (countUp0)
   {
   		PORT_OUT&= ~OUT1;

		char text_info[]="/OUT1_1\">RELAY #1 ON</a><hr>";

		plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,myip_str);
		plen = Fill_tcp_data(buf,plen,text_info);

   }
	else
	{
		PORT_OUT|= OUT1;

		char text_info[]="/OUT1_0\">RELAY #1 OFF</a><hr>";

	    plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,myip_str);
		plen = Fill_tcp_data(buf,plen,text_info);
	}

   if (countUp1)
   {
   		PORT_OUT&= ~OUT2;

		char text_info[]="/OUT2_1\">RELAY #2 ON</a><hr>";

		plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,myip_str);
		plen = Fill_tcp_data(buf,plen,text_info);   
	}
	else
	{
		PORT_OUT|= OUT2;

		char text_info[]="/OUT2_0\">RELAY #2 OFF</a><hr>";

	    plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,myip_str);
		plen = Fill_tcp_data(buf,plen,text_info);

	}


   return plen;

}

/******************************************************************************/



//*****************************************************************************
void LanTask (void)
{
   uint16_t plen, dat_p = 0;
   bool send;
   
   while (1)
   {
      plen = Enc28j60PacketReceive(BUFFER_SIZE, buf);
         /*plen will unequal to zero if there is a valid packet (without crc error) */
      if (plen == 0) return;

      // arp is broadcast if unknown but a host may also verify the mac address by sending it to a unicast address.
      if (Eth_type_is_arp_and_my_ip(buf, plen))
      {
         Make_arp_answer_from_request(buf);
         continue;
      }
      // check if the ip packet is for us:
      if (Eth_type_is_ip_and_my_ip(buf, plen) == 0)
      {
         continue;
      }
      // ping
      if((buf[IP_PROTO_P] == IP_PROTO_ICMP_V) && (buf[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V))
      {
         Make_echo_reply_from_request(buf, plen);
         continue;
      }

      // tcp port www start, compare only the lower byte
      if ((buf[IP_PROTO_P] == IP_PROTO_TCP_V) && (buf[TCP_DST_PORT_H_P] == 0) && (buf[TCP_DST_PORT_L_P] == TCP_PORT))
      {
         if (buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
         {
            Make_tcp_synack_from_syn(buf); // make_tcp_synack_from_syn does already send the syn,ack
            continue;
         }
         if (buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
         {
            Init_len_info(buf); // init some data structures
            dat_p = Get_tcp_data_pointer();
            if (dat_p == 0)
            { 
               // we can possibly have no data, just ack:
               if (buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V)
               {
                  Make_tcp_ack_from_any(buf);
               }
               continue;
            }
         }
         send = false;
         if (strncmp("GET ",(char *) & (buf[dat_p]), 4) != 0)
         {
          	// head, post and other methods for possible status codes see:
            // http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
            plen = Fill_tcp_data_p(buf, 0, PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>"));
            send = true;
         }

	      if (!send)
         {
            char *p;
            p = (char*) &(buf[dat_p+4]);  // start of incomming payload
            switch (GetCommand(p))
            {
               case 0:
                  break;
               case 1:
                  countUp0 = false;
                  break;
			   case 2:
                  countUp0 = true;
                  break;
			   case 3:
                  countUp1 = false;
                  break;
			   case 4:
                  countUp1 = true;
                  break;

            }
			
            plen = PrintWebpage(buf);
            send = true;
         }

         Make_tcp_ack_from_any(buf); // send ack for http get
         Make_tcp_ack_with_data(buf, plen); // send data       
      }

	  	// udp interface:
      if (buf[IP_PROTO_P]==IP_PROTO_UDP_V)
	  {
        	uint8_t payloadlen=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;
            // the received command has to start with t and be 4 char long
            // e.g "test\0"
            if(payloadlen)
			{
				
				char Data_TX[payloadlen];
				
				for(uint8_t i=0; i<sizeof(Data_TX); i++)
					Data_TX[i]=buf[UDP_DATA_P+i];

				char len[0];

				uint16_t status_mobbus = modbus_slave_status(Data_TX,sizeof(Data_TX), Slave_ID, len);

				if((status_mobbus!=NOT_SLAVE_ID) && (status_mobbus!=ERROR_CRC) && (buf[UDP_DST_PORT_H_P] == (UDP_Port>>8)) && (buf[UDP_DST_PORT_L_P] == (uint8_t)UDP_Port))
				{
					char Data_RX[len[0]];

					modbus_slave_tcp_udp(Data_TX, Data_RX);

					Make_udp_reply_from_request(buf,Data_RX,sizeof(Data_RX),UDP_Port);
				}

				
			}
       }// End udp interface.
   }
}

//*****************************************************************************************************************************

void init(void)
{


	//Watchdog initialization.
	cli();
	wdt_reset();
	wdt_disable();
	///////////////////////////////////


	// Port A initialization //
	DDRA= 0xFF;
 	PORTA= 0x00;
	//////////////////////////

	// Port B initialization //
	DDRB= OUT1 | OUT2 | CS_SPI_ENC28J60;
 	PORTB=  (1<<PB3);
	//////////////////////////

	// Port C initialization //
	DDRC= 0x00;
	PORTC= IN1 | IN2;
	//////////////////////////

	// Port D initialization //
	DDRD= BUZ | LED_RUN;
	PORTD= (1<<PD5);
	//////////////////////////

	//Analog Comparator initialization.

	ACSR|=(1<<ACD);		//Disable Comparator.

	/////////////////////////////////////////////////////////////////////


	// Ext. Interrupt initialization.
	MCUCR &= 0xFE;       // INT 0 sense on falling edge
	MCUCR |= 0x02;
	GICR |= (1<<INT0);        // enable interrupt on INT 0
	///////////////////////////////////

	//Timer1 initialization //

	START_SYS_TIMER;	//FCPU/1024

	OCR1A=14400;							//Interrupt Timer 1 sec.

	TIMSK=(1<<OCIE1A);	//Interrupt Timer1 COMPA.

	////////////////////////////////////////////////////////////////////


}


/////////////////////////////////////////////////////////////////////////

void LCD_Button(char *ptrMSG1, char *ptrMSG2)
{

	SEND_CMD(DD_RAM_ADDR1);

	uint8_t i = 0;

	while(ptrMSG1[i]!='\0')
	{
		SEND_CHAR(ptrMSG1[i]);
		i++;
	}


	SEND_CMD(DD_RAM_ADDR2);

	i = 0;

	while(ptrMSG2[i]!='\0')
	{
		SEND_CHAR(ptrMSG2[i]);
		i++;
	}	
}



/////////////////////////////////////////////////////////////////////////


void Init_Def(void)
{

	Slave_ID = eeprom_read_byte(EEP_ADD_MODBUS_ADDR);
	UDP_Port = eeprom_read_word(EEP_ADD_UDP_PORT);
	BaudRate = eeprom_read_byte(EEP_ADD_BAUDRATE_RS232);
}


/////////////////////////////////////////////////////////////////


void Init_My_IP(char *ptrMyIP, char *prtMyIP_Str)
{
	uint8_t temp;
	char temp_myip_str[12];

	for(uint8_t i=0; i<4; i++)
		ptrMyIP[i]=0;

	//Init MyIP String//
	for(uint16_t i=0; i<12; i++)
	{
		temp_myip_str[i]=eeprom_read_byte(i+ADDR_MYIP_EEP);
	
	}

	uint8_t m=0;
	uint8_t index_prtMyIP_Str=0;

	for(uint8_t tetr=0; tetr<4; tetr++)
	{
		uint8_t dec_tetr=4;
		for(uint8_t i=(tetr*3); i<((tetr*3)+3); i++)
		{
			if(temp_myip_str[i]!='0')
			{
				m=i;
				i=(tetr*3)+3;	
			}
			dec_tetr--;
		}
		
		for(uint8_t i=m; i<(m+dec_tetr); i++)
		{
			prtMyIP_Str[index_prtMyIP_Str]=temp_myip_str[i];
			index_prtMyIP_Str++;
		}

		prtMyIP_Str[index_prtMyIP_Str]='.';
		index_prtMyIP_Str++;
		
	}

	prtMyIP_Str[index_prtMyIP_Str-1]=0;
	/////////////////////////////////////////////////////////////


	//Init MyIP//
	for(uint8_t index_MyIP=0; index_MyIP<4; index_MyIP++)
	{
		uint8_t n=100;

		
		for(uint16_t i=(index_MyIP*3); i<((index_MyIP*3)+3); i++)
		{
			temp=(eeprom_read_byte(i+ADDR_MYIP_EEP)-0x30);
			ptrMyIP[index_MyIP]+=temp*n;
			n/=10;
		} 
	}



}

/////////////////////////////////////////////////////////////////

void But1_ON(void)
{
	while(BUT1_ON);
	PORT_INF&=~LED_RUN;
	PORT_INF|=BUZ;
	_delay_ms(200);
	PORT_INF&=~BUZ;
	PORT_INF|=LED_RUN;
}

void But2_ON(void)
{
	while(BUT2_ON);
	PORT_INF&=~LED_RUN;
	PORT_INF|=BUZ;
	_delay_ms(200);
	PORT_INF&=~BUZ;
	PORT_INF|=LED_RUN;
}
/////////////////////////////////////////////////////////////////


uint16_t DisplayMenu(void)
{
	uint16_t KCount=10;

	switch(CountMenu)
	{

		case 10: {
			LCD_Button("            TIME", "    ENTER SELECT");
			KCount=20;
			break;
		}
		case 30: {
			LCD_Button("           RS232", "    ENTER SELECT");
			KCount=30;
			break;
		}

		case 29: {		//BAUD RATE.
			LCD_Button("BAUD RATE  RS232", "    ENTER SELECT");
			KCount=20;
			break;
		}

		case 28: {		//SELECT BAUD RATE.

			switch(BaudRate)
			{
				case 1:{
					LCD_Button("2400   BAUD RATE", "      EDIT  EXIT");
					break;
				}

				case 2:{
					LCD_Button("4800   BAUD RATE", "      EDIT  EXIT");
					break;
				}

				case 3:{
					LCD_Button("9600   BAUD RATE", "      EDIT  EXIT");
					break;
				}
				
				case 4:{
					LCD_Button("14400  BAUD RATE", "      EDIT  EXIT");
					break;
				}

				case 5:{
					LCD_Button("19200  BAUD RATE", "      EDIT  EXIT");
					break;
				}

				case 6:{
					LCD_Button("28800  BAUD RATE", "      EDIT  EXIT");
					break;
				}

				case 7:{
					LCD_Button("38400  BAUD RATE", "      EDIT  EXIT");
					break;
				}

				case 8:{
					LCD_Button("57600  BAUD RATE", "      EDIT  EXIT");
					break;
				}

				case 9:{
					LCD_Button("76800  BAUD RATE", "      EDIT  EXIT");
					break;
				}

				case 10:{
					LCD_Button("115200 BAUD RATE", "      EDIT  EXIT");
					break;
				}
			}


			KCount=1;
			break;
		}

		case 27: {
			
			CountMenu=31+BaudRate;
			KCount=1;
			break;
		}

		case 32: {

			LCD_Button("2400            ", "    ENTER SELECT");
			
			while(!((BUT1_ON) || (BUT2_ON)));
			
			if(BUT1_ON)
			{
				But1_ON();
				CountMenu++;
							
			}

			if(BUT2_ON)
			{
				But2_ON();
				
				BaudRate=1;
				eeprom_update_byte(EEP_ADD_BAUDRATE_RS232, BaudRate);
				usart0_mod_init(BaudRate);
				CountMenu=28;
							
			}

			KCount=1;
			break;
		}

		case 33: {

			LCD_Button("4800            ", "    ENTER SELECT");
			
			while(!((BUT1_ON) || (BUT2_ON)));
			
			if(BUT1_ON)
			{
				But1_ON();
				CountMenu++;
							
			}

			if(BUT2_ON)
			{
				But2_ON();
				
				BaudRate=2;
				eeprom_update_byte(EEP_ADD_BAUDRATE_RS232, BaudRate);
				usart0_mod_init(BaudRate);
				CountMenu=28;
							
			}

			KCount=1;
			break;
		}

		case 34: {

			LCD_Button("9600            ", "    ENTER SELECT");
			
			while(!((BUT1_ON) || (BUT2_ON)));
			
			if(BUT1_ON)
			{
				But1_ON();
				CountMenu++;
							
			}

			if(BUT2_ON)
			{
				But2_ON();
				
				BaudRate=3;
				eeprom_update_byte(EEP_ADD_BAUDRATE_RS232, BaudRate);
				usart0_mod_init(BaudRate);
				CountMenu=28;
							
			}

			KCount=1;
			break;
		}

		case 35: {

			LCD_Button("14400           ", "    ENTER SELECT");
			
			while(!((BUT1_ON) || (BUT2_ON)));
			
			if(BUT1_ON)
			{
				But1_ON();
				CountMenu++;
							
			}

			if(BUT2_ON)
			{
				But2_ON();
				
				BaudRate=4;
				eeprom_update_byte(EEP_ADD_BAUDRATE_RS232, BaudRate);
				usart0_mod_init(BaudRate);
				CountMenu=28;
							
			}

			KCount=1;
			break;
		}


		case 36: {

			LCD_Button("19200           ", "    ENTER SELECT");
			
			while(!((BUT1_ON) || (BUT2_ON)));
			
			if(BUT1_ON)
			{
				But1_ON();
				CountMenu++;
							
			}

			if(BUT2_ON)
			{
				But2_ON();
				
				BaudRate=5;
				eeprom_update_byte(EEP_ADD_BAUDRATE_RS232, BaudRate);
				usart0_mod_init(BaudRate);
				CountMenu=28;
							
			}

			KCount=1;
			break;
		}

		case 37: {

			LCD_Button("28800           ", "    ENTER SELECT");
			
			while(!((BUT1_ON) || (BUT2_ON)));
			
			if(BUT1_ON)
			{
				But1_ON();
				CountMenu++;
							
			}

			if(BUT2_ON)
			{
				But2_ON();
				
				BaudRate=6;
				eeprom_update_byte(EEP_ADD_BAUDRATE_RS232, BaudRate);
				usart0_mod_init(BaudRate);
				CountMenu=28;
							
			}

			KCount=1;
			break;
		}

		case 38: {

			LCD_Button("38400           ", "    ENTER SELECT");
			
			while(!((BUT1_ON) || (BUT2_ON)));
			
			if(BUT1_ON)
			{
				But1_ON();
				CountMenu++;
							
			}

			if(BUT2_ON)
			{
				But2_ON();
				
				BaudRate=7;
				eeprom_update_byte(EEP_ADD_BAUDRATE_RS232, BaudRate);
				usart0_mod_init(BaudRate);
				CountMenu=28;
							
			}

			KCount=1;
			break;
		}

		case 39: {

			LCD_Button("57600           ", "    ENTER SELECT");
			
			while(!((BUT1_ON) || (BUT2_ON)));
			
			if(BUT1_ON)
			{
				But1_ON();
				CountMenu++;
							
			}

			if(BUT2_ON)
			{
				But2_ON();
				
				BaudRate=8;
				eeprom_update_byte(EEP_ADD_BAUDRATE_RS232, BaudRate);
				usart0_mod_init(BaudRate);
				CountMenu=28;
							
			}

			KCount=1;
			break;
		}

		case 40: {

			LCD_Button("76800           ", "    ENTER SELECT");
			
			while(!((BUT1_ON) || (BUT2_ON)));
			
			if(BUT1_ON)
			{
				But1_ON();
				CountMenu++;
							
			}

			if(BUT2_ON)
			{
				But2_ON();
				
				BaudRate=9;
				eeprom_update_byte(EEP_ADD_BAUDRATE_RS232, BaudRate);
				usart0_mod_init(BaudRate);
				CountMenu=28;
							
			}

			KCount=1;
			break;
		}

		case 41: {

			LCD_Button("115200           ", "    ENTER SELECT");
			
			while(!((BUT1_ON) || (BUT2_ON)));
			
			if(BUT1_ON)
			{
				But1_ON();
				CountMenu=31;
							
			}

			if(BUT2_ON)
			{
				But2_ON();
				
				BaudRate=10;
				eeprom_update_byte(EEP_ADD_BAUDRATE_RS232, BaudRate);
				usart0_mod_init(BaudRate);
				CountMenu=28;
							
			}

			KCount=1;
			break;
		}


		case 31: {

			LCD_Button("            EXIT", "    ENTER SELECT");
			
			if(BUT1_ON)
			{
				But1_ON();
				CountMenu=32;
							
			}

			if(BUT2_ON)
			{
				_delay_ms(100);
				if(BUT2_ON)
				{
					But2_ON();
					CountMenu=28;
				}
			}

			KCount=1;
			break;
		}
		

		case 60: {
			LCD_Button("        ETHERNET", "    ENTER SELECT");
			KCount=40;
			break;
		}
		
		case 100: {
			LCD_Button("            EXIT", "    ENTER SELECT");
			KCount=50;
			break;
		}

		case 99: {
			CountMenu=0;	//EXIT
			KCount=10;
			break;
		}

		case 59: {			//IP
			LCD_Button("IP      ETHERNET", "    ENTER SELECT");
			KCount=10;
			break;
		}

		case 58: {			//IP Address
			
			char temp_myip_str[12];
			for(uint16_t i=0; i<sizeof(temp_myip_str); i++)
				temp_myip_str[i]=eeprom_read_byte(i+ADDR_MYIP_EEP);
	
			SEND_CMD(DD_RAM_ADDR1);

			for(uint8_t i=0; i<3; i++)
				SEND_CHAR(temp_myip_str[i]);

			SEND_CHAR('.');

			for(uint8_t i=3; i<6; i++)
				SEND_CHAR(temp_myip_str[i]);

			SEND_CHAR('.');

			for(uint8_t i=6; i<9; i++)
				SEND_CHAR(temp_myip_str[i]);

			SEND_CHAR('.');
			
			for(uint8_t i=9; i<12; i++)
				SEND_CHAR(temp_myip_str[i]);
			
			SEND_CHAR(' ');

			LCD_Button("", "      EDIT  EXIT");

			KCount=1;
			break;
		}
		
		case 57: {		//Edit IP Address.
				
				bool end_ip=1;
				uint8_t count_ip=0;


				while(end_ip)
				{
					if(count_ip!=12) LCD_Button("", "    ENTER SELECT");

					char temp_myip_str[12];
					for(uint16_t i=0; i<sizeof(temp_myip_str); i++)
						temp_myip_str[i]=eeprom_read_byte(i+ADDR_MYIP_EEP);
	
					SEND_CMD(DD_RAM_ADDR1);

					for(uint8_t i=0; i<3; i++)
						SEND_CHAR(temp_myip_str[i]);

					SEND_CHAR('.');

					for(uint8_t i=3; i<6; i++)
						SEND_CHAR(temp_myip_str[i]);

					SEND_CHAR('.');

					for(uint8_t i=6; i<9; i++)
						SEND_CHAR(temp_myip_str[i]);

					SEND_CHAR('.');
			
					for(uint8_t i=9; i<12; i++)
						SEND_CHAR(temp_myip_str[i]);
			
					SEND_CHAR(' ');



					if(count_ip==12)
					{
						LCD_Button("", "      EDIT  EXIT");

						if(BUT1_ON)
							{
		
								But1_ON();
								end_ip=0;
							
							}

							if(BUT2_ON)
							{
								_delay_ms(100);
								if(BUT2_ON)
								{
									But2_ON();
									count_ip=0;
								}
							}

					}
					else
					{
						SEND_CMD(DD_RAM_ADDR1);

						for(uint8_t i=0; i<count_ip; i++)
						{
							SEND_CHAR(temp_myip_str[i]);
							if((i==2) || (i==5) || (i==8)) 
								SEND_CHAR('.');
						}

						SEND_CHAR(255);

						for(uint8_t i=10; i!=0; i--)
						{
							_delay_ms(100);
							if(BUT1_ON)
							{
		
								But1_ON();
								temp_myip_str[count_ip]++;
							
							}

							if(BUT2_ON)
							{
								_delay_ms(100);
								if(BUT2_ON)
								{
									But2_ON();
									count_ip++;
								}
							}
						}

						SEND_CMD(DD_RAM_ADDR1);

						for(uint8_t i=0; i<count_ip; i++)
						{
							SEND_CHAR(temp_myip_str[i]);
							if((i==2) || (i==5) || (i==8))
								SEND_CHAR('.');
						}

						SEND_CHAR(temp_myip_str[count_ip]);

						for(uint8_t i=10; i!=0; i--)
						{
							_delay_ms(100);
							if(BUT1_ON)
							{
		
								But1_ON();
								temp_myip_str[count_ip]++;
							}

							if(BUT2_ON)
							{
								_delay_ms(100);
								if(BUT2_ON)
								{
									But2_ON();
									count_ip++;	
								}
							}
						}

					
						if(temp_myip_str[count_ip]>'9') temp_myip_str[count_ip]='0';
						if((temp_myip_str[count_ip]>'2') && ((count_ip==0) || (count_ip==3) || (count_ip==6) || (count_ip==9))) temp_myip_str[count_ip]='0';

						eeprom_update_byte((ADDR_MYIP_EEP+count_ip), temp_myip_str[count_ip]);
					}
					

				} //end while(end_ip).
					
			KCount=10;
			CountMenu=59;
			break;
		} //end edit IP Address.


		case 69: {			//UDP
			LCD_Button("UDP     ETHERNET", "    ENTER SELECT");
			KCount=10;
			break;
		}

		case 68: {			//UDP PORT.
			LCD_Button("        UDP PORT", "      EDIT  EXIT");
			KCount=10;
			break;
		}
		
		case 67: {			//UDP PORT.
			
			break;
		}

		case 78: {			//UDP PORT EXIT.
			KCount=10;
			CountMenu=69;
			break;
		}

	} //end switch(CountMenu).

	return KCount;
}


/////////////////////////////////////////////////////////////////////////



int main (void)
{
	init();

	Init_Def();
	usart0_mod_init(BaudRate);				//Init UART.


	Init_My_IP(myip, myip_str);

	TWIOpen(0x40);				//Init I2C.

	//LCD initialisation 
	LCD_Ini(); 					     
	SEND_CMD(DISP_ON);
	SEND_CMD(CLR_DISP); 

  	Enc28j60Init(mymac);    //initialize enc28j60
    _delay_ms(10);
    InitPhy();
    Init_ip_arp_udp_tcp(mymac,myip,TCP_PORT);  //init the ethernet/ip layer

	
	uint16_t KCount = 10;
	sei();

	while(1)
	{
		
		timeUpdate = false;

		if(CountMenu) STOP_SYS_TIMER; else START_SYS_TIMER;
		
		if(BUT1_ON)
		{
			_delay_ms(100);
			if(BUT1_ON)
			{
		
				But1_ON();
				CountMenu+=KCount;
			}
		}

		if((BUT2_ON) && (CountMenu))
		{
			_delay_ms(100);
			if(BUT2_ON)
			{
				But2_ON();
				CountMenu--;
			}
		}

		KCount = DisplayMenu();

		

	} // End while.

}	//End main.



// INTERRUP //

// Interrupt Ext. Int0 //
ISR(INT0_vect)
{

   LanTask();

}
//////////////////////////

// Ivterrupt Timer1 COMPA //
ISR(TIMER1_COMPA_vect)
{
	TCNT1=0x0000;	//Reset Timer1.
	
	char temp_time[8];

	GetTimeRTC(temp_time);

	SEND_CMD(DD_RAM_ADDR1);

	SEND_CHAR(0x20);
	for(uint8_t i=0; i<sizeof(temp_time); i++)
		SEND_CHAR(temp_time[i]);

	for(uint8_t i=0; i<7; i++)
		SEND_CHAR(0x20);

	SEND_CMD(DD_RAM_ADDR2);

	for(uint8_t i=0; i<12; i++)
		SEND_CHAR(0x20);
	
	char menu[]="MENU";

	for(uint8_t i=0; i<(sizeof(menu)-1); i++)
		SEND_CHAR(menu[i]);

	PORT_INF^=LED_RUN;

}
