


//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <avr/eeprom.h>

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>

	#include "lcd.h"
	#include "USART.h"
	#include "RTOS.h"
	
	#define KEY1		(1<<PA0)
	#define KEY2		(1<<PA1)
	#define KEY3		(1<<PA2)
	#define KEY4		(1<<PA3)
	#define KEY5		(1<<PA4)
	#define DALLAS 		(1<<PA5)
	#define RELAY 		(1<<PA6)

	#define BUZ_DIR		(1<<PE4)
	#define BUZ_INV		(1<<PE5)

	#define FREQ1		(1<<PD6)
	#define FREQ2		(1<<PD7)

	#define RELAY_ON	PORTA|=(1<<PA6)
	#define RELAY_OFF	PORTA&=~(1<<PA6)
	
	#define PIN_KEY PINA


	uint32_t volatile time;

void init(void)
{

	// Input/Output Ports initialization

	// Port A initialization

	DDRA=(RELAY) | (DALLAS) | (1<<PA7);
	PORTA=(KEY1) | (KEY2) |(KEY3) |(KEY4) |(KEY5);

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

//////////////////////////////////


void Sys_Proc1(void)
{

	if(time<Sys_Tick_End[1])
		time=0;
	else
		time-=Sys_Tick_End[1];


	PORTA|=(1<<PA7);
		
	while(time)
	{
		 time--;
		 asm volatile ("nop");
	}

	PORTA&=~(1<<PA7);

	time=0x00000FFF;

	Sys_Proc_END();
	
}

void Sys_Proc2(void)
{
	
//	while(1)
//	{
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
//	}

	//RELAY_OFF;
		
	Sys_Proc_END();
}


//////////////////////////////////


int main()
{
	
	init();

#if 0
	usart0_init();
	usart1_init();

	//LCD initialisation 
	LCD_Ini(); 					     
	// delay_ms(2);  		
	SEND_CMD(DISP_ON);
	// delay_ms(10);  						
	SEND_CMD(CLR_DISP); 
	// write
//	SEND_STR("Hello World!!!");
#endif

	InitRTOS();

	sei();


	while(1)
	{
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");

	
	}

}
