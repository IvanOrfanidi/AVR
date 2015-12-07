
#ifndef ENABLE_BIT_DEFINITIONS
  #define   ENABLE_BIT_DEFINITIONS  
#endif
  #include <ioavr.h>
  #include <intrinsics.h>

  #include "main.h"

  #define PORT_BUZ	PORTD
  #define PORT_LED	PORTB
  #define LED 	        (1<<PB0)
  #define BUT1		(1<<PC2)
  #define BUT2		(1<<PD0)
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
  #define BUZ_TOGGLE  PORTD ^= 0x18

  #define RELAY_OFF  PORTD &= ~RELAY
  #define RELAY_ON   PORTD |= RELAY

  #define GetIrStatus() (!(PIND & BUT2))
  #define GetButStatus() (!(PIN_BUT & BUT1))
  #define GetRelayStatus() (!(PIND & RELAY))

  #define StartConvAdc() ADCSR |= (1<<ADSC)  


  #define MAX_TEMPERATUR      30 * 10   //Максимальная температура двигателя при котором сгенериться ALARM.
  #define DELTA_TEMPERATUR    3 *10    //Дельта на охлаждения двигателя при котором сбросится ALARM.

typedef enum STATUS_DRIVE {
  MOVE = 0,
  STOP = 1,
} STATUS_DRIVE;

STATUS_DRIVE eStop = STOP;
int8_t fAlarm = 0;   //Флаг Аварии датчика температуры и перегрева двигателя.
unsigned char AdcBuf = 0;

char strTemperature[5];
int iTemperature;

void BuzzerHandler(void);


void Init(void)
{
  //Init Port B //
  DDRB |=LED;
  //PORT_LED |=LED;
  ////////////////

  // Init Port C //
  PORTC|= BUT1;
  /////////////////

  // Init Port D //
  DDRD = BUZ1 | BUZ2 | RELAY;
  PORT_BUZ = BUT2;
  
  //Analog Comparator initialization.
  ACSR|=(1<<ACD); //Disable Comparator.
  
  //Timer1 initialization.
  TCCR1B=(1<<CS12)|(0<<CS11)|(1<<CS10);	
  //TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);	//Timer1 clock off.
  OCR1A=7816;           //Interrupt 1 sec.
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
  STATUS_DRIVE eStopOld;
  uint8_t fStatusAlarmOld = 0;
  Init();
  
  //InitADC();
  
  usart0_init();
  
  __enable_interrupt();
  
  usart0_write_str("-=START=-\r");
  
  //StartConvAdc();
  
  while(1)
  {
    __watchdog_reset();		//Reset WATCHDOG.
    
   BuzzerHandler();     //Обработчик зуммера.
   
//ЕСЛИ ЕСТЬ ТРЕВОГА
    if(fAlarm) 
    {
      LED_ON;
      if(!(fStatusAlarmOld))
      {
        fStatusAlarmOld = 1;
        eStopOld = (STATUS_DRIVE)GetRelayStatus();
        if(eStopOld == MOVE) {
          usart0_write_str("-NEXT STEP MOVE-\r");
        }
        else {
           usart0_write_str("-NEXT STEP STOP-\r");
        }
      }
      eStop = STOP;
      RELAY_OFF;
      
      //Если захотим, то остановим цикл двигателя, в тревоге, по кнопке навсегда.
      if(GetIrStatus() || GetButStatus())
      {
         _delay_ms(5); //Предотвращаем дребезг контактов.
         if(GetIrStatus() || GetButStatus())
         {
           usart0_write_str("-NEXT STEP STOP BUT-\r");
            eStopOld = STOP;  //Остановим состояния двигателя, чтоб он не запустился самостоятельно.
            for(uint8_t i=0; i<4; i++) {
              LED_TOGGLE;
              BUZ_TOGGLE;
              _delay_ms(500);
              __watchdog_reset();		//Reset WATCHDOG.
            }
          BUZ_OFF;
        }
      }
      LED_ON;
    }
    else 
 //ЕСЛИ ТРЕВОГИ НЕТ     
    {
      LED_OFF;
      //Проверим можно ли запустить двигатель после перегрева.
      if(fStatusAlarmOld) {
        fStatusAlarmOld = 0;
        eStop = eStopOld;
        if(eStop == MOVE) {
          RELAY_ON;
          usart0_write_str("-MOVE CONTINUATION-\r");
        }
        else {
          usart0_write_str("-STOP CONTINUATION-\r");
          RELAY_OFF;
        }
      }
      
      //Обработчик управления с кнопки двигателем.
      if(GetIrStatus() || GetButStatus())
      {
        _delay_ms(5);     //Предотвращаем дребезг контактов.
          if(GetIrStatus() || GetButStatus())
          {
            LED_ON;
            if(eStop == STOP) {
              usart0_write_str("-MOVE BUT1-\r");
              eStop = MOVE;
              RELAY_ON;
            }
            else {
              usart0_write_str("-STOP BUT1-\r");
              eStop = STOP;
              RELAY_OFF;
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
        }
      
    }
    
  }
  
}

//Таймер обработки датчика температуры DS12B12.
#pragma vector=TIMER1_COMPA_vect
__interrupt void TIMER1_COMPA(void)
{
  static uint8_t StepConverts = 0;
  TCNT1=0x00;						//Reset Timer1.
  __enable_interrupt();
  
  if(StepConverts == 0) {
   if(DS18B20_Start_Converts()){
     usart0_write_str("ERROR\r");
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

      if(iTemperature <= 0) { //Генерим ошибку датчика если температура меньше либо равно нулю.
        fAlarm = -1;
      }
      else {
        //fAlarm = 1;
      }
      
      if( (iTemperature >= MAX_TEMPERATUR) && (fAlarm>=0) ){
        fAlarm = 1;    //Генерим Тревогу по перегреву двигателя.
      }
      
      if( (iTemperature < MAX_TEMPERATUR - DELTA_TEMPERATUR) && (fAlarm>0) ) {
        fAlarm = 0;   //Сбрасываем Тревогу по возвращению температуры двигателя в нормальный предел.
      }
      
      if(fAlarm > 0) {
        usart0_write_str("-STOP ALARM-\r");
      }
      if(fAlarm < 0) {
         usart0_write_str("-STOP ERROR-\r");
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

void BuzzerHandler(void)
{
    //Включаем зуммер в случае ошибки работы датчика температуры.
    if(fAlarm < 0) {
       BUZ_ON;
    }
    else {
      BUZ_OFF;
    }
}