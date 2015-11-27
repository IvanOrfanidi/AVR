
// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <util/delay.h>

	#define USART_DELAY		10
	#define USART_TIMEOUT	30000

	void at(void);

	int while_usart(char *pDataUsart);
