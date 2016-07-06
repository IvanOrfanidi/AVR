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

//���� � �������� ���������� ���� ������ ���
#define PORT_DATA PORTC
#define PIN_DATA  PINC
#define DDRX_DATA DDRC

//���� � �������� ���������� ����������� ������ ���
#define PORT_SIG_RS PORTC
#define PIN_SIG_RS  PINC
#define DDRX_SIG_RS DDRC

#define PORT_SIG_RW PORTC
#define PIN_SIG_RW  PINC
#define DDRX_SIG_RW DDRC


#define PORT_SIG_EN PORTC
#define PIN_SIG_EN  PINC
#define DDRX_SIG_EN DDRC

//������ ������� � ������� ���������� ����������� ������ ��� 
#define RS 0
#define RW 1
#define EN 2

#define CHECK_FLAG_BF
#define BUS_4BIT
#define HD44780
#define DEPTH_LINE  16
//******************************************************************************
//�������
#define		DISP_ON			0x0C	//LCD control constants
#define		DISP_OFF		0x08	//
#define		CLR_DISP		0x01	//
#define		DD_RAM_ADDR1		0x80	//
#define		DD_RAM_ADDR2		0xC0	//  
    
#define LCD_SPEED 10 

/*���������������� �������*/
#define LCD_Goto(x,y) LCD_WriteCom(((((y)& 1)*0x40)+((x)& 15))|128) 


//��������� �������
void LCD_Init(void);//������������� ������ � ���
void LCD_Clear(void);//������� ���
void LCD_WriteData(unsigned char data); //������� ���� ������ �� ���
void LCD_WriteCom(unsigned char data); //�������� ������� ���
void LCD_SendStringFlash(unsigned char __flash *str); //������� ������ �� ���� ������
void LCD_SendString(char *str); //������� ������ �� ���
void LCD_CommonFunc(unsigned char data);
#endif