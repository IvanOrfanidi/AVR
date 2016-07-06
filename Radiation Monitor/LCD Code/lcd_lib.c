
#define   ENABLE_BIT_DEFINITIONS  
#include <ioavr.h>
#include <intrinsics.h>

#include "lcd_lib.h"
#include "main.h"


//макросы для работы с битами
#define FLAG_BF 7

void LCD_WriteComInit(unsigned char data)
{
  _delay_us(40);
  ClearBit(PORT_SIG_RS, RS);	//установка RS в 0 - команды
#ifdef BUS_4BIT
  unsigned char tmp; 
  tmp  = PORT_DATA & 0x0F;
  tmp |= (data & 0xF0);
  PORT_DATA = tmp;		//вывод старшей тетрады 
#else
  unsigned char data; 
  PORT_DATA = data;		//вывод данных на шину индикатора 
#endif  
  SetBit(PORT_SIG_EN, EN);	        //установка E в 1
  _delay_us(LCD_SPEED);
  ClearBit(PORT_SIG_EN, EN);	//установка E в 0 - записывающий фронт
}


//общая функция
#pragma inline = forced
void LCD_CommonFunc(unsigned char data)
{
#ifdef BUS_4BIT  
  unsigned char tmp; 
  tmp  = PORT_DATA & 0x0F;
  tmp |= (data & 0xF0);

  PORT_DATA = tmp;		//вывод старшей тетрады 
  SetBit(PORT_SIG_EN, EN);	        
  _delay_us(LCD_SPEED);
  ClearBit(PORT_SIG_EN, EN);	

  data = __swap_nibbles(data);
  tmp  = PORT_DATA & 0x0F;
  tmp |= (data & 0xF0);
 
  PORT_DATA = tmp;		//вывод младшей тетрады 
  SetBit(PORT_SIG_EN, EN);	        
  _delay_us(LCD_SPEED);
  ClearBit(PORT_SIG_EN, EN);	 
#else 
  PORT_DATA = data;		//вывод данных на шину индикатора 
  SetBit(PORT_SIG_EN, EN);	        //установка E в 1
  _delay_us(LCD_SPEED);
  ClearBit(PORT_SIG_EN, EN);	//установка E в 0 - записывающий фронт
#endif
}

//функция ожидания готовности lcd
#pragma inline = forced
void LCD_Wait(void)
{
#ifdef CHECK_FLAG_BF
  #ifdef BUS_4BIT
  
  unsigned char data;
  DDRX_DATA &= 0x0F;            //конфигурируем порт на вход
  PORT_DATA |= 0xF0;	        //включаем pull-up резисторы
  SetBit(PORT_SIG_RW, RW);         //RW в 1 чтение из lcd
  ClearBit(PORT_SIG_RS, RS);	//RS в 0 команды
  do{
    SetBit(PORT_SIG_EN, EN);	
    _delay_us(LCD_SPEED);
    data = PIN_DATA & 0xF0;      //чтение данных с порта
    ClearBit(PORT_SIG_EN, EN);
    data = __swap_nibbles(data);
    SetBit(PORT_SIG_EN, EN);	
    _delay_us(LCD_SPEED);
    data |= PIN_DATA & 0xF0;      //чтение данных с порта
    ClearBit(PORT_SIG_EN, EN);
    data = __swap_nibbles(data);
  }while((data & (1<<FLAG_BF))!= 0 );
  ClearBit(PORT_SIG_RW, RW);
  DDRX_DATA |= 0xF0; 
  
  #else
  unsigned char data;
  DDRX_DATA = 0;                //конфигурируем порт на вход
  PORT_DATA = 0xFF;	        //включаем pull-up резисторы
  SetBit(PORT_SIG_RW, RW);         //RW в 1 чтение из lcd
  ClearBit(PORT_SIG_RS, RS);	//RS в 0 команды
  do{
    SetBit(PORT_SIG_EN, EN);	
    _delay_us(LCD_SPEED);
    data = PIN_DATA;            //чтение данных с порта
    ClearBit(PORT_SIG_EN, EN);	
  }while((data & (1<<FLAG_BF))!= 0 );
  ClearBit(PORT_SIG_RW, RW);
  DDRX_DATA = 0xFF; 
  #endif    
#else
  _delay_us(40);
#endif  
}

//функция записи команды 
void LCD_WriteCom(unsigned char data)
{
  LCD_Wait();
  ClearBit(PORT_SIG_RS, RS);	//установка RS в 0 - команды
  LCD_CommonFunc(data);
}

//функция записи данных
void LCD_WriteData(unsigned char data)
{
  LCD_Wait();
  SetBit(PORT_SIG_RS, RS);	        //установка RS в 1 - данные
  LCD_CommonFunc(data);
}

//функция инициализации
void LCD_Init(void)
{
#ifdef BUS_4BIT
  DDRX_DATA |= 0xF0;
  PORT_DATA |= 0xF0; 
#else  
  DDRX_DATA |= 0xFF;
  PORT_DATA |= 0xFF;
#endif
  
  DDRX_SIG_RS |= (1<<RS);
  PORT_SIG_RS |= (1<<RS);
  
  DDRX_SIG_RW |= (1<<RW);
  PORT_SIG_RW |= (1<<RW);
  
  DDRX_SIG_EN |= (1<<EN);
  PORT_SIG_EN |= (1<<EN);
  
  ClearBit(PORT_SIG_RW, RW);
  _delay_ms(40);
  
#ifdef HD44780  
  LCD_WriteComInit(0x30); 
  _delay_ms(10);
  LCD_WriteComInit(0x30);
  _delay_ms(1);
  LCD_WriteComInit(0x30);
#endif
  
#ifdef BUS_4BIT  
  LCD_WriteComInit(0x20); //4-ми разрядная шина
  LCD_WriteCom(0x28); //4-ми разрядная шина, 2 - строки
#else
  LCD_WriteCom(0x38); //8-ми разрядная шина, 2 - строки
#endif
  LCD_WriteCom(0x08);
  LCD_WriteCom(0x0c);  //0b00001111 - дисплей вкл, курсор и мерцание выключены
  LCD_WriteCom(0x01);  //0b00000001 - очистка дисплея
  _delay_ms(LCD_SPEED);
  LCD_WriteCom(0x06);  //0b00000110 - курсор движется вправо, сдвига нет
}

//функция вывода строки из флэш памяти
void LCD_SendStringFlash(unsigned char __flash *str)
{
  unsigned char data;			
  while (*str)
  {
    data = *str++;
    LCD_WriteData(data);
  }
}

//функция вывда строки из ОЗУ
void LCD_SendString(char *str)
{
    unsigned int i=0;
    _Bool bNextString = 0;
    // set first row 
    LCD_WriteCom(DD_RAM_ADDR1);

    // loop to the end of the string
    while(str[i] != 0) {	
      if( (str[i] != '\r') && (str[i] != '\n') )
      {
              LCD_WriteData(str[i]);  
      }
      else
      {
              bNextString = 1;
              LCD_WriteCom(DD_RAM_ADDR2);
      }
      if( (i == 15) && (bNextString == 0) ) {
              LCD_WriteCom(DD_RAM_ADDR2);
      }
      i++;
    }
}

void LCD_Clear(void)
{
  LCD_WriteCom(0x01);
  _delay_ms(LCD_SPEED);
}
