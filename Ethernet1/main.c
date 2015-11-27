

//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <avr/eeprom.h>
	#include <avr/wdt.h>
	#include <util/delay.h>
	#include <util/crc16.h>

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>

	#include "enc28j60.h"
	#include "ip_arp_udp_tcp.h"
	#include "net.h"


	#define true      1
	#define false     0
	#define bool      _Bool

	#define RELAY		(1<<PB0)
	#define PORT_OUT 	PORTB
	#define PIN_IN		PINB


	#define BUZ			(1<<PD6)
	#define PORT_BUZ	PORTD

	#define LED_RUN		(1<<PC1)
	#define LED_GND 	(1<<PC3)
	#define PORT_LED		PORTC

	#define CS_SPI_ENC28J60		(1<<PC4)

//**************************************************************************************

// please modify the following two lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
static uint8_t mymac[6] = {0x00,0x21,0x68,0x33,0xD6,0x30}; 
static uint8_t myip[4] = {192,168,1,14};
char myip_str[]="192.168.1.14";

//// listen port for udp
#define UDP_PORT 1000
#define TCP_PORT 80				//listen port for tcp/www (max range 1-254)

#define BUFFER_SIZE 512
static uint8_t buf[BUFFER_SIZE+1];

bool timeUpdate = false;

bool countUp0 = true;

//**************************************************************************************

void init(void)
{
	//Watchdog initialization //

	//!!!WDTON fuse set to “0“ means programmed and “1“ means unprogrammed!!!//

	cli();
	wdt_reset();		//Watchdog Reset.
	wdt_enable(WDTO_8S);

	////////////////////////////////////////////////////////////////////


   DDRB = (RELAY);
   PORTB = 0b11111110;


   DDRC = (LED_RUN) | (LED_GND) | (CS_SPI_ENC28J60);
   PORTC = (CS_SPI_ENC28J60);


   DDRD = (BUZ);
   PORTD = 0xFF;
   PORTD&= ~(BUZ);

   	////////////////////////////////////////////////////////////////////

	// Init Interrupt //
   EICRA &= 0xFE;       // INT 0 sense on falling edge

   EICRA |= 0x02;
   EIMSK |= (1<<INT0);        // enable interrupt on INT 0

	////////////////////////////////////////////////////////////////////

}


//*****************************************************************************


//*****************************************************************************

uint8_t GetCommand (char *x)
{
   if (strncmp("/ ", x, 2) == 0) return 0;
   if (strncmp("/REL_1", x, 6) == 0) return 1;
   if (strncmp("/REL_0", x, 6) == 0) return 2;
   return 0;
}

//*****************************************************************************


uint16_t PrintWebpage (uint8_t *buf)
{


   uint16_t plen;
   plen = Fill_tcp_data_p(buf, 0, PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
   plen = Fill_tcp_data_p(buf, plen, PSTR("<center><h1>Welcome Home!<hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("Ivan Orfanidi.<hr>"));

	char http_start[]="<a href=\"http://";

   if (countUp0)
   {

   		PORT_OUT&= ~RELAY;

		char text_info[]="/REL_1\">RELAY #1 OFF</a><hr>";

	    plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,myip_str);
		plen = Fill_tcp_data(buf,plen,text_info);

   }
	else
	{
		PORT_OUT|= RELAY;

		char text_info[]="/REL_0\">RELAY #1 ON</a><hr>";

		plen = Fill_tcp_data(buf,plen,http_start);
		plen = Fill_tcp_data(buf,plen,myip_str);
		plen = Fill_tcp_data(buf,plen,text_info);
	}

 return plen;

}

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


//*****************************************************************************
int main (void)
{
	init();
	wdt_reset();

    Enc28j60Init(mymac);    //initialize enc28j60
	wdt_reset();
    _delay_ms(10);
	
	InitPhy();
	wdt_reset();
	
    Init_ip_arp_udp_tcp(mymac,myip,TCP_PORT);  //init the ethernet/ip layer
	wdt_reset();
	
	PORT_LED|= LED_RUN;


    sei();            // enable interrupt

   while (1)
   {

		timeUpdate = false;
		      
		
		wdt_reset();


   }	//End while(1).

}	//End main().


/******************************************************************************/



// INTERRUP //

// Interrupt Ext. Int0 //
ISR(INT0_vect)
{

	LanTask();
   
}
//////////////////////////




