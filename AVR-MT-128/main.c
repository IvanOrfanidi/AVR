
#include <ioavr.h>
#include <intrinsics.h>

// Standard Input/Output functions.
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <ctype.h>
  #include <ctype.h>
  #include <stdint.h>
  #include <pgmspace.h>
  #include <inavr.h> 
  #include <stdbool.h> 


  #include "lcd_lib.h"



  #define B1		(1<<PA0)
  #define B2		(1<<PA1)
  #define B3		(1<<PA2)
  #define B4		(1<<PA3)
  #define B5		(1<<PA4)

  #define DALLAS 	(1<<PA5)
  #define RELAY 	(1<<PA6)
  
  #define PIN_BUT PINA

void init(void)
{
    // Input/Output Ports initialization

    // Port A initialization


    // Port B initialization

    DDRB=0x00;
    PORTB=0x00;


    // Port C initialization

    DDRC=0xF7;

    PORTC=0x00;

    // Port D initialization

    DDRD=0x08;

    PORTD=0x00;

    // Port E initialization

    DDRE=0x30;

    PORTE=0x00;


    // Port F initialization

    DDRF=0x00;

    PORTF=0x00;

    // Port G initialization

    DDRG=0x00;

    PORTG=0x00;
}

int main()
{
  init();

 LCD_Init(); 
  _delay_ms(100); 
  LCD_Goto(0, 0);
  LCD_WriteCom(DISP_ON);
  LCD_WriteCom(CLR_DISP);
  _delay_ms(100);
  LCD_SendString("  --==START==--");
  
  __enable_interrupt();

  while(1)
  {
          
/*
      if(!(PIN_BUT & B1)) {
              SEND_CMD(CLR_DISP); 
              SEND_STR("0123456789012345ABCDEFGH");
      }
      

      if(!(PIN_BUT & B2)){
              SEND_CMD(DISP_CUR2);
      }


      if(!(PIN_BUT & B3)) {
              char temp[]="ABCDEFGHabcdefgh!\n\r0123456789\n\r";
              //for(uint8_t i=0; i<sizeof(temp); i++) usart1_write(temp[i]);
      }


      if(!(PIN_BUT & B4)) {
              char temp[]="ABCDEFGHabcdefgh!\n0123456789\n";
              //usart1_write_str(temp);    
      }
*/
  }
}
