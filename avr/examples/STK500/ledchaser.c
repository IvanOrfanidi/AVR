/**************************************************
 *
 * This is a LED chaser example program for the AVR
 * STK500 starter kit evaluation board. This program
 * works for all AVRs.
 *
 * Description:
 * This program will turn on the User LEDs one at
 * a time and circulate this procedure in a forever
 * loop.
 *
 * Copyright 1996-2005,2008 IAR Systems AB. All rights reserved.
 *
 * $Revision: 6759 $
 *
 **************************************************/


/* Include I/O definition file */
#include <ioavr.h>
#include <intrinsics.h>

int main( void )
{
    unsigned char  led;

    PORTB   = 0xFF; /* Give PORTB and the User LEDs an initial startvalue */

    led     = 1;    /* Start with LED0 */

    DDRB    = 0xFF; /* Set PORTB as output */

    while(1)        /* Eternal loop */
    {
      PORTB = ~led;   /* Invert the output since a zero means: LED on */
      led <<= 1;      /* Move to next LED by performing a rotate left */

        if (!led)
        {
          led = 1;      /* If overflow: start with LED0 again */
        }

        __delay_cycles(500000);  /* Wait for 500 000 cycles */
    }
}
