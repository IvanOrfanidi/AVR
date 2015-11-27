
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


//**************************************************************************************

// please modify the following two lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:

//static uint16_t mywwwport = 150; // listen port for tcp/www (max range 1-254)

#define BUFFER_SIZE 700

//// listen port for udp
#define UDP_PORT 1200
#define TCP_PORT 80				//listen port for tcp/www (max range 1-254)

static uint8_t buf[BUFFER_SIZE+1];

bool timeUpdate = false;

bool countUp00 = true;
bool countUp01 = true;
bool countUp10 = true;
bool countUp11 = true;

bool back_countUp00 = false;
bool back_countUp01 = false;
bool back_countUp10 = false;
bool back_countUp11 = false;


bool countUp20 = true;
bool countUp21 = true;

bool back_countUp20 = true;
bool back_countUp21 = true;


bool countUp30 = true;
bool countUp31 = true;

bool back_countUp30 = true;
bool back_countUp31 = true;

uint8_t data_sel[4];


//**************************************************************************************

//**************************************************************************************


//**************************************************************************************


uint8_t GetCommand (char *x)
{
   if (strncmp("/ ", x, 2) == 0) return 0;
   if (strncmp("/OUT00_1", x, 8) == 0) return 1;
   if (strncmp("/OUT00_0", x, 8) == 0) return 2;
   if (strncmp("/OUT01_1", x, 8) == 0) return 3;
   if (strncmp("/OUT01_0", x, 8) == 0) return 4;

   if (strncmp("/OUT10_1", x, 8) == 0) return 5;
   if (strncmp("/OUT10_0", x, 8) == 0) return 6;
   if (strncmp("/OUT11_1", x, 8) == 0) return 7;
   if (strncmp("/OUT11_0", x, 8) == 0) return 8;

   if (strncmp("/OUT20_1", x, 8) == 0) return 9;
   if (strncmp("/OUT20_0", x, 8) == 0) return 10;
   if (strncmp("/OUT21_1", x, 8) == 0) return 11;
   if (strncmp("/OUT21_0", x, 8) == 0) return 12;

   if (strncmp("/OUT30_1", x, 8) == 0) return 13;
   if (strncmp("/OUT30_0", x, 8) == 0) return 14;
   if (strncmp("/OUT31_1", x, 8) == 0) return 15;
   if (strncmp("/OUT31_0", x, 8) == 0) return 16;

   return 0;
}


//**************************************************************************************
uint16_t PrintWebpage (uint8_t *buf)
{


   uint16_t plen;
   plen = Fill_tcp_data_p(buf, 0, PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
   plen = Fill_tcp_data_p(buf, plen, PSTR("<center><h1>Welcome to Home!!!<hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("Author Ivan Orfanidi.<hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("SYSTEM TIME "));

   const char http_start[]="<a href=\"http://192.168.1.250";

   char temp_time[8];

   GetTimeRTC(temp_time);
   plen = Fill_tcp_data_len(buf,plen,temp_time, sizeof(temp_time));
   plen = Fill_tcp_data_p(buf,plen,PSTR("<hr>"));

	char text_http[]="\">REFRESH</a><hr>";

	plen = Fill_tcp_data(buf,plen,http_start);
	plen = Fill_tcp_data(buf,plen,text_http);

///////////////////////////////////////////////////////////////////////////////

   if (countUp00)
   {
   		if(back_countUp00!=countUp00)
			data_sel[0]=0x0F;

		char text_info[]="/OUT00_1\">RELAY #1 ON</a><hr>";

		plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp00=countUp00;

   }
	else
	{
		if(back_countUp00!=countUp00)
			data_sel[0]=0x0F;

		char text_info[]="/OUT00_0\">RELAY #1 ON</a><hr>";

	    plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp00=countUp00;
	}

   if (countUp01)
   {
  		if(back_countUp01!=countUp01)
			data_sel[0]=0xF0;

		char text_info[]="/OUT01_1\">RELAY #1 OFF</a><hr>";

		plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp01=countUp01;   
	}
	else
	{
  		if(back_countUp01!=countUp01)
			data_sel[0]=0xF0;

		char text_info[]="/OUT01_0\">RELAY #1 OFF</a><hr>";

	    plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp01=countUp01;

	}

///////////////////////////////////////////////////////////////////////////////

   if (countUp10)
   {
   		if(back_countUp10!=countUp10)
			data_sel[1]=0x0F;

		char text_info[]="/OUT10_1\">RELAY #2 ON</a><hr>";

		plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp10=countUp10;

   }
	else
	{
		if(back_countUp10!=countUp10)
			data_sel[1]=0x0F;

		char text_info[]="/OUT10_0\">RELAY #2 ON</a><hr>";

	    plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp10=countUp10;
	}

   if (countUp11)
   {
  		if(back_countUp11!=countUp11)
			data_sel[1]=0xF0;

		char text_info[]="/OUT11_1\">RELAY #2 OFF</a><hr>";

		plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp11=countUp11;   
	}
	else
	{
  		if(back_countUp11!=countUp11)
			data_sel[1]=0xF0;

		char text_info[]="/OUT11_0\">RELAY #2 OFF</a><hr>";

	    plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp11=countUp11;

	}


///////////////////////////////////////////////////////////////////////////////

   if (countUp20)
   {
   		if(back_countUp20!=countUp20)
			data_sel[2]=0x0F;

		char text_info[]="/OUT20_1\">Unit 1 ON</a><hr>";

		plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp20=countUp20;

   }
	else
	{
		if(back_countUp20!=countUp20)
			data_sel[2]=0x0F;

		char text_info[]="/OUT20_0\">Unit 1 ON</a><hr>";

	    plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp20=countUp20;
	}

   if (countUp21)
   {
  		if(back_countUp21!=countUp21)
			data_sel[2]=0xF0;

		char text_info[]="/OUT21_1\">Unit 1 OFF</a><hr>";

		plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp21=countUp21;   
	}
	else
	{
  		if(back_countUp21!=countUp21)
			data_sel[2]=0xF0;

		char text_info[]="/OUT21_0\">Unit 1 OFF</a><hr>";

	    plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp21=countUp21;

	}

///////////////////////////////////////////////////////////////////////////////


   if (countUp30)
   {
   		if(back_countUp30!=countUp30)
			data_sel[3]=0x0F;

		char text_info[]="/OUT30_1\">Unit 2 ON</a><hr>";

		plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp30=countUp30;

   }
	else
	{
		if(back_countUp30!=countUp30)
			data_sel[3]=0x0F;

		char text_info[]="/OUT30_0\">Unit 2 ON</a><hr>";

	    plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp30=countUp30;
	}

   if (countUp31)
   {
  		if(back_countUp31!=countUp31)
			data_sel[3]=0xF0;

		char text_info[]="/OUT31_1\">Unit 2 OFF</a><hr>";

		plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp31=countUp31;   
	}
	else
	{
  		if(back_countUp31!=countUp31)
			data_sel[3]=0xF0;

		char text_info[]="/OUT31_0\">Unit 2 OFF</a><hr>";

	    plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,text_info);

		back_countUp31=countUp31;

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
                  countUp00 = false;
                  break;
			   case 2:
                  countUp00 = true;
                  break;
			   case 3:
                  countUp01 = false;
                  break;
			   case 4:
                  countUp01 = true;
                  break;
			   case 5:
                  countUp10 = false;
                  break;
			   case 6:
                  countUp10 = true;
                  break;
			   case 7:
                  countUp11 = false;
                  break;
			   case 8:
                  countUp11 = true;
                  break;
			   case 9:
                  countUp20 = false;
                  break;
			   case 10:
                  countUp20 = true;
                  break;
			   case 11:
                  countUp21 = false;
                  break;
			   case 12:
                  countUp21 = true;
                  break;
			   case 13:
                  countUp30 = false;
                  break;
			   case 14:
                  countUp30 = true;
                  break;
			   case 15:
                  countUp31 = false;
                  break;
			   case 16:
                  countUp31 = true;
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

	OCR1A=14400;		//Interrupt Timer 1 sec.

	TIMSK=(1<<OCIE1A);	//Interrupt Timer1 COMPA.

	////////////////////////////////////////////////////////////////////

}

void Init_Ethernet(void)
{
	uint8_t mymac[6] = {0x00,0x2F,0x68,0x12,0xAC,0x30}; 
	uint8_t myip[4] = {192, 168, 1, 250};

  	Enc28j60Init(mymac);    //initialize enc28j60.
    _delay_ms(10);

    InitPhy();				//config enc28j60.

    Init_ip_arp_udp_tcp(mymac,myip,TCP_PORT);  //init the ethernet/ip layer

}
/////////////////////////////////////////////////////////////////////////
/*
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
*/
/////////////////////////////////////////////////////////////////

void Inquiry_External_Periphery(void)
{


	if(data_sel[2]==0x0F)
	{
		char msg[]="Hello1";

		for(uint8_t i=0; i<sizeof(msg); i++)
			usart0_write(msg[i]);

		data_sel[2]=0x00;
	}
	
	if(data_sel[2]==0xF0)
	{
		char msg[]="World1";

		for(uint8_t i=0; i<sizeof(msg); i++)
		usart0_write(msg[i]);

		data_sel[2]=0x00;
	}



	if(data_sel[3]==0x0F)
	{
		char msg[]="Hello2";

		for(uint8_t i=0; i<sizeof(msg); i++)
			usart0_write(msg[i]);

		data_sel[3]=0x00;
	}
	
	if(data_sel[3]==0xF0)
	{
		char msg[]="World2";

		for(uint8_t i=0; i<sizeof(msg); i++)
		usart0_write(msg[i]);

		data_sel[3]=0x00;
	}

	void usart0_clear_tx_buffer();
//	void usart0_clear_rx_buffer();


}

/////////////////////////////////////////////////////////////////////////

void Inquiry_Inner_Periphery(void)
{
	if(data_sel[0]==0x0F)
	{
		PORT_OUT|=OUT1;
		data_sel[0]=0x00;
	}
	
	if(data_sel[0]==0xF0)
	{
		PORT_OUT&=~OUT1;
		data_sel[0]=0x00;
	}
	
	if(data_sel[1]==0x0F)
	{
		PORT_OUT|=OUT2;
		data_sel[1]=0x00;
	}
	
	if(data_sel[1]==0xF0)
	{
		PORT_OUT&=~OUT2;
		data_sel[1]=0x00;
	}

}

/////////////////////////////////////////////////////////////////////////


int main (void)
{
	init();

	usart0_init();

	TWIOpen(0x40);				//Init I2C.

	//LCD initialisation 
	LCD_Ini(); 					     
	SEND_CMD(DISP_ON);
	SEND_CMD(CLR_DISP); 

	Init_Ethernet();

	sei();

	while(1)
	{

		timeUpdate = false;

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

	sei();

	Inquiry_Inner_Periphery();

	Inquiry_External_Periphery();

	char temp_time[8];

	GetTimeRTC(temp_time);

	SEND_CMD(DD_RAM_ADDR1);

	SEND_CHAR(0x20);
	for(uint8_t i=0; i<sizeof(temp_time); i++)
		SEND_CHAR(temp_time[i]);

	for(uint8_t i=0; i<7; i++)
		SEND_CHAR(0x20);

	SEND_CMD(DD_RAM_ADDR2);

	for(uint8_t i=0; i<16; i++)
		SEND_CHAR(0x20);
	
	PORT_INF^=LED_RUN;
}
