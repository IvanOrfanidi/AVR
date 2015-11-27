

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Buzzer.h"


//Процедура использует таймер T1(16bit). 
void buzzer_timer(uint16_t Buz_Freq, uint16_t Buz_Stop)
{

stopbuzzer=Buz_Stop;

sei();		//Enable Interrupt.

DDRD|=(1<<BUZ);

//Инициализируем таймер T1.

TIMSK|=(1<<OCIE1B);			//Устанавливаем генерацию прерывания по совпадению OCIE1B.

TCCR1A|=(1<<COM1B0);			//Состояние вывода OC1B меняется на противоположное.

OCR1B=Buz_Freq;			//Buzzer ON.


TCCR1B|=(0<<CS12)|(0<<CS11)|(1<<CS10);	


}



//*INTERRUPT*//

//Interrupt for TIMER1 COMPB.
ISR (TIMER1_COMPB_vect)
{

t++;

TCNT1=0x0000;		//Clear timer T1.

if (t==stopbuzzer)
	{

	TCCR1B=0;				//Stop timer T1.
	TCCR1A&=~(1<<COM1B0);	//Disabled Interrupt COM1B0.
	TCNT1=TCNT1=0x0000;		//Clear timer T1.
	t=0;
	}


}



