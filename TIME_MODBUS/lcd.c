#include <avr/io.h>
#include "lcd.h"
#include <util/delay.h>

#define		RS_HIGH			PORT_LCD |= (1<<_RS )
#define		RS_LOW			PORT_LCD &= ~(1<<_RS)

#define		RW_HIGH			PORT_LCD |= (1<<RW)
#define		RW_LOW			PORT_LCD &= ~(1<<RW)

#define		E_HIGH			PORT_LCD |= (1<<E)
#define		E_LOW			PORT_LCD &= ~(1<<E)


#define		LCD_CHAR_SPEED		2

  

unsigned char data_lcd;

void _E (void) {
	E_HIGH;
	_delay_us(5);
	E_LOW;
}



//--------Initialize LCD-----------------------------
void LCD_Ini(void) {

	RS_LOW;				
	_delay_ms(110);	
	PORT_LCD=0b00110000;		
	_E();	
	_delay_ms(10);	
	PORT_LCD=0b00110000;
	_E();
	_delay_ms(10);
	PORT_LCD=0b00110000;
	_E();
	_delay_ms(10);
	PORT_LCD=0b00100000;
	_E();

}      
    
//--------Send char to LCD-----------------------------
void SEND_CHAR (unsigned char c){
    
	_delay_ms(LCD_CHAR_SPEED);              	//Delay for LCD char
	data_lcd = 0x0F | c;			    		//get upper nibble
	PORT_LCD = (PORT_LCD | 0xF0) & data_lcd;   	 //set D4-D7
	RS_HIGH;		  			    		//set LCD to data mode
	_E();
	data_lcd = c << 4;				    		//get down nibble
	PORT_LCD = (PORT_LCD & 0x0F) | data_lcd;	//set D4-D7 (only PORT_LCD4-PORT_LCD)
	RS_HIGH;					    	   	//set LCD to data mode
	_E();
}               
    

//--------Send command to LCD-----------------------------
void SEND_CMD (unsigned char c) {
    
	_delay_ms(LCD_CHAR_SPEED);              	//Delay for LCD char
	data_lcd = 0x0F | c;			    	//get upper nibble
	PORT_LCD = (PORT_LCD | 0xF0) & data_lcd;   	 //set D4-D7
	RS_LOW;					    			//set LCD to data mode
	_E();    
	data_lcd = c << 4;				    		//get down nibble
	PORT_LCD = (PORT_LCD & 0x0F) | data_lcd;	//set D4-D7 (only PORT_LCD-PORT_LCD)
	RS_LOW;					    			//set LCD to data mode
	_E();
}                    

//--------Send string to LCD-----------------------------
void SEND_STR(unsigned char* str) {

	uint8_t i=0;

	// loop to the end of the string
	while(str[i]!='\0') {	
		
		SEND_CHAR(str[i]);  
		i++;
	}

}
