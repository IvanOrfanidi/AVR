
#ifndef ENABLE_BIT_DEFINITIONS
  #define   ENABLE_BIT_DEFINITIONS  
#endif

#include <ioavr.h>
#include <intrinsics.h>


#include "main.h"

void InitGPIO(void)
{
   
}

int main( void )
{

  InitGPIO();
  
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
  }
}
