

//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/eeprom.h>
	#include <avr/pgmspace.h>
	#include <util/delay.h>

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>


// User File functions.
	#include "USART.h"
	#include "SIM.h"


////////////////////////////////////////////////////////////

// Служебные макросы
	#define true      1
	#define false     0
	#define bool      _Bool
//	#define NULL      0
	
////////////////////////////////////////////////////////////

// Macro Device.
	#define  USB_RST		(1<<PB2)		//Port OUTPUT.
	#define  STATUS			(1<<PB3)		//Port INPUT.
	#define  IN1			(1<<PB0)		//Port INPUT.

	#define  PWR_KEY		(1<<PA2)		//Port OUTPUT(-_-).
	#define  DCD			(1<<PA4)		//Port INPUT.
	#define  DTR			(1<<PA5)		//Port OUTPUT.
	#define  CTS			(1<<PA6)		//Port INPUT.
	#define  RTS			(1<<PA7)		//Port OUTPUT.

	#define RELAY1			(1<<PC7)		//Port OUTPUT.
	#define RELAY2			(1<<PC6)		//Port OUTPUT.


	#define  LED			(1<<PD7)		//Port OUTPUT.
	#define  RI				(1<<PD6)		//Port INPUT.
	#define  USB_PRESENT	(1<<PD5)		//Port INPUT.
	#define	 BUZ			(1<<PD4)		//Port OUTPUT.
	#define	 B1				(1<<PD3)		//Port INPUT.
	#define	 IN2			(1<<PD2)		//Port INPUT.


	#define PORT_RELAY		PORTC
	#define PORT_OUT		PORTD
	#define PORT_SIM		PORTA


	#define PIN_STATUS		PINB
	#define PIN_IN			PIND
	#define PIN_SIM			PINA

////////////////////////////////////////////////////////////

// Macro Timers
	//Timer 0.
	#define START_TIMER0 TCCR0=(0<<CS02)|(1<<CS01)|(1<<CS00);	//Timer0 start clock Fcpu/64.
	#define STOP_TIMER0 TCCR0=(0<<CS02)|(0<<CS01)|(0<<CS00);	//Timer0 stop clock.
	//Timer 1.
	#define START_TIMER1 TCCR1B=(1<<CS12)|(0<<CS11)|(1<<CS10);	//Timer1 start clock Fcpu/1024.
	#define STOP_TIMER1 TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);	//Timer1 stop clock.

	//Timer 2.
	#define START_TIMER2 TCCR2=(0<<CS22)|(0<<CS21)|(1<<CS20);	//Timer2 start clock.
	#define STOP_TIMER2 TCCR2=(0<<CS22)|(0<<CS21)|(0<<CS20);	//Timer2 stop clock.

////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void init(void){

	// Port A initialization //
	DDRA = RTS | DTR;// | PWR_KEY;
	PORTA = CTS | DCD;// | PWR_KEY;



	// Port B initialization //
	DDRB = USB_RST;
	PORTB = STATUS | IN1 | USB_RST;


	// Port C initialization //
	DDRC = RELAY1 | RELAY2;
	PORTC = 0x00;


	// Port D initialization //
	DDRD = LED | BUZ;
	PORTD = LED | RI | USB_PRESENT | B1 | IN2;

	//////////////////////////////////////////////////////////////////////////////


	//Analog Comparator initialization.

	ACSR|=_BV(ACD);		//Disable Comparator.

	/////////////////////////////////////////////////////////////////////


	//Init Timer0//
	
	TIMSK|=(1<<TOIE0);
	/////////////////////////////////////////////////////////////////////

	//Init Timer1//
	
	OCR1A=0x1C20;			//Interrupt 1c.
	TIMSK|=(1<<OCIE1A);

	/////////////////////////////////////////////////////////////////////



	/////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////////////
char u[20];

////////////////////////////////////////////////////////////////////////////////////////

int main(void){

	init();

	usart0_init();

	sei();

	for(uint8_t delay=0; delay<3; delay++) 
		_delay_ms(1000);

	while(AtOk())			//Проверяем готовность модуля командой AT.
	{
		PORT_SIM|= (PWR_KEY);
		_delay_ms(2000);
		PORT_SIM&= ~(PWR_KEY);
		_delay_ms(5000);
		PORT_SIM|= (PWR_KEY);
	}
	
	while(ConfigGsmModules())	_delay_ms(1000);

	_delay_ms(500);

	while(GsmReg())	_delay_ms(2000);

//	START_TIMER0;

	_delay_ms(1000);

	//SimIMEI(u);	//!!!And IMEI_NUM_ENABLE

	while(SignalQualityReport(u)) 	_delay_ms(2000);
	

	while(SendSMS("+79119008502", "Hello GSM SIM300!")) _delay_ms(2000);

	//while(UnstructuredSupplementaryServiceData("#102#", u)) _delay_ms(2000);


	while(1){
		
		//PORT_OUT|=BUZ;
		
		PORT_OUT&=~BUZ;
		PORT_OUT&=~BUZ;
		PORT_OUT&=~BUZ;

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// INTERRUPT //




