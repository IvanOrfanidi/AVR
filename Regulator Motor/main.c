
#ifndef ENABLE_BIT_DEFINITIONS
  #define   ENABLE_BIT_DEFINITIONS  
#endif
  #include <ioavr.h>
  #include <intrinsics.h>

  #include "main.h"

  #define PORT_BUZ	PORTD
  #define PORT_LED	PORTB
  #define LED 	        (1<<PB0)
  #define BUT		(1<<PC2)
  #define IR            (1<<PC4)
  #define PIN_BUT	PINC
  #define BUZ1		(1<<PD4)
  #define BUZ2		(1<<PD3)
  #define RELAY         (1<<PD2)

  #define LED_ON      PORT_LED |= LED
  #define LED_OFF     PORT_LED &= ~LED
  #define LED_TOGGLE  PORT_LED ^= LED

  #define BUZ_ON    PORTD |= 0x18
  #define BUZ_OFF   PORTD &= ~0x18

  #define RELAY_ON    PORTD |= RELAY
  #define RELAY_OFF   PORTD &= ~RELAY

  #define GetIrStatus() (!(PIND & (1<<PD0)))
  #define GetButStatus() (!(PIN_BUT & BUT))

  #define StartConvAdc() ADCSR |= (1<<ADSC)  


  #define MAX_TEMPERATUR  330
  #define DELTA_TEMPERATUR  30


uint8_t fStop = 1;
int8_t fAlarm = 0;   //Флаг Аварии датчика температуры и перегрева двигателя.
unsigned char AdcBuf = 0;

char strTemperature[5];
int iTemperature;



void Init(void)
{
  //Init Port B //
  DDRB |=LED;
  //PORT_LED |=LED;
  ////////////////

  // Init Port C //
  PORTC|= BUT;
  /////////////////

  // Init Port D //
  DDRD = BUZ1 | BUZ2 | RELAY;
  PORT_BUZ = 0;
  
  //Analog Comparator initialization.
  ACSR|=(1<<ACD); //Disable Comparator.
  
  //Timer1 initialization.
  TCCR1B=(1<<CS12)|(0<<CS11)|(1<<CS10);	
  //TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);	//Timer1 clock off.
  OCR1A=7816;
  TIMSK|=(1<<OCIE1A);	//Interrupt Timer1 COMPA.
  ////////////////////////////////////////////////////////////////////
  
  
  //WATCHDOG initialization.
  WDTCR=(1<<WDTOE)|(1<<WDE);									//Enable WATCHDOG.
  WDTCR=(1<<WDTOE)|(1<<WDE)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0);	//WATCHDOG clock 2100ms.
  ////////////////////////////////////////////////////////////////////
}

/*
void InitADC(void)
{
  //инициализируем АЦП
  //ион - напряжение питания, выравнивание влево
  ADCSR=(1<<ADEN);		//Enable ADC.
  ADMUX = (0<<REFS1)|(1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(1<<MUX2)|(0<<MUX1)|(0<<MUX0);
  
  //вкл. ацп, режим одиночного преобразования, разрешение прерывания, частота преобр. = FCPU/128
  ADCSR = (1<<ADEN)|(0<<ADSC)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}
*/


int main( void )
{
  __disable_interrupt();
  uint8_t fStopOld;
  Init();
  
  //InitADC();
  
  usart0_init();
  
  __enable_interrupt();
  
  usart0_write_str("-=START=-\r");
  
  StartConvAdc();
  
  while(1)
  {
    __watchdog_reset();		//Reset WATCHDOG.
    
    if( (GetIrStatus() || GetButStatus()) && (!(fAlarm)) ) {
      LED_ON;
      if(fStop) {
        usart0_write_str("BUT S=0\r");
        fStop = 0;
      }
      else {
        usart0_write_str("BUT S=1\r");
        fStop = 1;
      }
      
      if(!(fStop)){
          usart0_write_str("-MOVE-\r");
      }
      else {
          usart0_write_str("-STOP-\r");
      }
      
      for(uint8_t i=0; i<50; i++) {
        LED_TOGGLE;
          _delay_ms(50);
           __watchdog_reset();		//Reset WATCHDOG.
      }
       LED_OFF;
      
      while( GetButStatus() ) {
           __watchdog_reset();		//Reset WATCHDOG.
      }
      
    }
       
    if(fAlarm) {
      fStopOld = fStop;
      fStop = 1;
      LED_ON;
    }
    else {
      LED_OFF;
    }
    
    if(fAlarm < 0) {
       BUZ_ON;
    }
    else {
      BUZ_OFF;
    }
      
    
  }
  
}

#pragma vector=TIMER1_COMPA_vect
__interrupt void TIMER1_COMPA(void)
{
  static uint8_t StepConverts = 0;
  TCNT1=0x00;						//Reset Timer1.
  __enable_interrupt();
  
  if(StepConverts == 0) {
   if(DS18B20_Start_Converts()){
     fAlarm = -1;
     return;
   }
   StepConverts++;
   return;
  }
  
  if(StepConverts == 1) {
      DS18B20_Temperature(strTemperature); 
      usart0_write_str(strTemperature); usart0_write_str("\r");
      iTemperature = atoi(strTemperature);
      StepConverts = 0;

      if(iTemperature <= 0) {
        fAlarm = -1;
      }
      else {
        fAlarm = 1;
      }
      
      if( (iTemperature >= MAX_TEMPERATUR) && (fAlarm>=0) ){
        fAlarm = 1;
      }
      
      if( (iTemperature < MAX_TEMPERATUR - DELTA_TEMPERATUR) && (fAlarm>0) ) {
        fAlarm = 0;
      }
      
      if(fAlarm>0) {
        usart0_write_str("-ALARM-\r");
      }
      if(fAlarm<0) {
         usart0_write_str("ERROR\r");
      }

      return;
    }
}

/*
#pragma vector=ADC_vect
__interrupt void adc_my(void) 
{
  //считываем старший регистр АЦП
  AdcBuf = ADCH;
  if(AdcBuf < 210){
    LED_ON;
    _delay_ms(500);
  }
  else {
    LED_OFF;
  }
  StartConvAdc();
}
*/