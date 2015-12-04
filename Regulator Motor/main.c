
#ifndef ENABLE_BIT_DEFINITIONS
  #define   ENABLE_BIT_DEFINITIONS  
#endif

  #include "main.h"

  #define PORT_BUZ	PORTD
  #define PORT_LED	PORTB
  #define LED 	        (1<<PB0)
  #define BUT		(1<<PC2)
  #define PIN_BUT	PINC
  #define BUZ		(1<<PD4)
  #define RELAY         (1<<PD2)

  #define LED_ON    PORT_LED |= LED
  #define LED_OFF   PORT_LED &= ~LED

  #define BUZ_ON    PORTD |= BUZ
  #define BUZ_OFF   PORTD &= ~BUZ

  #define RELAY_ON    PORTD |= RELAY
  #define RELAY_OFF   PORTD &= ~RELAY

char strTemperature[5];
int iTemperature;
#define TIME_UPDATE					750

void Init(void)
{
  //Init Port B //
  DDRB |=LED;
  PORT_LED |=LED;
  ////////////////

  // Init Port C //
  PORTC|=	BUT;
  /////////////////

  // Init Port D //
  DDRD =BUZ | RELAY;
  PORT_BUZ = 0;
  
  //Analog Comparator initialization.
  ACSR|=(1<<ACD); //Disable Comparator.
  
  //Timer1 initialization.
  TCCR1B=(1<<CS12)|(0<<CS11)|(1<<CS10);	
  //TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);	//Timer1 clock off.
  OCR1A=1953;
  TIMSK|=(1<<OCIE1A);	//Interrupt Timer1 COMPA.
  ////////////////////////////////////////////////////////////////////
  
  
  //WATCHDOG initialization.
  //WDTCR=(1<<WDTOE)|(1<<WDE);									//Enable WATCHDOG.
  //WDTCR=(1<<WDTOE)|(1<<WDE)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0);	//WATCHDOG clock 2100ms.
  ////////////////////////////////////////////////////////////////////
}

int main( void )
{
  
  Init();
  
  usart0_init();
  
  __enable_interrupt();
  
  usart0_write_str("-=START=-\r");
  
  while(1)
  {
    if(DS18B20_Start_Converts()) {
      usart0_write_str("ERROR\r");
      //
    }
    else {
        _delay_ms(TIME_UPDATE);
        DS18B20_Temperature(strTemperature); 
        usart0_write_str(strTemperature); usart0_write_str("\r");
        iTemperature = atoi(strTemperature);
        if(iTemperature > 320){
          LED_ON;
        }
        else {
          LED_OFF;
        }
    }
    
    _delay_ms(1000);
    
  }
  
}

#pragma vector=TIMER1_COMPA_vect
__interrupt void TIMER1_COMPA(void)
{
  TCNT1=0x00;						//Reset Timer1.
  //PORTB^=LED;
}