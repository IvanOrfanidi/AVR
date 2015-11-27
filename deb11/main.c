


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

	char buf_eth_http[50];

	char myip_str[]="195.168.1.10";
static const char http[]="<a href=\"http://";


int main()
{
	buf_eth_http[0]=0;
	char text_http[]="\">REFRECH</a><hr>";

	strcat(buf_eth_http, http);
	strcat(buf_eth_http, myip_str);
	strcat(buf_eth_http, text_http);

	

 	char text_info[]="/LED1_1\">яберндхнд ╧1 ON</a><hr>";

	buf_eth_http[(strlen(text_http)+strlen(myip_str))-1]=0;
	strcat(buf_eth_http, text_info);



}
