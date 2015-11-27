

	#include <avr/io.h>
	#include <avr/interrupt.h>

	uint16_t stopbuzzer;
	uint16_t t;
	
	#define BUZ					PD4		//Pin Buzzer.
	
	#define ON 1
	#define OFF 0


void buzzer_timer(uint16_t Buz_Freq, uint16_t Buz_Stop);
