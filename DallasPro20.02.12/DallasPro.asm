
// PROG FOR DALLAS PRO //
// IVAN ORFANIDI //
//	  02.2012	 //
//  FREQ 4MHz Int RC   //

.device avrmega8	
.nolist

.include "C:\Program Files\Atmel\AVR Tools\AvrAssembler2\Appnotes\m8def.inc"
.list


	.equ KEY0=PC2			//Pin INPUT Key#0.
	.equ KEY1=PC4			//Pin INPUT Key#1.
	.equ KEY2=PC3			//Pin INPUT Key#2.
	.equ KEY3=PC5			//Pin INPUT Key#3.
	.equ KEY4=PD7			//Pin INPUT Key#4.

	.equ BUT_PROG=PD2		 //!!!PD2 FOR NEW VERSION.

	.equ PC_CONNECT=PD4	//Pin INPUT Connect Terminal PC.
	
	.equ PORT_LED=PORTB
	.equ PIN_LED=PINB
	.equ LED_RED=PB1
	.equ LED_GREEN=PB0

	.equ DALLAS=PD3			//Pin OUTPUT/INPUT DALLAS.


	.equ ADC_BAT=PC0		//Pin INPUT Bat.

	.def COUNT_RX=R17
	.def COMMAND=R18
	.def COUNTER=R19
	.def BYTE=R20
	.def COUNTER_BYTE=R21

	.def FAM=R1
	.def KEY_5=R2
	.def KEY_4=R3
	.def KEY_3=R4
	.def KEY_2=R5
	.def KEY_1=R5
	.def KEY_0=R6
	.def CRC=R7



	.equ ON=1
	.equ OFF=0

	.equ ZERRO=80	//80
	.equ ONE=25		//25

	.equ RX_BUFFER_SIZE=9

	.equ ADDR_EEP_KEY0=0x0110
	.equ ADDR_EEP_KEY1=0x0120
	.equ ADDR_EEP_KEY2=0x0130
	.equ ADDR_EEP_KEY3=0x0140
	.equ ADDR_EEP_KEY4=0x0150
	.equ ADDR_EEP_PASSWORD=0x0160

	.equ DELAY_ONE=60
	.equ DELAY_ZERRO=20

	.equ TIMER1_COMPA=0x7A12

	.equ UBAT_CODE=165

RJMP init	
		
	.org 0x006
		RJMP TIM1_COMPA_vect

	.org 0x00B				
		RJMP USART_RXC_vect	

	.org 0x0014		//End Address Vector Interrupt.


init:


// Init Stack //

	LDI R16, low(RAMEND);	
	OUT SPL, R16;			

	LDI R16, high(RAMEND);	
	OUT SPH, R16;

///////////////////////////////////////////////////////////////////

// Init Analog Comparator //

	LDI R16, (1<<ACD);		//Disable Comparator.
	OUT ACSR, R16;


///////////////////////////////////////////////////////////////////

// Port B initialization //

	LDI R16, (1<<LED_RED)|(1<<LED_GREEN);
	OUT DDRB, R16;

	LDI R16, (0<<LED_RED)|(0<<LED_GREEN);
	OUT PORTB, R16;
	
///////////////////////////////////////////////////////////////////

// Port C initialization //

	LDI R16, (0<<KEY0)|(0<<KEY1)|(0<<KEY2)|(0<<KEY3);
	OUT DDRC, R16;

	LDI R16, (1<<KEY0)|(1<<KEY1)|(1<<KEY2)|(1<<KEY3);
	OUT PORTC, R16;

///////////////////////////////////////////////////////////////////

// Port D initialization //

	LDI R16, (0<<PC_CONNECT)|(0<<BUT_PROG)|(0<<DALLAS)|(0<<KEY4);
	OUT DDRD, R16;

	LDI R16, (1<<PC_CONNECT)|(1<<BUT_PROG)|(0<<DALLAS)|(1<<KEY4);
	OUT PORTD, R16;

///////////////////////////////////////////////////////////////////

// Init Timer 1 //

	LDI R16, (1<<CS12)|(0<<CS11)|(0<<CS10);	//Freq 15 625Hz (0x3D09).
	OUT TCCR1B, R16;
	
	LDI R16, high(TIMER1_COMPA);	//Interrupt 2c.
	OUT OCR1AH, R16;

	LDI R16, low(TIMER1_COMPA);
	OUT OCR1AL, R16;
	
	LDI R16, (1<<OCIE1A);
	OUT TIMSK, R16;

///////////////////////////////////////////////////////////////////

// Init ADC //

	SEI;	//Enable Interrupt.

	LDI R16, (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(0<<ADPS1);
	OUT ADCSRA, R16;

	LDI R16, (0<<REFS1)|(1<<REFS0)|(1<<ADLAR);
	OUT ADMUX, R16;

	CLI;	//Disable Interrupt.

///////////////////////////////////////////////////////////////////


//	Init USART	//

.equ XTAL=4000000;					
.equ baudrate = 9600;						
.equ bauddivider = XTAL/(16*baudrate)-1;

	
	LDI R16, low(bauddivider);
	OUT UBRRL, R16;


	LDI R16, high(bauddivider);
	OUT UBRRH, R16;


	LDI R16, (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(0<<TXCIE)|(0<<UDRIE);
	OUT UCSRB, R16;
	
	LDI R16, (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);	//8-N-1//
	OUT UCSRC, R16;
		

///////////////////////////////////////////////////////////////////

// Init WATCHDG //
/*
	LDI R16, (1<<WDTOE)|(1<<WDE);
	OUT WDTCR, R16;

	LDI R17, (1<<WDP2)|(1<<WDP1)|(1<<WDP0);
	OR R16,R17;
	OUT WDTCR, R16;
									;# WDP2 # WDP1 # WDP0 #  	Time Reset    #
									;#  0   #   0  #   0  # 16.3��.	(17.3��.) #
									;#  0   #   0  #   1  # 32.5��.	(34.3��.) #
									;#  0   #   1  #   0  # 65��.	(68.5��.) #
									;#  0   #   1  #   1  # 130��.	(140��.)  #
									;#  1   #   0  #   0  # 260��.	(270��.)  #
									;#  1   #   0  #   1  # 520��.	(550��.)  #
									;#  1   #   1  #   0  # 1000��.	(1100��.) #
									;#  1   #   1  #   1  # 2100��.	(2200��.) #

*/
///////////////////////////////////////////////////////////////////



RJMP main

// INTERRUPT //

// INTERRUPT USART RX OK //
USART_RXC_vect:

	SBIS PIND, PC_CONNECT;
		RETI;

//	SBI PORT_LED, LED_GREEN;

	LDI COUNT_RX, RX_BUFFER_SIZE;
	
	cycle_buf_rx:

		SBIS UCSRA, RXC;
			RJMP PC-1;

	
		//	�������� �� ������.
		IN R16, UCSRA;

		ANDI R16, (1<<FE)|(1<<DOR)|(1<<UPE);
			BREQ	no_error
		
		SBI PORT_LED, LED_RED;		//If Error RX Data.
	
		no_error:

			IN R16, UDR;

			PUSH R16;

			DEC COUNT_RX;
				BRNE cycle_buf_rx;


		CBI PORT_LED, LED_GREEN;	


			POP R16;	// Stack -> R16


			CPI R16, 0x30;			//	0	48
				BREQ write_key0;
		
			CPI R16, 0x31;			//	1	49
				BREQ write_key1;

			CPI R16, 0x32;			//	2	50
				BREQ write_key2;

			CPI R16, 0x33;			//	3	51
				BREQ write_key3;

			CPI R16, 0x34;			//	4	52
				BREQ write_key4;

			CPI R16, 0x35;			//	5	53
				BREQ read_password;

			CPI R16, 0x36;			//	6	54
				BREQ reboot_unit;

		RJMP not_key
	
	reboot_unit:
		
		POP R16;	//	0
		POP R16;	//	1
		POP R16;	//	2
		POP R16;	//	3
		POP R16;	//	4
		POP R16;	//	5
		POP R16;	//	6
		POP R16;	//	7

		SBI PORT_LED, LED_GREEN;	

		RETI;

	write_key0:
		
		LDI XL, low(ADDR_EEP_KEY0);		//Low address  END BYTE EPPROM for KEY0.
		LDI XH, high(ADDR_EEP_KEY0);	//High address END BYTE EPPROM for KEY0.


		RJMP write_eeprom;

	write_key1:

		LDI XL, low(ADDR_EEP_KEY1);		//Low address  END BYTE EPPROM for KEY1.
		LDI XH, high(ADDR_EEP_KEY1);	//High address END BYTE EPPROM for KEY1.


		RJMP write_eeprom;

	write_key2:

		LDI XL, low(ADDR_EEP_KEY2);		//Low address  END BYTE EPPROM for KEY2.
		LDI XH, high(ADDR_EEP_KEY2);	//High address END BYTE EPPROM for KEY2.

		RJMP write_eeprom

	write_key3:

		LDI XL, low(ADDR_EEP_KEY3);		//Low address  END BYTE EPPROM for KEY3.
		LDI XH, high(ADDR_EEP_KEY3);	//High address END BYTE EPPROM for KEY3.

		RJMP write_eeprom

	write_key4:

		LDI XL, low(ADDR_EEP_KEY4);		//Low address  END BYTE EPPROM for KEY4.
		LDI XH, high(ADDR_EEP_KEY4);	//High address END BYTE EPPROM for KEY4.

		RJMP write_eeprom

	read_password:

		LDI XL, low(ADDR_EEP_PASSWORD);		//Low address  END BYTE EPPROM for PASSWORD.
		LDI XH, high(ADDR_EEP_PASSWORD);	//High address END BYTE EPPROM for PASSWORD.
	
		POP R16;	//	0
		POP R16;	//	1
		POP R16;	//	2
		POP R16;	//	3
		POP R16;	//	4
		POP R16;	//	5
		POP R16;	//	6
		POP R16;	//	7

		RJMP read_eeprom_key_all

	
	write_eeprom:

		LDI COUNT_RX, RX_BUFFER_SIZE;
		DEC COUNT_RX;
	
		cycle_write_eeprom:

			SBIC EECR, EEWE;
				RJMP PC-1;

			OUT EEARL, XL;
			OUT EEARH, XH;
	
			POP R16;

			OUT EEDR, R16;

			SBI EECR, EEMWE;
			SBI EECR, EEWE;

			INC XL;

			DEC COUNT_RX;
				BRNE cycle_write_eeprom;

			RJMP read_eeprom_key_all

	not_key:

		SBI PORT_LED, LED_RED		//If Error Key.

read_eeprom_key_all:

//0/////////////////////////////////////////////////////////////////////////

	// READ EEPROM KEY0 //
read_eeprom_key0:

	LDI XL, low(ADDR_EEP_KEY0);			//Low address  END BYTE EPPROM for KEY0.
	LDI XH, high(ADDR_EEP_KEY0);		//High address END BYTE EPPROM for KEY0.

		LDI COUNT_RX, RX_BUFFER_SIZE;
		DEC COUNT_RX;

		LDI R16, 0x30;	//Code key 0.
		PUSH R16;		// '0'(0x30) -> Stack

	cycle_read_eeprom_key0:

		SBIC EECR, EEWE;
			RJMP PC-1;
		
		OUT EEARL, XL;
		OUT EEARH, XH;

		SBI EECR, EERE;		//SET BIT READ EEPROM.

		IN R16, EEDR;
		
		PUSH R16;			//Code key 0 -> Stack

		INC XL;

		DEC COUNT_RX;
			BRNE cycle_read_eeprom_key0


		LDI COUNT_RX, RX_BUFFER_SIZE;

	cycle_usart_key0:
		
		POP R16;

		SBIS UCSRA, UDRE;
			RJMP PC-1
					
		OUT UDR, R16;

		DEC COUNT_RX;
			BRNE cycle_usart_key0

////////////////////////////////////////////////////////////////////////////

//1/////////////////////////////////////////////////////////////////////////

	// READ EEPROM KEY1 //
read_eeprom_key1:

	LDI XL, low(ADDR_EEP_KEY1);			//Low address  END BYTE EPPROM for KEY1.
	LDI XH, high(ADDR_EEP_KEY1);		//High address END BYTE EPPROM for KEY1.

		LDI COUNT_RX, RX_BUFFER_SIZE;
		DEC COUNT_RX;

		LDI R16, 0x31;	//Code key 1.
		PUSH R16;		// '1'(0x31) -> Stack

	cycle_read_eeprom_key1:

		SBIC EECR, EEWE;
			RJMP PC-1;
		
		OUT EEARL, XL;
		OUT EEARH, XH;

		SBI EECR, EERE;		//SET BIT READ EEPROM.

		IN R16, EEDR;
		
		PUSH R16;			//Code key 1 -> Stack

		INC XL;

		DEC COUNT_RX;
			BRNE cycle_read_eeprom_key1


		LDI COUNT_RX, RX_BUFFER_SIZE;

	cycle_usart_key1:
		
		POP R16;

		SBIS UCSRA, UDRE;
			RJMP PC-1
					
		OUT UDR, R16;

		DEC COUNT_RX;
			BRNE cycle_usart_key1

////////////////////////////////////////////////////////////////////////////
		
//2/////////////////////////////////////////////////////////////////////////

	// READ EEPROM KEY2 //
read_eeprom_key2:

	LDI XL, low(ADDR_EEP_KEY2);			//Low address  END BYTE EPPROM for KEY2.
	LDI XH, high(ADDR_EEP_KEY2);		//High address END BYTE EPPROM for KEY2.

		LDI COUNT_RX, RX_BUFFER_SIZE;
		DEC COUNT_RX;

		LDI R16, 0x32;	//Code key 2.
		PUSH R16;		// '2'(0x32) -> Stack

	cycle_read_eeprom_key2:

		SBIC EECR, EEWE;
			RJMP PC-1;
		
		OUT EEARL, XL;
		OUT EEARH, XH;

		SBI EECR, EERE;		//SET BIT READ EEPROM.

		IN R16, EEDR;
		
		PUSH R16;			//Code key 2 -> Stack

		INC XL;

		DEC COUNT_RX;
			BRNE cycle_read_eeprom_key2


		LDI COUNT_RX, RX_BUFFER_SIZE;

	cycle_usart_key2:
		
		POP R16;

		SBIS UCSRA, UDRE;
			RJMP PC-1
					
		OUT UDR, R16;

		DEC COUNT_RX;
			BRNE cycle_usart_key2

////////////////////////////////////////////////////////////////////////////

//3/////////////////////////////////////////////////////////////////////////

	// READ EEPROM KEY3 //
read_eeprom_key3:

	LDI XL, low(ADDR_EEP_KEY3);			//Low address  END BYTE EPPROM for KEY3.
	LDI XH, high(ADDR_EEP_KEY3);		//High address END BYTE EPPROM for KEY3.

		LDI COUNT_RX, RX_BUFFER_SIZE;
		DEC COUNT_RX;

		LDI R16, 0x33;	//Code key 3.
		PUSH R16;		// '3'(0x33) -> Stack

	cycle_read_eeprom_key3:

		SBIC EECR, EEWE;
			RJMP PC-1;
		
		OUT EEARL, XL;
		OUT EEARH, XH;

		SBI EECR, EERE;		//SET BIT READ EEPROM.

		IN R16, EEDR;
		
		PUSH R16;			//Code key 3 -> Stack

		INC XL;

		DEC COUNT_RX;
			BRNE cycle_read_eeprom_key3


		LDI COUNT_RX, RX_BUFFER_SIZE;

	cycle_usart_key3:
		
		POP R16;

		SBIS UCSRA, UDRE;
			RJMP PC-1
					
		OUT UDR, R16;

		DEC COUNT_RX;
			BRNE cycle_usart_key3

////////////////////////////////////////////////////////////////////////////

//4/////////////////////////////////////////////////////////////////////////

	// READ EEPROM KEY4 //
read_eeprom_key4:

	LDI XL, low(ADDR_EEP_KEY4);			//Low address  END BYTE EPPROM for KEY4.
	LDI XH, high(ADDR_EEP_KEY4);		//High address END BYTE EPPROM for KEY4.

		LDI COUNT_RX, RX_BUFFER_SIZE;
		DEC COUNT_RX;

		LDI R16, 0x34;	//Code key 4.
		PUSH R16;		// '4'(0x34) -> Stack

	cycle_read_eeprom_key4:

		SBIC EECR, EEWE;
			RJMP PC-1;
		
		OUT EEARL, XL;
		OUT EEARH, XH;

		SBI EECR, EERE;		//SET BIT READ EEPROM.

		IN R16, EEDR;
		
		PUSH R16;			//Code key 4 -> Stack

		INC XL;

		DEC COUNT_RX;
			BRNE cycle_read_eeprom_key4


		LDI COUNT_RX, RX_BUFFER_SIZE;

	cycle_usart_key4:
		
		POP R16;

		SBIS UCSRA, UDRE;
			RJMP PC-1
					
		OUT UDR, R16;

		DEC COUNT_RX;
			BRNE cycle_usart_key4

////////////////////////////////////////////////////////////////////////////

//5/////////////////////////////////////////////////////////////////////////

	// READ EEPROM PASSWORD //
read_eeprom_password:

	LDI XL, low(ADDR_EEP_PASSWORD);			//Low address  END BYTE EPPROM for KEY0.
	LDI XH, high(ADDR_EEP_PASSWORD);		//High address END BYTE EPPROM for KEY0.

		LDI COUNT_RX, RX_BUFFER_SIZE;
		DEC COUNT_RX;

		LDI R16, 0x35;	//Code key 5.
		PUSH R16;		// '5'(0x35) -> Stack

	cycle_read_eeprom_password:

		SBIC EECR, EEWE;
			RJMP PC-1;
		
		OUT EEARL, XL;
		OUT EEARH, XH;

		SBI EECR, EERE;		//SET BIT READ EEPROM.

		IN R16, EEDR;
		
		PUSH R16;			//Code key 5 -> Stack

		INC XL;

		DEC COUNT_RX;
			BRNE cycle_read_eeprom_password


		LDI COUNT_RX, RX_BUFFER_SIZE;

	cycle_usart_password:
		
		POP R16;

		SBIS UCSRA, UDRE;
			RJMP PC-1
					
		OUT UDR, R16;

		DEC COUNT_RX;
			BRNE cycle_usart_password

////////////////////////////////////////////////////////////////////////////

	CBI PORT_LED, LED_GREEN;


RETI;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TIM1_COMPA_vect:

	SEI;
	
	NOP;
	
	CLR R16;
	OUT TCNT1H, R16;	//Reset Timer1.
	OUT TCNT1L, R16;
	

	SBI ADCSRA, ADSC	//Start ADC.

	WaitADC:
		SBIC ADCSRA, ADSC
			RJMP WaitADC

	IN R16, ADCH;
	LDI R17, UBAT_CODE;

	CP R16, R17
		BRSH norm_bat
	
			SBI PORT_LED, LED_RED;

			//DELAY// 
			//for led//

			LDI R16, 255	
			DEC R16
			BRNE PC-1

			//END DELAY//

			CBI PORT_LED, LED_RED;

	norm_bat:

RETI;

//END INTERRUPT//
/////////////////////////////////////////////////////////////////////////////////////////

prog_new_key:

		SBI PORT_LED, LED_GREEN;

		CLR FAM;
		CLR KEY_0;
		CLR KEY_1;
		CLR KEY_2;
		CLR KEY_3;
		CLR KEY_4;
		CLR KEY_5;
		CLR CRC;

		
		// START RESET //

		SBI DDRD, DALLAS;

start_reset:

		CBI PORTD, DALLAS;
		
					//DELAY RESSET// 
			//500 us < tRST //

			LDI R16, 255	//
			DEC R16;
			BRNE PC-1;
		
			LDI R16, 255	//
			DEC R16;
			BRNE PC-1;

			LDI R16, 150	//
			DEC R16;
			BRNE PC-1;

		SBI PORTD, DALLAS;


		LDI COUNTER, 100;
		
cycle_reset_high:
				
		LDI R16, 255	//
		DEC R16;

		SBIS PIND, DALLAS
			RJMP new_key_connect		

		BRNE PC-3;


		DEC COUNTER;
			BRNE cycle_reset_high
	
		RJMP start_reset

new_key_connect:
		
/*
	// DECODER PRESENSE PULSE //
		SBIC PIND, DALLAS
			
		INC COUNTER;

	   	SBIS PIND, DALLAS 	
				RJMP PC-2

		
		CPI COUNTER, 120
			BRLO PC+2

			RJMP prog_new_key
			
			NOP
		
		CPI COUNTER, 135
			BRSH PC+2

			RJMP prog_new_key
			
			NOP

	// END DECODER PRESENSE PULSE //

	SBIS UCSRA, UDRE;
	RJMP PC-1
	OUT UDR, COUNTER;
*/

		//DELAY// 
	LDI R16, 255	
	DEC R16;
	BRNE PC-1;

		//DELAY// 
	LDI R16, 255	
	DEC R16;
	BRNE PC-1;


	// START COMMAND 0x33 //

// 1	0
	CBI PORTD, DALLAS

	LDI R16, ONE;
	DEC R16;
	BRNE PC-1

	SBI PORTD, DALLAS
	
		LDI R16, ZERRO;
		DEC R16;
		BRNE PC-1
// 1	1
	CBI PORTD, DALLAS

		LDI R16, ONE;
		DEC R16;
		BRNE PC-1

	SBI PORTD, DALLAS
	
		LDI R16, ZERRO;
		DEC R16;
		BRNE PC-1
// 0	2
	CBI PORTD, DALLAS
		
		LDI R16, ZERRO;
		DEC R16;
		BRNE PC-1

	SBI PORTD, DALLAS

		LDI R16, ONE;
		DEC R16;
		BRNE PC-1
// 0	3
	CBI PORTD, DALLAS
		
		LDI R16, ZERRO;
		DEC R16;
		BRNE PC-1

	SBI PORTD, DALLAS

		LDI R16, ONE;
		DEC R16;
		BRNE PC-1
// 1	4
	CBI PORTD, DALLAS

	LDI R16, ONE;
	DEC R16;
	BRNE PC-1

	SBI PORTD, DALLAS
	
		LDI R16, ZERRO;
		DEC R16;
		BRNE PC-1
// 1	5
	CBI PORTD, DALLAS

		LDI R16, ONE;
		DEC R16;
		BRNE PC-1

	SBI PORTD, DALLAS
	
		LDI R16, ZERRO;
		DEC R16;
		BRNE PC-1
// 0	6
	CBI PORTD, DALLAS
		
		LDI R16, ZERRO;
		DEC R16;
		BRNE PC-1

	SBI PORTD, DALLAS

		LDI R16, ONE;
		DEC R16;
		BRNE PC-1
// 0	7
	CBI PORTD, DALLAS
		
		LDI R16, ZERRO;
		DEC R16;
		BRNE PC-1

	SBI PORTD, DALLAS

		LDI R16, ONE;
		DEC R16;
		BRNE PC-1
	
	// DELAY //
	LDI R16, 100;
	DEC R16;
	BRNE PC-1
	// END DELAY //



	CLR BYTE;
	LDI COUNTER, 8;

start_fam:

	SBI PORTD, DALLAS;
		
	LDI R16, DELAY_ONE;
	DEC R16;
	BRNE PC-1;
	
	SBIS PIND, DALLAS;
		SBR BYTE, 0;
	
	CBI PORTD, DALLAS;

		LDI R16, DELAY_ZERRO;
		DEC R16;
		BRNE PC-1;

	
	LSL BYTE;
	DEC COUNTER;

	BRNE start_fam

	MOV FAM, BYTE;

//

	CLR BYTE;
	LDI COUNTER, 8;

start_key5:

	SBI PORTD, DALLAS;
		
	LDI R16, DELAY_ONE;
	DEC R16;
	BRNE PC-1;
	
	SBIS PIND, DALLAS;
		SBR BYTE, 0;
	
	CBI PORTD, DALLAS;

		LDI R16, DELAY_ZERRO;
		DEC R16;
		BRNE PC-1;

	
	LSL BYTE;
	DEC COUNTER;

	BRNE start_key5

	MOV KEY_5, BYTE;

//
	CLR BYTE;
	LDI COUNTER, 8;

start_key4:

	SBI PORTD, DALLAS;
		
	LDI R16, DELAY_ONE;
	DEC R16;
	BRNE PC-1;
	
	SBIS PIND, DALLAS;
		SBR BYTE, 0;
	
	CBI PORTD, DALLAS;

		LDI R16, DELAY_ZERRO;
		DEC R16;
		BRNE PC-1;
	
	LSL BYTE;
	DEC COUNTER;

	BRNE start_key4

	MOV KEY_4, BYTE;

//
	CLR BYTE;
	LDI COUNTER, 8;

start_key3:

	SBI PORTD, DALLAS;
		
	LDI R16, DELAY_ONE;
	DEC R16;
	BRNE PC-1;
	
	SBIS PIND, DALLAS;
		SBR BYTE, 0;
	
	CBI PORTD, DALLAS;

		LDI R16, DELAY_ZERRO;
		DEC R16;
		BRNE PC-1;
	
	LSL BYTE;
	DEC COUNTER;

	BRNE start_key3

	MOV KEY_3, BYTE;

//
	CLR BYTE;
	LDI COUNTER, 8;

start_key2:

	SBI PORTD, DALLAS;
		
	LDI R16, DELAY_ONE;
	DEC R16;
	BRNE PC-1;
	
	SBIS PIND, DALLAS;
		SBR BYTE, 0;
	
	CBI PORTD, DALLAS;

		LDI R16, DELAY_ZERRO;
		DEC R16;
		BRNE PC-1;
	
	LSL R16;
	DEC COUNTER;

	BRNE start_key2

	MOV KEY_2, BYTE;

//
	CLR BYTE;
	LDI COUNTER, 8;

start_key1:

	SBI PORTD, DALLAS;
		
	LDI R16, DELAY_ONE;
	DEC R16;
	BRNE PC-1;
	
	SBIS PIND, DALLAS;
		SBR BYTE, 0;
	
	CBI PORTD, DALLAS;

		LDI R16, DELAY_ZERRO;
		DEC R16;
		BRNE PC-1;
	
	LSL BYTE;
	DEC COUNTER;

	BRNE start_key1

	MOV KEY_1, BYTE;

//
	CLR BYTE;
	LDI COUNTER, 8;

start_key0:

	SBI PORTD, DALLAS;
		
	LDI R16, DELAY_ONE;
	DEC R16;
	BRNE PC-1;
	
	SBIS PIND, DALLAS;
		SBR BYTE, 0;
	
	CBI PORTD, DALLAS;

		LDI R16, DELAY_ZERRO;
		DEC R16;
		BRNE PC-1;
	
	LSL BYTE;
	DEC COUNTER;

	BRNE start_key0

	MOV KEY_0, BYTE;

//

	CLR BYTE;
	LDI COUNTER, 8;

start_crc:

	SBI PORTD, DALLAS;
		
	LDI R16, DELAY_ONE;
	DEC R16;
	BRNE PC-1;
	
	SBIS PIND, DALLAS;
		SBR BYTE, 0;
	
	CBI PORTD, DALLAS;

		LDI R16, DELAY_ZERRO;
		DEC R16;
		BRNE PC-1;
	
	LSL BYTE;
	DEC COUNTER;

	BRNE start_crc

	MOV CRC, BYTE;

//

//!!!
	
	SBIS UCSRA, UDRE;
	RJMP PC-1
	OUT UDR, FAM;

	SBIS UCSRA, UDRE;
	RJMP PC-1
	OUT UDR, KEY_0;

	SBIS UCSRA, UDRE;
	RJMP PC-1
	OUT UDR, KEY_1;

	SBIS UCSRA, UDRE;
	RJMP PC-1
	OUT UDR, KEY_2;

	SBIS UCSRA, UDRE;
	RJMP PC-1
	OUT UDR, KEY_3;

	SBIS UCSRA, UDRE;
	RJMP PC-1
	OUT UDR, KEY_4;

	SBIS UCSRA, UDRE;
	RJMP PC-1
	OUT UDR, KEY_5;

	SBIS UCSRA, UDRE;
	RJMP PC-1
	OUT UDR, CRC;


//RJMP main
RJMP prog_new_key;


/////////////////////////////////////////////////////////////////////////////////////////

main:

		SEI; 					//Enable Interrupt.

		SBIS PIND, BUT_PROG;
			RJMP prog_new_key;

		SBIC PIND, PC_CONNECT;
			SBI PORT_LED, LED_GREEN;

		SBIS PIND, PC_CONNECT;
			CBI PORT_LED, LED_GREEN;
		
		

		//START DELAY RESET//

		SBIC PIND, DALLAS;	//Delay Start RESET.
			RJMP PC-8;

	
	//480us < tRSTL//

		SBIS PIND, DALLAS;	//Delay End RESET.
			RJMP PC-1;

	//END DELAY RESET//


		//DELAY// 
	//15 us < tPDH < 60 us//

	LDI R16, 80	//	T=10us. 20/0.25=80.
	DEC R16;
	BRNE PC-1;

		
		//END DELAY//



	//START PRESENCE//

	//60us < tPDL < 240us//

	SBI DDRD, DALLAS	//����� PRESENCE ������� ���������� �� �����.		

		LDI R16, 250	//	T=100us. 10/0.25=250. 
		DEC R16
		BRNE PC-1

	CBI DDRD, DALLAS	//���������� ���������� PRESENCE ��������� �����.

//END PRESENCE//


//START READ COMMAND//

	CLR R2;
	CLR R3;
	CLR R4;
	CLR R5;
	CLR R6;
	CLR R7;
	CLR R8;
	CLR R9;
	CLR COMMAND;

//1/////////////////////////////////////////////////////////////////////////

			SBIC PIND, DALLAS	//��� ���� ������ ����.
				RJMP PC-1		//��� � ����� ������ ������� ���� �������
			
			INC R2;
	   
			SBIS PIND, DALLAS 	//��� ���� MASTER ���������� �������.
				RJMP PC-2

//2/////////////////////////////////////////////////////////////////////////

			SBIC PIND, DALLAS	//��� ���� ������ ����.
				RJMP PC-1		//��� � ����� ������ ������� ���� �������
			
			
			INC R3;
	   
			SBIS PIND, DALLAS 	//��� ���� MASTER ���������� �������.
				RJMP PC-2

//3/////////////////////////////////////////////////////////////////////////
			
			SBIC PIND, DALLAS	//��� ���� ������ ����.
				RJMP PC-1		//��� � ����� ������ �������� ���� �������
		
			INC R4;
	   
			SBIS PIND, DALLAS 	//��� ���� MASTER ���������� �������.
				RJMP PC-2
				 	  	
//4/////////////////////////////////////////////////////////////////////////
	
			SBIC PIND, DALLAS	//��� ���� ������ ����.
				RJMP PC-1		//��� � ����� ������ ��������� ���� �������
			
			
			INC R5;
	   
			SBIS PIND, DALLAS 	//��� ���� MASTER ���������� �������.
				RJMP PC-2

//5/////////////////////////////////////////////////////////////////////////
			
			SBIC PIND, DALLAS	//��� ���� ������ ����.
				RJMP PC-1		//��� � ����� ������ ������ ���� �������

			INC R6;
	   
			SBIS PIND, DALLAS 	//��� ���� MASTER ���������� �������.
				RJMP PC-2
 	  	
//6/////////////////////////////////////////////////////////////////////////
			
			SBIC PIND, DALLAS	//��� ���� ������ ����.
				RJMP PC-1		//��� � ����� ������ ������� ���� �������
			
			INC R7;
	   
			SBIS PIND, DALLAS 	//��� ���� MASTER ���������� �������.
				RJMP PC-2
 	  	
//7/////////////////////////////////////////////////////////////////////////
			
			SBIC PIND, DALLAS	//��� ���� ������ ����.
				RJMP PC-1		//��� � ����� ������ �������� ���� �������
			
			INC R8;
	   
			SBIS PIND, DALLAS 	//��� ���� MASTER ���������� �������.
				RJMP PC-2
 	  	
//8/////////////////////////////////////////////////////////////////////////
			
			SBIC PIND, DALLAS	//��� ���� ������ ����.
				RJMP PC-1		//��� � ����� ������ �������� ���� �������
			
			
				INC R9;
	   
			SBIS PIND, DALLAS 	//��� ���� MASTER ���������� �������.
				RJMP PC-2

/////////////////////////////////////////////////////////////////////////
		

//END READ COMMAND//

	SBIS UCSRA, UDRE;
	RJMP PC-1
	OUT UDR, R1;



	SBIS UCSRA, UDRE;
	RJMP PC-1
	OUT UDR, R3;


//START DECODER COMMAND//
	LDI R16, 30;

	CP R2, R16;
		BRSH one_7;

	RJMP PC+2;
one_7:
	SBR COMMAND, 0b10000000;

	CP R3, R16;
		BRSH one_6;

	RJMP PC+2;
one_6:
	SBR COMMAND, 0b01000000;

	CP R4, R16;
		BRSH one_5;

	RJMP PC+2;
one_5:
	SBR COMMAND, 0b00100000;

	CP R5, R16;
		BRSH one_4;

	RJMP PC+2;
one_4:
	SBR COMMAND, 0b00010000;

	CP R6, R16;
		BRSH one_3;

	RJMP PC+2;
one_3:
	SBR COMMAND, 0b00001000;

	CP R7, R16;
		BRSH one_2;

	RJMP PC+2;
one_2:
	SBR COMMAND, 0b00000100;

	CP R8, R16;
		BRSH one_1;

	RJMP PC+2;
one_1:
	SBR COMMAND, 0b00000010;

	CP R9, R16;
		BRSH one_0;

	RJMP PC+2;
one_0:
	SBR COMMAND, 0b00000001;


	CPI COMMAND, 0x33;
		BREQ read_ram;
	

//END DECODER COMMAND//

	RJMP main

read_ram:


//SELECT KEY//


	// KEY0 //
	SBIC PINC, KEY0
		RJMP PC+3

	LDI XL, low(ADDR_EEP_KEY0);			//Low address  END BYTE EPPROM for KEY0.
	LDI XH, high(ADDR_EEP_KEY0);		//High address END BYTE EPPROM for KEY0.


	// KEY1 //
	SBIC PINC, KEY1
		RJMP PC+3

	LDI XL, low(ADDR_EEP_KEY1);			//Low address  END BYTE EPPROM for KEY0.
	LDI XH, high(ADDR_EEP_KEY1);		//High address END BYTE EPPROM for KEY0.


	// KEY2 //
	SBIC PINC, KEY2
		RJMP PC+3

	LDI XL, low(ADDR_EEP_KEY2);			//Low address  END BYTE EPPROM for KEY0.
	LDI XH, high(ADDR_EEP_KEY2);		//High address END BYTE EPPROM for KEY0.


	// KEY3 //
	SBIC PINC, KEY3
		RJMP PC+3

	LDI XL, low(ADDR_EEP_KEY3);			//Low address  END BYTE EPPROM for KEY0.
	LDI XH, high(ADDR_EEP_KEY3);		//High address END BYTE EPPROM for KEY0.


	// KEY4 //
	SBIC PIND, KEY4
		RJMP PC+3

	LDI XL, low(ADDR_EEP_KEY4);			//Low address  END BYTE EPPROM for KEY0.
	LDI XH, high(ADDR_EEP_KEY4);		//High address END BYTE EPPROM for KEY0.


	SBI PORT_LED, LED_RED;


	LDI COUNTER, 8;

	write_ram_key:

		SBIC EECR, EEWE;
			RJMP PC-1;
		
		OUT EEARL, XL;
		OUT EEARH, XH;

		SBI EECR, EERE;		//SET BIT READ EEPROM.

		IN BYTE, EEDR;

		LDI COUNTER_BYTE, 8

send_byte_cycle:

			SBIC PIND, DALLAS	
				RJMP PC-1;	

			SBRS BYTE, 0				//If 0 then pin DALLAS in OUTPUT.
				SBI DDRD, DALLAS;
		
			LDI R16, DELAY_ONE;
			DEC R16;
			BRNE PC-1;	

			CBI DDRD, DALLAS;

			SBIS PIND, DALLAS	
				RJMP PC-1;

			LSR BYTE
			
			DEC COUNTER_BYTE;

			BRNE send_byte_cycle


		INC XL;
		
		DEC COUNTER;
			BRNE write_ram_key

		CBI PORT_LED, LED_RED;

//////////////////////////////////////////////////////////////////////////

RJMP main

/////////////////////////////////////////////////////////////////////////////////////////


// EEPROM //

.eseg	//Write EEPROM.

// Table CRC //
.db 0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65
.db 157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220
.db 35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98
.db 190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255
.db 70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7
.db 219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154
.db 101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36
.db 248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185
.db 140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205
.db 17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80
.db 175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238
.db 50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115
.db 202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139
.db 87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22
.db 233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168
.db 116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
////////////////////////////////////////////////////////////////////////////

// Table KEYS //
/*
//ADDRESS KEY0//
.org ADDR_EEP_KEY0								//START ADDRESS KEY0

.db 0x01								//1-BYTE FAMLY CODE (LOW ADDRESS)
.db 0x0A, 0x55, 0x14, 0x00, 0x00, 0x00	//6-BYTE SERIAL NUMBER KEY1
.db 0x83								//1-BYTE CRC (HIGH ADDRESS)

*/

.org ADDR_EEP_KEY0								//START ADDRESS KEY0

.db 0x01								//1-BYTE FAMLY CODE (LOW ADDRESS)
.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	//6-BYTE SERIAL NUMBER KEY1
.db 0x3D								//1-BYTE CRC (HIGH ADDRESS)

											
//ADDRESS KEY1//
.org ADDR_EEP_KEY1								//START ADDRESS KEY1

.db 0x01								//1-BYTE FAMLY CODE (LOW ADDRESS)
.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	//6-BYTE SERIAL NUMBER KEY1
.db 0x3D								//1-BYTE CRC (HIGH ADDRESS)


//ADDRESS KEY2//
.org ADDR_EEP_KEY2							//START ADDRESS KEY2

.db 0x01								//1-BYTE FAMLY CODE (LOW ADDRESS)
.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	//6-BYTE SERIAL NUMBER KEY2
.db 0x3D								//1-BYTE CRC (HIGH ADDRESS)
			
											
//ADDRESS KEY3//
.org ADDR_EEP_KEY3							//START ADDRESS KEY3

.db 0x01								//1-BYTE FAMLY CODE (LOW ADDRESS)
.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	//6-BYTE SERIAL NUMBER KEY3
.db 0x3D								//1-BYTE CRC (HIGH ADDRESS)


//ADDRESS KEY4//
.org ADDR_EEP_KEY4							//START ADDRESS KEY4

.db 0x01								//1-BYTE FAMLY CODE (LOW ADDRESS)
.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	//6-BYTE SERIAL NUMBER KEY4
.db 0x3D								//1-BYTE CRC (HIGH ADDRESS)


//////////////////////////////////////////////////////////////////////////

// Table PASSWORD //

//ADDRESS PASSWORD//
.org ADDR_EEP_PASSWORD							//START ADDRESS PASSWORD

.db '8','7','6','5','4','3','2','1'				//PASSWORD

//////////////////////////////////////////////////////////////////////////
