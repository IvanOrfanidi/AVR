
#ifndef ENABLE_BIT_DEFINITIONS
  #define   ENABLE_BIT_DEFINITIONS  
#endif

#include <ioavr.h>
#include <intrinsics.h>

#include "main.h"

#define ADD_PWM_VALUE   50

#define StartConvAdc() ADCSR |= (1<<ADSC)  


void InitPeriph(void)
{
  //Analog Comparator initialization.
  ACSR|=(1<<ACD);		//Disable Comparator.
  
  //Timer0 initialization PWM.
  OCR0 = ADD_PWM_VALUE;
  TCCR0 = 0;
  /* CS02 CS01 CS00 Description 
   0 0 0 No clock source (Timer/Counter stopped)
   0 0 1 clkI/O/(No prescaling) 
   0 1 0 clkI/O/8 (From prescaler) 
   0 1 1 clkI/O/64 (From prescaler)
   1 0 0 clkI/O/256 (From prescaler)
   1 0 1 clkI/O/1024 (From prescaler)
   1 1 0 External clock source on T0 pin. Clock on falling edge
   1 1 1 External clock source on T0 pin. Clock on rising edge */
  TCCR0 |= (0<<CS02)|(0<<CS01)|(1<<CS00);	//Timer1 clock on.
  TCCR0 |= (1<<COM01)|(1<<COM00);
  TCCR0 |= (0<<WGM01)|(1<<WGM00);
  ////////////////////////////////////////////////////////////////////
  
  //Timer1 initialization.
  TCCR1B=(1<<CS12)|(0<<CS11)|(1<<CS10);	
  //TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);	//Timer1 clock off.
  OCR1A=7816;           //Interrupt 1 sec.
  TIMSK|=(1<<OCIE1A);	//Interrupt Timer1 COMPA.
  ////////////////////////////////////////////////////////////////////
  

  //ADC initialization.
  //ион - напряжение питания, выравнивание влево
  ADCSRA=(1<<ADEN);		//Enable ADC.
  ADMUX = (0<<REFS1)|(1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0);
  
  //вкл. ацп, режим одиночного преобразования, разрешение прерывания, частота преобр. = FCPU/128
  ADCSRA = (1<<ADEN)|(0<<ADSC)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
  ////////////////////////////////////////////////////////////////////
  
  // Ext. Interrupt initialization.
  MCUCR &= 0xFE;       // INT 0 sense on falling edge
  MCUCR |= 0x02;
  GICR |= (1<<INT0);        // enable interrupt on INT 0
  ///////////////////////////////////
  
}

int main( void )
{

  InitPeriph();
  
  //usart0_init();
  
  LCD_Init(); 

  _delay_ms(100); 
  LCD_Goto(0, 0);
  LCD_WriteCom(DISP_ON);
  LCD_WriteCom(CLR_DISP);
  _delay_ms(100);
  LCD_SendString("  --==START==--");
  
  __enable_interrupt();
  
  //StartConvAdc();
  
  while(1)
  {
     
  }
}

/******** INTRRUPT ********/

#pragma vector = TIMER1_COMPA_vect
__interrupt void TIMER1_COMPA(void)
{
    TCNT1=0x00;		//Reset Timer2.
    
}

#pragma vector = INT0_vect
__interrupt void EXT_INT0(void)
{
   
}