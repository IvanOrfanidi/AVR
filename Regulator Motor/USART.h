#ifndef USART_H
#define USART_H

        #include <ioavr.h>
        #include <intrinsics.h>
        // Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
        #include <ctype.h>
        #include <stdint.h>

	#include "main.h"

	#define RX_BUFFER_SIZE0 1
	#define TX_BUFFER_SIZE0 64

	#define BAUD_RATE0 9600
	#define u2x0		0
	#define CHARTER_SIZE0   8
	#define PARITY_MODE0    'N'
	#define STOP_BIT0       1

	#define USART0_DELAY_MS 1

	#define UPE0 2
	#define DOR0 3
	#define FE0 4


	void usart0_init();

	void usart0_mod_init(uint8_t baud_rate);

	void usart0_clear_tx_buffer();

	void usart0_clear_rx_buffer();

	void usart0_write(uint8_t byte);

	void usart0_write_str(char *pData_Usart0);

	uint8_t usart0_read();

	uint8_t usart0_rx_len();

	uint8_t rx_error0();

	uint8_t usart0_busy_wait(void);

	void usart_init0();

#endif
