#ifndef __LCD
#define __LCD


#define PORT_LCD PORTD

#define _RS	PD1
#define E	PD3
#define RW	PD2


#define		DISP_ON				0b00001100	//LCD control constants
#define		DISP_OFF			0b00001000	//

#define		DISP_CUR1			0b00001110
#define		DISP_CUR2			0b00001101

#define		CLR_DISP			0b00000001	//Clear LCD
#define		CUR_HOME			0b00000010	//0x0D & 0x0A
#define		ENTRY_INC			0b00000110	//
#define		DD_RAM_ADDR			0b10000000	//
#define		DD_RAM_ADDR1		0b10000000	//
#define		DD_RAM_ADDR2		0b11000000	//  
#define		DD_RAM_ADDR3		40			//      
#define		CG_RAM_ADDR			0b01000000	// 

//--------Initialize LCD-----------------------------
void LCD_Ini(void);
    
//--------Send char to LCD-----------------------------
void SEND_CHAR (unsigned char c);

//--------Send command to LCD-----------------------------
void SEND_CMD (unsigned char c); 

//--------Send string to LCD-----------------------------
void SEND_STR(unsigned char* str);

#endif


