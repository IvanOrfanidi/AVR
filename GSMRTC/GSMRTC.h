

	#include "USART.h"
	#include <ctype.h>

	#define	USART_NUM_RTC 0

	uint8_t GetDateTimeGSM(char *pDateTime);

	uint8_t SetDateTimeGSM(const char *pDateTime);

	uint8_t GetTimeGSM(char *pTime);

	uint8_t SetTimeGSM(const char *pTime);
