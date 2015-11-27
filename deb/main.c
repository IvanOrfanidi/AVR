
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

	typedef struct ATcommand{
		char at[2];
		int ok;
		char err[6];

	} at_cmd;

	at_cmd *prtPacket;

int main()
{
	char t[7];

	memcpy(t, "AT", sizeof("AT"));

	uint16_t i=sizeof(prtPacket->err);

	for(uint8_t i=0; i<2; i++)	prtPacket->at[i]=t[i];

	prtPacket->ok=i;


return 0;
}
