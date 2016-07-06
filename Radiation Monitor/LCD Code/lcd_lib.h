/*******************************************************************************

*******************************************************************************/
#ifndef LCD_LIB_H
#define LCD_LIB_H

#define   ENABLE_BIT_DEFINITIONS  
#include <ioavr.h>
#include <intrinsics.h>

#define   _delay_ms(temp)(__delay_cycles((temp * F_CPU)/1000));
#define   _delay_us(temp)(__delay_cycles((temp * F_CPU)/1000000UL));

#define ClearBit(reg, bit)       reg &= (~(1<<(bit)))
#define SetBit(reg, bit)         reg |= (1<<(bit))

//порт к которому подключена шина данных ЖКД
#define PORT_DATA PORTC
#define PIN_DATA  PINC
#define DDRX_DATA DDRC

//порт к которому подключены управляющие выводы ЖКД
#define PORT_SIG_RS PORTC
#define PIN_SIG_RS  PINC
#define DDRX_SIG_RS DDRC

#define PORT_SIG_RW PORTC
#define PIN_SIG_RW  PINC
#define DDRX_SIG_RW DDRC


#define PORT_SIG_EN PORTC
#define PIN_SIG_EN  PINC
#define DDRX_SIG_EN DDRC

//Номера выводов к которым подключены управляющие выводы ЖКД 
#define RS 0
#define RW 1
#define EN 2

#define CHECK_FLAG_BF
#define BUS_4BIT
#define HD44780
#define DEPTH_LINE  16
//******************************************************************************
//макросы
#define		DISP_ON			0x0C	//LCD control constants
#define		DISP_OFF		0x08	//
#define		CLR_DISP		0x01	//
#define		DD_RAM_ADDR1		0x80	//
#define		DD_RAM_ADDR2		0xC0	//  
    
#define LCD_SPEED 10 

/*позиционирование курсора*/
#define LCD_Goto(x,y) LCD_WriteCom(((((y)& 1)*0x40)+((x)& 15))|128) 


//прототипы функций
void LCD_Init(void);//инициализация портов и жкд
void LCD_Clear(void);//очистка жкд
void LCD_WriteData(unsigned char data); //выводит байт данных на жкд
void LCD_WriteCom(unsigned char data); //посылает команду жкд
void LCD_SendStringFlash(unsigned char __flash *str); //выводит строку из флэш памяти
void LCD_SendString(char *str); //выводит строку из ОЗУ
void LCD_CommonFunc(unsigned char data);
#endif