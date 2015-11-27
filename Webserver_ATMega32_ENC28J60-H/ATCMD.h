
#ifndef ATCMD_H
#define ATCMD_H


	#include <avr/pgmspace.h>

	#define BUF_SIZE_AT 24

	#define BACKSPASE	0x08
	#define ENTER		0x0D
	
	#define ADDR_MYIP_EEP 0x0000

	uint8_t atcommand(void);

	void atcmd_init(void);

	enum{
		OK, INC, DEC, ERROR, NOT
	};

#endif
