#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>


#define RX_BUFFER_SIZE0	256
#define TX_BUFFER_SIZE0 300


#define BAUD_RATE0 115200
#define u2x0			0
#define CHARTER_SIZE0 8
#define PARITY_MODE0 'N'
#define STOP_BIT0	1


#define UPE 2
#define DOR 3
#define FE 4


#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)


uint8_t rx_status0;
uint8_t tx_status0;


void usart0_init();
void usart0_write(uint8_t byte);
void usart0_clear_rx_buffer();
void usart0_clear_tx_buffer();

uint8_t usart0_read();
uint8_t usart0_rx_len();
uint8_t rx_error0();


