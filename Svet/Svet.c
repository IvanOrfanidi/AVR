
//Функции для МК.
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


// Стандартные функции Ввода/Вывода.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


#define CPU_freq 1000000UL		//Freq CPU.


#define ON 1
#define OFF 0
#define HIGH 1
#define LOW 0


#define RELAY		PA1			//Pin OUTPUT Relay.
#define PORT_RELAY	PORTA		//Port OUTPUT Relay.	
#define PIN_TIME	PINB		//Port INPUT jmp seting time.
#define LED_RED		PD5			//Pin OUTPUT Led Red.
#define LED_GREEN	PD4			//Pin OUTPUT Led Green.
#define PORT_LEDS	PORTD		//Port OUTPUT Leds.


const int relay_on=0x0380;		//Порог вкл. Реле.
const int relay_off=0x0340;		//Порог выкл. Реле.

unsigned long int second_day; 
unsigned long int second_night;
unsigned long int time_delay;	//Time Delay.


//INITIALIZATION FUNCTION//
int Init()
{

// Port A initialization.
DDRA=(0<<PA0)|(1<<PA1)|(0<<PA2)|(0<<PA3)|(0<<PA4)|(0<<PA5)|(0<<PA6)|(0<<PA7);
PORTA=(0<<PA0)|(0<<PA1)|(0<<PA2)|(0<<PA3)|(0<<PA4)|(0<<PA5)|(0<<PA6)|(0<<PA7);

// Port B initialization.
DDRB=(0<<PB0)|(0<<PB1)|(0<<PB2)|(0<<PB3)|(0<<PB4)|(0<<PB5)|(0<<PB6)|(0<<PB7);
PORTB=(1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3)|(1<<PB4)|(1<<PB5)|(1<<PB6)|(1<<PB7);


// Port C initialization.
DDRC=(0<<PC0)|(0<<PC1)|(0<<PC2)|(0<<PC3)|(0<<PC4)|(0<<PC5)|(0<<PC6)|(0<<PC7);
PORTC=(0<<PC0)|(0<<PC1)|(0<<PC2)|(0<<PC3)|(0<<PC4)|(0<<PC5)|(0<<PC6)|(0<<PC7);

// Port D initialization.
DDRD=(0<<PD0)|(0<<PD1)|(0<<PD2)|(0<<PD3)|(1<<PD4)|(1<<PD5)|(0<<PD6)|(0<<PD7);
PORTD=(0<<PD0)|(0<<PD1)|(0<<PD2)|(0<<PD3)|(0<<PD4)|(0<<PD5)|(0<<PD6)|(0<<PD7);
/////////////////////////////////////////////////////////////////////



//Analog Comparator initialization.

ACSR|=_BV(ACD);		//Disable Comparator.

////////////////////////////////////////////////////////////////////




//ADC initialization.

sei();					//Enable Interrupt for ADC seting.

ADCSRA|=_BV(ADEN);		//Enable ADC.
ADCSRA|=_BV(ADPS1);		//fclkADC=fclkIO/4.

ADMUX=(0<<REFS0)|(1<<REFS1);		//Uref=AVcc.

cli();					//Disable Interrupt.
/////////////////////////////////////////////////////////////////////


//Timer0 initialization.

TCCR0=(1<<CS02)|(0<<CS01)|(1<<CS00);	//976.5625Hz.
TIMSK|=_BV(TOIE0);						//Interrupt OVERFLOT Timer0.


////////////////////////////////////////////////////////////////////


//Timer1 initialization.

TCCR1B=(0<<CS12)|(1<<CS11)|(1<<CS10);	//Freq 15 625Hz (0x3D09).
//TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);	//Timer1 clock off.

OCR1A=0x3D09;	//15 625 = 0x3D09.


TIMSK|=_BV(OCIE1A);	//Interrupt Timer1 COMPA.
////////////////////////////////////////////////////////////////////



//WATCHDOG initialization.

WDTCR=(1<<WDTOE)|(1<<WDE);									//Enable WATCHDOG.

WDTCR=(1<<WDTOE)|(1<<WDE)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0);	//WATCHDOG clock 2100ms.


////////////////////////////////////////////////////////////////////

MCUCR=(1<<SE);						//Enable Sleep.

return 0;

}


int main(void)
{


	Init();							//Initialization Procedur.

	sei();							//Enable Interrupt.

	while(1)
	{
		asm volatile ("wdr");		//Reset WATCHDOG.

	
		asm volatile ("sleep");		//Sleep CPU.
	
	}

return 0;

}

//INTERRUPT//


//Interrupt Timer1 COMARATOR A.//
ISR(TIMER1_COMPA_vect)				//Interrupt 1c.
{
	cli();							//Disable Interrupt.

	TCNT1=0x00;						//Reset Timer1.


	ADCSRA|=(1<<ADSC);				//Start ADC.

		while (ADCSRA & & (1<<ADSC)); 		//Wait for the AD conversion to complete.



		if(ADC>relay_on){

			
				//PORT_LEDS=(1<<LED_RED);
				//PORT_RELAY=(1<<RELAY);
				
				second_day=0;

		}
		
		if(ADC<relay_off){

		
				//PORT_LEDS=(1<<LED_GREEN);
				//PORT_RELAY=(0<<RELAY);
			
				
				second_night=0;

		}


	time_delay=PIN_TIME;
	time_delay=(time_delay*30)+1;
	
	
	if(second_day>=time_delay){

		PORT_LEDS=(1<<LED_GREEN);
		PORT_RELAY&=~_BV(RELAY);
		second_day=0;

		TCCR0=0x00;		//Stop Timer0.
	}



	if(second_night>=time_delay){

		PORT_LEDS=(1<<LED_RED);
		PORT_RELAY|=_BV(RELAY);
		second_night=0;

		TCCR0=0x00;		//Stop Timer0.

	}

	
	second_day++;
	second_night++;

	sei();				//Enable Interrupt.

}
//////////////////////////////////////////////////////////////////////////////


//Interrupt Timer0 OVERFLOT.//
ISR(TIMER0_OVF_vect){				//Interrupt 1/((1000000/1024)/256)=0.26c.

//Мигаем светодиодом обозначая инициализацию.//
if(PIND!=0b00100000){
	PORT_LEDS=(1<<LED_RED);			//Led Red ON. Led Green OFF.
}
	else
	{
		PORT_LEDS=(1<<LED_GREEN);	//Led Red OFF. Led Green ON.
	}

	

}

//////////////////////////////////////////////////////////////////////////////

