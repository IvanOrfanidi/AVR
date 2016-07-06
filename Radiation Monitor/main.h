


// Standard Device functions.
    #include <ioavr.h>
    #include <intrinsics.h>

// Standard Input/Output functions.
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include <ctype.h>
    #include <pgmspace.h>
    #include <inavr.h> 
    #include <stdbool.h> 

    //#include "USART.h"

    #include "lcd_lib.h"

    #define   F_CPU 14745600UL

    #define   _delay_ms(temp)(__delay_cycles((temp * F_CPU)/1000));
    #define   _delay_us(temp)(__delay_cycles((temp * F_CPU)/1000000UL));

    #define ClearBit(reg, bit)       reg &= (~(1<<(bit)))
    #define SetBit(reg, bit)         reg |= (1<<(bit))	

