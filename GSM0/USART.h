#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>


	#define RX_BUFFER_SIZE0 400
	#define TX_BUFFER_SIZE0 400

	#define BAUD_RATE0 115200
	#define u2x0			0
	#define CHARTER_SIZE0 8
	#define PARITY_MODE0 'N'
	#define STOP_BIT0	1


	#define RX_BUFFER_SIZE1 256
	#define TX_BUFFER_SIZE1 256

	#define BAUD_RATE1 19200
	#define u2x1			0
	#define CHARTER_SIZE1 8
	#define PARITY_MODE1 'N'
	#define STOP_BIT1	1


	#define UPE0 2
	#define DOR0 3
	#define FE0 4


	uint8_t rx_status0;
	uint8_t tx_status0;

	void usart0_init();

	void usart0_clear_tx_buffer();

	void usart0_clear_rx_buffer();

	void usart0_write(uint8_t byte);

	uint8_t usart0_read();

	uint8_t usart0_rx_len();

	uint8_t rx_error0();


	#if defined (__AVR_ATmega162__)

		uint8_t rx_status1;
		uint8_t tx_status1;

		void usart1_init();

		void usart1_clear_tx_buffer();

		void usart1_clear_rx_buffer();

		void usart1_write(uint8_t byte);

		uint8_t usart1_read();

		uint8_t usart1_rx_len();

		uint8_t rx_error1();

	#endif
