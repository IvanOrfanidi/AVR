

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
	#include <util/delay.h>
	#include "USART.h"


	#define true      1
	#define false     0
	#define bool      _Bool


	#define BUZ		(1<<PD7)
	#define PORT_BUZ	PORTD

	#define LED1	(1<<PA0)
	#define LED2	(1<<PA1)
	#define LED3	(1<<PA2)
	#define LED4	(1<<PA3)

	#define BUT1	(1<<PA4)
	#define BUT2	(1<<PA5)
	#define BUT3	(1<<PA6)
	#define BUT4	(1<<PA7)

	#define PORT_OUT	PORTA
	#define PIN_IN		PINA


	#define LED_ERR	(1<<PB0)
	#define LED_RUN (1<<PB1)
	#define CS_SPI_ENC28J60		(1<<PB2)
	#define PORT_LED		PORTB
	
	#define PC_CONNECT		(PIND & (1<<PD6))

	#define CTS				(1<<PD5)		//Port OUTPUT.
	#define DTR				(1<<PD4)		//Port INPUT.
	#define RTS				(1<<PD3)		//Port INPUT.
	#define PIN_RS232		PIND
	#define PORT_RS232		PORTD


//**************************************************************************************

// please modify the following two lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
static uint8_t mymac[6] = {0x00,0x21,0x68,0xFF,0xA1,0x30}; 
static uint8_t myip[4] = {192,168,1,20};
static uint16_t mywwwport = 80; // listen port for tcp/www (max range 1-254)

#define BUFFER_SIZE 512

//// listen port for udp
#define MYUDPPORT 1200

static uint8_t buf[BUFFER_SIZE+1];

bool timeUpdate = false;

bool countUp0 = true;

//**************************************************************************************


uint8_t GetCommand (char *x)
{
   if (strncmp("/ ", x, 2) == 0) return 0;
   if (strncmp("/LED_0", x, 6) == 0) return 1;
   if (strncmp("/LED_1", x, 6) == 0) return 2;

   return 0;
}


//**************************************************************************************

//**************************************************************************************
uint16_t PrintWebpage (uint8_t *buf)
{


   uint16_t plen;
   plen = Fill_tcp_data_p(buf, 0, PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
   plen = Fill_tcp_data_p(buf, plen, PSTR("<center><h1>Welcome to WWW.Home #1!!!<hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("Author Ivan Orfanidi.<hr>"));



   plen = Fill_tcp_data_p(buf,plen,PSTR("<hr><a href=\"http://192.168.1.20\">REFRESH</a><hr>"));



   if (countUp0)
   {
   		PORT_OUT&= ~LED1;


		plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.1.20/LED_0\">LED ON</a><hr>"));

   }
	else
	{
		PORT_OUT|= LED1;


		plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.1.20/LED_1\">LED OFF</a><hr>"));

	}


   return plen;

}

/******************************************************************************/

//**************************************************************************************

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
      if ((buf[IP_PROTO_P] == IP_PROTO_TCP_V) && (buf[TCP_DST_PORT_H_P] == 0) && (buf[TCP_DST_PORT_L_P] == mywwwport))
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
		 continue;       
      }


		// udp interface:
        if (buf[IP_PROTO_P]==IP_PROTO_UDP_V)
		{
        	uint8_t payloadlen=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;
            // the received command has to start with t and be 4 char long
            // e.g "test\0"
            if(payloadlen)
			{
				char msg1[]="Hello";
				Make_udp_reply_from_request(buf,msg1,sizeof(msg1),MYUDPPORT);
			}
         }// End udp interface.

   }// End while(1)

}// End LanTask

//*****************************************************************************



void init(void)
{

   DDRA = LED1 | LED2 | LED3 | LED4;         // all output high
   PORTA = BUT1 | BUT2 | BUT3 | BUT4;

   DDRB = LED_ERR | LED_RUN | CS_SPI_ENC28J60;
   PORTB = 0xFF;
   PORTB&= ~(LED_ERR);

   DDRC = 0x00;
   PORTC = 0xFF;

   DDRD = CTS | BUZ;
   PORTD = 0xFF;
   PORTD&= ~(BUZ);

   MCUCR &= 0xFE;       // INT 0 sense on falling edge
   MCUCR |= 0x02;
   GICR |= 0x40;        // enable interrupt on INT 0

	cli();
	wdt_reset();
	wdt_disable();
}

////////////////////////////////////////////////////////


int main (void)
{
	init();
	
	usart0_init();				//Init UART.

    Enc28j60Init(mymac);    //initialize enc28j60
    _delay_ms(10);
    InitPhy();
    Init_ip_arp_udp_tcp(mymac,myip,80);  //init the ethernet/ip layer
	
	PORTB&= ~(LED_RUN);

	sei();
	
	while(1)
	{
		timeUpdate = false;
	}

}
////////////////////////////////////////////////////////////////////////////////////////////


// INTERRUPT //

// Interrupt Ext. INT0 //
ISR(INT0_vect)
{
   LanTask();
}
///////////////////
