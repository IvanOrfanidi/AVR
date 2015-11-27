#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define RX_BUFFER_SIZE0 125
#define TX_BUFFER_SIZE0 300

#define RX_BUFFER_SIZE1 100
#define TX_BUFFER_SIZE1 100

#define BAUD_RATE0 9600
#define u2x0			0
#define CHARTER_SIZE0 8
#define PARITY_MODE0 'N'
#define STOP_BIT0	1


#define BAUD_RATE1 19200
#define u2x1			0
#define CHARTER_SIZE1 8
#define PARITY_MODE1 'N'
#define STOP_BIT1	1


#define UPE 2
#define DOR 3
#define FE 4


#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)

uint8_t rx_status1;
uint8_t tx_status1;

uint8_t rx_status0;
uint8_t tx_status0;

void usart0_init();
void usart1_init();

void usart0_clear_tx_buffer();
void usart1_clear_tx_buffer();

void usart0_clear_rx_buffer();
void usart1_clear_rx_buffer();

void usart0_write(uint8_t byte);
void usart1_write(uint8_t byte);

uint8_t usart0_read();
uint8_t usart1_read();

uint8_t usart0_rx_len();
uint8_t usart1_rx_len();

uint8_t rx_error0();
uint8_t rx_error1();

