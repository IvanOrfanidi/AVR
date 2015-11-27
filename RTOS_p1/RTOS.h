#ifndef __RTOS
#define __RTOS

	#include <avr/io.h>
	#include <avr/interrupt.h>

// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <math.h>
	#include <stdint.h>

	#define SYS_TIKER		0x00FF
	
	#define PROCES_SYS		2

	
 uint8_t Proc_Num;

 uint8_t Sys_End_Proc;

 uint16_t Sys_Tick_End[PROCES_SYS];

 uint8_t fProc_END;

void InitRTOS(void);

void Sys_Date_Read(uint8_t Proc, char *pSys_Temp_Data);
void Sys_Date_Write(uint8_t Proc, char *pSys_Temp_Data);


#endif
