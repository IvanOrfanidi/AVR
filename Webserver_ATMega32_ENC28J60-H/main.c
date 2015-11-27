

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
	#include "ATCMD.h"

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

	uint8_t myip_eep[12]EEMEM = {'1','9','2','1','6','8','0','0','1','0','3','0'};

//**************************************************************************************

// please modify the following two lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
char mymac_char[]="0x00,0x21,0x68,0x01,0x00,0x49";
uint8_t mymac[6] = {0x00,0x21,0x68,0x01,0x00,0x49}; 
uint8_t myip[4];
char myip_str[]="XXXxXXXxXXXxXXX";

static uint16_t mywwwport = 80; // listen port for tcp/www (max range 1-254)

#define BUFFER_SIZE 512

static uint8_t buf[BUFFER_SIZE+1];
//char timeString[30] = "Time since reset: ";
bool timeUpdate = false;

bool countUp0 = true;
bool countUp1 = true;
bool countUp2 = true;
bool countUp3 = true;
bool countUp4 = true;


#define BUFFER_HTTP_TEXT 64
char buf_eth_http[BUFFER_HTTP_TEXT];
static const char http[]="<a href=\"http://";


//**************************************************************************************

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

//////////////////////////////////////////////////////////////////////


uint8_t GetCommand (char *x)
{
   if (strncmp("/ ", x, 2) == 0) return 0;
   if (strncmp("/LED1_1", x, 7) == 0) return 1;
   if (strncmp("/LED1_0", x, 7) == 0) return 2;
   if (strncmp("/LED2_1", x, 7) == 0) return 3;
   if (strncmp("/LED2_0", x, 7) == 0) return 4;
   if (strncmp("/LED3_1", x, 7) == 0) return 5;
   if (strncmp("/LED3_0", x, 7) == 0) return 6;
   if (strncmp("/LED4_1", x, 7) == 0) return 7;
   if (strncmp("/LED4_0", x, 7) == 0) return 8;
   if (strncmp("/BUZ_0", x, 6) == 0) return 9;
   if (strncmp("/BUZ_1", x, 6) == 0) return 10;

   return 0;
}


//**************************************************************************************
uint16_t PrintWebpage (uint8_t *buf)
{

   char DataString[]="";
  
   uint16_t plen;
   plen = Fill_tcp_data_p(buf, 0, PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
   plen = Fill_tcp_data_p(buf, plen, PSTR("<center><h1>Welcome to WWW.Home #1!!!<hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("Author Ivan Orfanidi.<hr>"));
   plen = Fill_tcp_data_p(buf,plen,PSTR("BUTTON: "));

	for(uint8_t i=1; i<5; i++)
	{
		plen = Fill_tcp_data_p(buf,plen,PSTR(" "));
		plen = Fill_tcp_data_p(buf,plen,PSTR("S"));
		sprintf(DataString, "%d", i);
   		plen = Fill_tcp_data  (buf, plen, DataString);


		if(!(PIN_IN & (1<<(i+3))))
		{
			plen = Fill_tcp_data_p(buf,plen,PSTR(" = 1"));
		}
		else
		{
			plen = Fill_tcp_data_p(buf,plen,PSTR(" = 0"));
		}
		
	}

   plen = Fill_tcp_data_p(buf,plen,PSTR(".<hr>"));



	buf_eth_http[0]=0;
	char text_http[]="\">REFRESH</a><hr>";

	strcat(buf_eth_http, http);
	strcat(buf_eth_http, myip_str);
	strcat(buf_eth_http, text_http);

	uint8_t Len_URL=((strlen(text_http)+strlen(myip_str))-1);

   plen = Fill_tcp_data(buf,plen,buf_eth_http);

   if (countUp0)
   {
   	    PORT_OUT&= ~LED1;

   		char text_info[]="/LED1_1\">LED #1 ON</a><hr>";

		buf_eth_http[Len_URL]=0;
		strcat(buf_eth_http, text_info);

	  plen = Fill_tcp_data(buf,plen,buf_eth_http);
      //plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.1.10/LED1_1\">яберндхнд ╧1 ON</a><hr>"));
   }
   else
   {
   	    PORT_OUT|= LED1;

		char text_info[]="/LED1_0\">LED #1 OFF</a><hr>";

		buf_eth_http[Len_URL]=0;
		strcat(buf_eth_http, text_info);

	  plen = Fill_tcp_data(buf,plen,buf_eth_http);
      //plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.1.10/LED1_0\">яберндхнд ╧1 OFF</a><hr>"));
   }

   if (countUp1)
   {
   		char text_info[]="/LED2_1\">LED #2 ON</a><hr>";

		buf_eth_http[Len_URL]=0;
		strcat(buf_eth_http, text_info);

	  plen = Fill_tcp_data(buf,plen,buf_eth_http);

      //plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.1.10/LED2_1\">яберндхнд ╧2 ON</a><hr>"));
	  PORT_OUT&= ~LED2;
   }
   else
   {


   		char text_info[]="/LED2_0\">LED #2 OFF</a><hr>";

		buf_eth_http[Len_URL]=0;
		strcat(buf_eth_http, text_info);

	  plen = Fill_tcp_data(buf,plen,buf_eth_http);

      //plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.1.10/LED2_0\">яберндхнд ╧2 OFF</a><hr>"));
	  PORT_OUT|= LED2;

	}

   if (countUp2)
   {
   		char text_info[]="/LED3_1\">LED #3 ON</a><hr>";

		buf_eth_http[Len_URL]=0;
		strcat(buf_eth_http, text_info);

	  plen = Fill_tcp_data(buf,plen,buf_eth_http);
      //plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.1.10/LED3_1\">яберндхнд ╧3 ON</a><hr>"));
	  PORT_OUT&= ~LED3;
   }
   else
   {

   		char text_info[]="/LED3_0\">LED #3 OFF</a><hr>";

		buf_eth_http[Len_URL]=0;
		strcat(buf_eth_http, text_info);

	  plen = Fill_tcp_data(buf,plen,buf_eth_http);
      //plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.1.10/LED3_0\">яберндхнд ╧3 OFF</a><hr>"));
	  PORT_OUT|= LED3;

	}

   if (countUp3)
   {

   		char text_info[]="/LED4_1\">LED #4 ON</a><hr>";

		buf_eth_http[Len_URL]=0;
		strcat(buf_eth_http, text_info);

	  plen = Fill_tcp_data(buf,plen,buf_eth_http);
      //plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.1.10/LED4_1\">яберндхнд ╧4 ON</a><hr>"));
	  PORT_OUT&= ~LED4;
   }
   else
   {
   		char text_info[]="/LED4_0\">LED #4 OFF</a><hr>";

		buf_eth_http[Len_URL]=0;
		strcat(buf_eth_http, text_info);

	  plen = Fill_tcp_data(buf,plen,buf_eth_http);
      //plen = Fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://192.168.1.10/LED4_0\">яберндхнд ╧4 OFF</a><hr>"));
	  PORT_OUT|= LED4;

	}

   if (countUp4)
   {
   		char text_info[]="/BUZ_1\">BUZZER ON</a><hr>";

		buf_eth_http[Len_URL]=0;
		strcat(buf_eth_http, text_info);

	  plen = Fill_tcp_data(buf,plen,buf_eth_http);

	  PORT_BUZ&= ~BUZ;
   }
   else
   {
   		char text_info[]="/BUZ_0\">BUZZER OFF</a><hr>";

		buf_eth_http[Len_URL]=0;
		strcat(buf_eth_http, text_info);

	  plen = Fill_tcp_data(buf,plen,buf_eth_http);

	  PORT_BUZ|= BUZ;

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
         if (strncmp("GET ",(char *) & (buf[dat_p]), 4) != 0)			//GET type.
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
			  case 5:
                  countUp2 = false;
                  break;
               case 6:
                  countUp2 = true;
                  break;
				case 7:
                  countUp3 = false;
                  break;
               case 8:
                  countUp3 = true;
                  break;
				case 9:
                  countUp4 = true;
                  break;
               case 10:
                  countUp4 = false;
                  break;
            }
            plen = PrintWebpage(buf);
            send = true;
         }

         Make_tcp_ack_from_any(buf); // send ack for http get
         Make_tcp_ack_with_data(buf, plen); // send data       
      }
   }
}

//*****************************************************************************
int main (void)
{
	init();
	
	Init_My_IP(myip, myip_str);

	usart0_init();				//Init UART.

    Enc28j60Init(mymac);    //initialize enc28j60
    _delay_ms(10);
    InitPhy();
    Init_ip_arp_udp_tcp(mymac,myip,80);  //init the ethernet/ip layer


	uint8_t req;


    sei();            // enable global interrupt

   while (1)
   {

   	 timeUpdate = false;      

	  if(PC_CONNECT)
	  {
	  	PORT_LED|= LED_RUN;

		req=atcommand();

		if(req==254)
		{
			char ver[]="1.01";
			usart0_write(':'); usart0_write(' ');
			usart0_write_str(ver);
			usart0_write(0x0D); 	usart0_write(0x0A);
		}

		if(req==251)
			PORT_BUZ|= BUZ;

		if(req==250)
			PORT_BUZ&= ~BUZ;

		if(req==249)
			PORT_LED|= LED_ERR;

		if(req==248)
			PORT_LED&= ~LED_ERR;


		if(req==247)
		{
			usart0_write(':'); usart0_write(' ');		
			usart0_write(((PIND & BUZ) && 1) + 0x30);
			usart0_write(0x0D); 	usart0_write(0x0A);

		}

		if(req==246)
		{
			usart0_write(':'); usart0_write(' ');		
			usart0_write(((PORT_LED & LED_ERR) && 1) + 0x30);
			usart0_write(0x0D); 	usart0_write(0x0A);
		}

		if(req==244)
		{
			usart0_write(':'); usart0_write(' ');		
			usart0_write_str(mymac_char);
			usart0_write(0x0D); 	usart0_write(0x0A);
		}

		if(req==243)
		{
			usart0_write(':'); usart0_write(' ');		
			usart0_write(((PIN_IN & 0b00001111) && 1) + 0x30);
			usart0_write(0x0D); 	usart0_write(0x0A);
		}

		if(req==242)
		{
			PORT_OUT|= 0b00001111;
		}

		if(req==241)
		{
			PORT_OUT&= ~(0b00001111);
		}

	  }
	  else
	  {
	  	PORT_LED&= ~LED_RUN;
	  }


   }	//End while(1).

}	//End main().


/******************************************************************************/


//*****************************************************************************
ISR(INT0_vect)
{
   LanTask();
}
