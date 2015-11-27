
	#include "RTOS.h"


void InitRTOS(void)
{

	Proc_Num=1;

	OCR1A=SYS_TIKER;

	TIMSK=(1<<OCIE1A);	//Interrupt Timer1 COMPA.
	
	TCCR1B=(0<<CS12)|(0<<CS11)|(1<<CS10);

}



void Sys_Proc_END(void)
{

	Sys_Tick_End[Proc_Num-1]=SYS_TIKER-TCNT1;

	TCNT1=SYS_TIKER-1;

	fProc_END=1;

}

ISR(TIMER1_COMPA_vect)
{
	TCNT1=0x0000;

	asm volatile ("LDI R17,0x10");
	asm volatile ("LDI R18,0xFD");
	
	asm volatile ("OUT 0x3E,R17");
	asm volatile ("OUT 0x3D,R18");

	if(fProc_END)
	{
		fProc_END=0;
	}
	else
	{
		Sys_Tick_End[Proc_Num-1]=0x0009;
	}

	Proc_Num++;
	if(Proc_Num>PROCES_SYS)
		Proc_Num=1;

	sei();

	switch(Proc_Num)
	{
		case 1:			
	 		Sys_Proc1();
			break;

		case 2:
	 		Sys_Proc2();
			break;
		}

	Sys_End_Proc=0;

}
