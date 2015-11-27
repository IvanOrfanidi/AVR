/* Name: uart.c
 * Project: AVR USB driver for CDC interface on Low-Speed USB
 * Author: Osamu Tamura
 * Creation Date: 2006-06-18
 * Tabsize: 4
 * Copyright: (c) 2006 by Recursion Co., Ltd.
 * License: Proprietary, free under certain conditions. See Documentation.
 *
 * 2006-07-08   adapted to higher baudrate by T.Kitazawa
 */
/*
General Description:
    This module implements the UART rx/tx system of the USB-CDC driver.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>   /* needed by usbdrv.h */
#include "oddebug.h"
#include "usbdrv.h"
#include "uart.h"

extern uchar    sendEmptyFrame;

#define TX_RX_BUF_PROG_SIZE 21

#define OFF 0xFF
#define ON 0x00
#define CMD_CONNECT	0xF0
#define CMD_CONFIG	0xFA


uint8_t irptr_prog = 0;
uint8_t tx_buf_prog[TX_RX_BUF_PROG_SIZE];
uint8_t rx_buf_prog[TX_RX_BUF_PROG_SIZE];
uint8_t Prog_True = 0;
uint8_t Answer_Byte_Count = OFF;

/* UART buffer */
uchar    urptr, uwptr, irptr, iwptr;
uchar    rx_buf[RX_SIZE+HW_CDC_BULK_IN_SIZE], tx_buf[TX_SIZE];


void uartInit(ulong baudrate, uchar parity, uchar stopbits, uchar databits)
{
usbDWord_t   br;

    br.dword = ((F_CPU>>3)+(baudrate>>1)) / baudrate - 1;
	UCSR0A  |= (1<<U2X0);

#if DEBUG_LEVEL < 1
    /*    USART configuration    */
    UCSR0B  = 0;
    UCSR0C  = URSEL_MASK | ((parity==1? 3:parity)<<UPM00) | ((stopbits>>1)<<USBS0) | ((databits-5)<<UCSZ00);
    UBRR0L  = br.bytes[0];
    UBRR0H  = br.bytes[1];
#else
    DBG1(0xf0, br.bytes, 2);
#endif /* DEBUG_LEVEL */

    UCSR0B  = (1<<RXEN0) | (1<<TXEN0);


#ifdef UART_INVERT
	DDRB	|= (1<<PB1)|(1<<PB0);
	PCMSK1	|= (1<<PCINT9)|(1<<PCINT8);
	PCICR	|= (1<<PCIE1);
#endif
}

void uartPoll(void)
{
	uchar		next;

	/*  device => RS-232C  */
	while(uwptr!=irptr){
        tx_buf_prog[irptr_prog] = tx_buf[irptr];
        irptr   = (irptr+1);
		irptr_prog++;
		
        if( usbAllRequestsAreDisabled() && uartTxBytesFree()>HW_CDC_BULK_OUT_SIZE ) {
            usbEnableAllRequests();
        }
		
		
    }

	if( (irptr_prog>2) && (irptr_prog==(tx_buf_prog[0]+5)) )
	{
		
		if(tx_buf_prog[3])
		{
			config_prog();	
			
		}
			else
			{
				uint8_t len_package = irptr_prog-2;


				if(tx_buf_prog[(tx_buf_prog[0]+4)]==_crc_intelhex_update(tx_buf_prog, (tx_buf_prog[0]+4)))	//Control CRC Intel.
				{
					//PORTB|=(1<<PB0);
					Prog_True=1;
				}
			}
		irptr_prog=0;
	}

	/*  device <= RS-232C  */
	while(Answer_Byte_Count<TX_RX_BUF_PROG_SIZE) 
	{
	    rx_buf[Answer_Byte_Count] = rx_buf_prog[Answer_Byte_Count];
	        
		Answer_Byte_Count++;
		if(Answer_Byte_Count==TX_RX_BUF_PROG_SIZE)
		{
			iwptr = TX_RX_BUF_PROG_SIZE;
			urptr = 0;
			Answer_Byte_Count=OFF;
		}

		PORTB|=(1<<PB0);
			
    }

		PORTB&=~(1<<PB0);

	/*  USB <= device  */
    if( usbInterruptIsReady() && (iwptr!=urptr || sendEmptyFrame) ) {
        uchar   bytesRead, i;

        bytesRead = (iwptr-urptr) & RX_MASK;
        if(bytesRead>HW_CDC_BULK_IN_SIZE)
            bytesRead = HW_CDC_BULK_IN_SIZE;
		next	= urptr + bytesRead;
		if( next>=RX_SIZE ) {
			next &= RX_MASK;
			for( i=0; i<next; i++ )
				rx_buf[RX_SIZE+i]	= rx_buf[i];
		}
        usbSetInterrupt(rx_buf+urptr, bytesRead);
        urptr   = next;
		if( bytesRead )

        /* send an empty block after last data block to indicate transfer end */
        sendEmptyFrame = (bytesRead==HW_CDC_BULK_IN_SIZE && iwptr==urptr)? 1:0;
    }
}


#ifdef UART_INVERT
/*
	enables software-inverter (PC0 -|>o- PB0, PC1 -|>o- PB1)
	to connect to RS-232C line directly. ( <= 2400 bps )
	(atmega48/88 only)
*/
ISR( PCINT1_vect, ISR_NAKED )
{
	asm volatile(
		"out	%0, r16"    	"\n\t"
		"in		r16, __SREG__"	"\n\t"
		"out	%1, r16"    	"\n\t"

		"in		r16, %2"    	"\n\t"
        "com    r16"    		"\n\t"
		"out	%3, r16"    	"\n\t"

		"in		r16, %1"    	"\n\t"
		"out	__SREG__, r16"	"\n\t"
		"in		r16, %0"    	"\n\t"
         :
         : "I" (_SFR_IO_ADDR(GPIOR0)),
           "I" (_SFR_IO_ADDR(GPIOR1)),
           "I" (_SFR_IO_ADDR(PINC)),
           "I" (_SFR_IO_ADDR(PORTB))
        );

	reti();
}
#endif


int _crc_intelhex_update(char *pData_CRC, int Len)
{
	uint8_t crc=0;
	uint8_t i;

	for(i=0; i<Len; i++) 
		crc+=pData_CRC[i];
	
	crc = 0xFF - crc;

	return ++crc;				

}

void config_prog(void)
{

	if(tx_buf_prog[3]==CMD_CONNECT)	// Если запрашивают Connect.
	{	
		uint8_t i;
		for(i=0; i<TX_RX_BUF_PROG_SIZE; i++)
			rx_buf_prog[i]=i+0x30;

		Answer_Byte_Count = ON;
		
	}
	
}
