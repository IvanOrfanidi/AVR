

//Functions Device.
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/eeprom.h>
	#include <avr/pgmspace.h>
	#include <util/delay.h>


// Standard Input/Output functions.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

	#include "DALLAS.h"	
	#include "USART2.h"


	#define  FT232RL_RST	(1<<PB2)		//Port OUTPUT.
	#define  DCD			PA4				//Port INPUT.
	#define  DTR			(1<<PA5)		//Port OUTPUT.
	#define  CTS			PA6				//Port INPUT.
	#define  RTS			(1<<PA7)		//Port OUTPUT.


	#define  PWR_KEY		PA2				//Port OUTPUT(-_-).
	#define  LED			PD7				//Port OUTPUT.
	#define  PORT_LED		PORTD
	#define  PIN_KEY		PIND
	#define	 KEY0			PD3
	
	//Макросы работающие с GSM модулем SIM300.//
	#define AT			for(uint8_t i=0; i<sizeof(at); i++)	usart0_write(at[i]);	//Trasmit AT.
	#define ATE0 		for(uint8_t i=0; i<sizeof(echo_off); i++)	usart0_write(echo_off[i]);
	#define ATV1 		for(uint8_t i=0; i<sizeof(atv_on); i++)	usart0_write(atv_on[i]);
	#define ATV0 		for(uint8_t i=0; i<sizeof(atv_off); i++)	usart0_write(atv_off[i]);
	#define ATD			for(uint8_t n=0; n<sizeof(atd); n++) usart0_write(atd[n]);

	#define NUMBER0_UCS2	for(uint16_t n=ADDR_EEP_NUM0; n<(sizeof(number0)+ADDR_EEP_NUM0); n++) number0_unicode(n);
	#define NUMBER1_UCS2	for(uint16_t n=ADDR_EEP_NUM1; n<(sizeof(number0)+ADDR_EEP_NUM1); n++) number1_unicode(n);
	#define NUMBER2_UCS2	for(uint16_t n=ADDR_EEP_NUM2; n<(sizeof(number0)+ADDR_EEP_NUM2); n++) number2_unicode(n);
	#define NUMBER3_UCS2	for(uint16_t n=ADDR_EEP_NUM3; n<(sizeof(number0)+ADDR_EEP_NUM3); n++) number3_unicode(n);

	#define ENTER_ATD	for(uint8_t n=0; n<sizeof(atd_enter); n++) usart0_write(atd_enter[n]);
	#define SMS			for(uint8_t n=0; n<sizeof(sms_send); n++) usart0_write(sms_send[n]);
	#define	CMGF1		for(uint8_t n=0; n<sizeof(format_text); n++) usart0_write(format_text[n]);
	#define ENTER_SMS 	for(uint8_t n=0; n<sizeof(sms_enter); n++) usart0_write(sms_enter[n]);				
	#define END_SMS 	for(uint8_t n=0; n<sizeof(sms_end); n++) usart0_write(sms_end[n]); //Ctrl+Z
	#define CSCS_UCS2	for(uint8_t n=0; n<sizeof(te_format_ucs2); n++) usart0_write(te_format_ucs2[n]);

	#define NUMBER0_GSM	for(uint16_t n=ADDR_EEP_NUM0; n<(sizeof(number0)+ADDR_EEP_NUM0); n++) usart0_write(eeprom_read_byte(n));
	#define NUMBER1_GSM	for(uint16_t n=ADDR_EEP_NUM1; n<(sizeof(number1)+ADDR_EEP_NUM1); n++) usart0_write(eeprom_read_byte(n));
	#define NUMBER2_GSM	for(uint16_t n=ADDR_EEP_NUM2; n<(sizeof(number2)+ADDR_EEP_NUM2); n++) usart0_write(eeprom_read_byte(n));
	#define NUMBER3_GSM	for(uint16_t n=ADDR_EEP_NUM3; n<(sizeof(number3)+ADDR_EEP_NUM3); n++) usart0_write(eeprom_read_byte(n));
	
	//Макрос запускающий Таймер.
	#define START_TIMER0 TCCR0=(1<<CS02)|(0<<CS01)|(1<<CS00);	//Timer0 start clock.
	#define STOP_TIMER0 TCCR0=(0<<CS02)|(0<<CS01)|(0<<CS00);	//Timer0 stop clock.

	#define ADDR_EEP_PAS  0x0000

	#define ADDR_EEP_NUM0 0x0008
	#define ADDR_EEP_NUM1 0x0014
	#define ADDR_EEP_NUM2 0x0020
	#define ADDR_EEP_NUM3 0x002C

	//////////////////////////////////////////////////////////////////////////////////

	#define ADDR_EEP_SMS0 0x0038
	#define ADDR_EEP_SMS1 0x007F
	#define ADDR_EEP_SMS2 0x00C6
	#define ADDR_EEP_SMS3 0x010D
	
		
	const char at[]={'A', 'T', 0x0D, 0x0A};
	const char echo_off[]={'A','T','E', '0', 0x0D, 0x0A};
	const char atv_off[]={'A','T','V', '0', 0x0D, 0x0A};
	const char atv_on[]={'A','T','V', '1', 0x0D, 0x0A};
	const char format_text[]={'A','T','+','C','M','G','F','=','1', 0x0D, 0x0A};

	const char atd_enter[]={';', 0x0D, 0x0A};
	const char sms_enter[]={'"', 0x0D, 0x0A};

	const char sms_end[]={0x1A};
	const char atd[4]={'A','T','D'};

	const char sms_send[]={'A','T','+','C','M','G','S','=','"'};

	const char te_format_ucs2[]={'A','T','+','C','S','C','S','=','"','U','C','S','2','"', 0x0D, 0x0A};
	
	char pasvord[]EEMEM={'1','2','3','4','5','6','7','8'};
	char number0[]EEMEM={'+','7','9','5','1','6','8','3','1','1','4','6'};
	char number1[]EEMEM={'+','7','9','5','1','6','8','3','1','1','4','6'};
	char number2[]EEMEM={'+','7','9','5','1','6','8','3','1','1','4','6'};
	char number3[]EEMEM={'+','7','9','5','1','6','8','3','1','1','4','6'};


	char text_sms0[]EEMEM="AБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЭЮЯабвгдежзийклмнопрстуфхцчшщьъыэюя000000"; // Текст нашей smsки.
	char text_sms1[]EEMEM="BБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЭЮЯабвгдежзийклмнопрстуфхцчшщьъыэюя111111"; // Текст нашей smsки.
	char text_sms2[]EEMEM="CБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЭЮЯабвгдежзийклмнопрстуфхцчшщьъыэюя222222"; // Текст нашей smsки.
	char text_sms3[]EEMEM="DБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЭЮЯабвгдежзийклмнопрстуфхцчшщьъыэюя333333"; // Текст нашей smsки.

void init(void){

// Port A initialization

DDRA=(1<<PA7)|(0<<PA6)|(1<<PA5)|(0<<PA4)|(0<<PA3)|(1<<PA2)|(0<<PA1)|(0<<PA0);

PORTA=0xFF & (0<<PWR_KEY);

//////////////////////////////////////////////////////////////////////////////

PORTB=(1<<PB7);
DDRB=0x00;


PORTC=0x00;
DDRC=0xC0;

DDRD=(1<<LED);
PORTD=(1<<LED);


//Analog Comparator initialization.

	ACSR|=_BV(ACD);		//Disable Comparator.

/////////////////////////////////////////////////////////////////////

//Init Timer0//

TIMSK|=(1<<TOIE0);

/////////////////////////////////////////////////////////////////////

}

int main()
{



	init();

	PORTA|=(1<<PWR_KEY);
	_delay_ms(2000);
	PORTA&=(0<<PWR_KEY);
	_delay_ms(5000);
	PORTA|=(1<<PWR_KEY);


	usart0_init();

	sei();

	_delay_ms(3000);

	PORT_LED&=~(1<<LED);

#if 0
	while(at_ok())				//Проверяем готовность модуля командой AT.
	{ 							//Если все правильно и скорости совпадают вернтся ответ "0".
		at_ok();
									
		ATV0;	//Макрос отключающий словесный формат ответа. Т.е. теперь будут приходить коды ответа.

			_delay_ms(1000);
		
		ATE0;	//Макрос отключающий эхо от модуля.

			_delay_ms(1000);
	}

#endif

	CMGF1;			//Макрос уставливает режим PDU.

	//while(ok());
	_delay_ms(1000);

	CSCS_UCS2;		//Макрос устанавливает кодировку Юникод. По умолчанию GSM.

	//while(ok());	//Ждем ОК (! ОК прийдет уже не как "OK", а как "0").

	_delay_ms(1000);


	
		usart0_clear_rx_buffer();
		usart0_clear_tx_buffer();

	//	ATD;
	//	NUMBER3_GSM;
	//	ENTER_ATD;

	while(1)
	{

		if(!(PIN_KEY & (1<<KEY0)))
		{
			sms_tx(0,0);
						for(uint8_t p=0; p<5; p++) _delay_ms(1000);

			sms_tx(0,1);
						for(uint8_t p=0; p<5; p++) _delay_ms(1000);

			sms_tx(0,2);
						for(uint8_t p=0; p<5; p++) _delay_ms(1000);

			sms_tx(0,3);
						for(uint8_t p=0; p<5; p++) _delay_ms(1000);


			PORT_LED&=~(1<<LED);

			for(uint8_t p=0; p<10; p++) _delay_ms(1000);

			usart0_clear_rx_buffer();
			usart0_clear_tx_buffer();

			ATD;
			NUMBER0_GSM;
			ENTER_ATD;	

		}
			else
			{
				PORT_LED|=(1<<LED);
			}

	}

return 0;

}


int at_ok()
{
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

		AT;

		uint8_t ok_true=0;

		while(usart0_rx_len()==0);

		if(usart0_read()=='0') return 0;
		
		usart0_clear_rx_buffer();
		return 1;

	
}

int ok()
{
	while(!(usart0_rx_len()));
	if(usart0_read()=='0') return 0;
	
	return usart0_read();
}

//Функция преобразуещая наш номер в ASCII символах в юникод(формат UCS2).//
//И отправляет его на usart.//
//!!!Теперь и номер нужно записать в юникоде.//

void number0_unicode(uint16_t num)
{
	usart0_clear_tx_buffer();	//Clear Tx buffer.
	usart0_clear_rx_buffer();	//Clear Rx buffer.


	if(eeprom_read_byte(num)=='+')	//Если в начале номера не цифра, а + (+7951xxxxxxx-например).
	{
		char plus[]="002B";
		for(uint8_t t1=0; t1<sizeof(plus); t1++) usart0_write(plus[t1]);
	}  
		else
		{
			char not_plus[]="003";
			for(uint8_t t1=0; t1<sizeof(not_plus); t1++) usart0_write(not_plus[t1]);

			usart0_write(eeprom_read_byte(num));
		}
	_delay_ms(100);

}

void number1_unicode(uint16_t num)
{
	usart0_clear_tx_buffer();	//Clear Tx buffer.
	usart0_clear_rx_buffer();	//Clear Rx buffer.


	if(eeprom_read_byte(num)=='+')	//Если в начале номера не цифра, а + (+7951xxxxxxx-например).
	{
		char plus[]="002B";
		for(uint8_t t1=0; t1<sizeof(plus); t1++) usart0_write(plus[t1]);
	}  
		else
		{
			char not_plus[]="003";
			for(uint8_t t1=0; t1<sizeof(not_plus); t1++) usart0_write(not_plus[t1]);

			usart0_write(eeprom_read_byte(num));
		}
	_delay_ms(100);

}

void number2_unicode(uint16_t num)
{
	usart0_clear_tx_buffer();	//Clear Tx buffer.
	usart0_clear_rx_buffer();	//Clear Rx buffer.


	if(eeprom_read_byte(num)=='+')	//Если в начале номера не цифра, а + (+7951xxxxxxx-например).
	{
		char plus[]="002B";
		for(uint8_t t1=0; t1<sizeof(plus); t1++) usart0_write(plus[t1]);
	}  
		else
		{
			char not_plus[]="003";
			for(uint8_t t1=0; t1<sizeof(not_plus); t1++) usart0_write(not_plus[t1]);

			usart0_write(eeprom_read_byte(num));
		}
	_delay_ms(100);

}

void number3_unicode(uint16_t num)
{
	usart0_clear_tx_buffer();	//Clear Tx buffer.
	usart0_clear_rx_buffer();	//Clear Rx buffer.


	if(eeprom_read_byte(num)=='+')	//Если в начале номера не цифра, а + (+7951xxxxxxx-например).
	{
		char plus[]="002B";
		for(uint8_t t1=0; t1<sizeof(plus); t1++) usart0_write(plus[t1]);
	}  
		else
		{
			char not_plus[]="003";
			for(uint8_t t1=0; t1<sizeof(not_plus); t1++) usart0_write(not_plus[t1]);

			usart0_write(eeprom_read_byte(num));
		}
	_delay_ms(100);

}

//Функция переводит ASCII символа в два байта юникода(формат UCS2).
int text_unicode(uint16_t word)	
{
	if(word>0x00BF)
	{
		word+=0x0350;	//прибавляем к ASCII коду 0x350.
						//Почему 0x350? Потому что русские символы начинаются с 0x0410.					//А=0x0C, 0x0C+0x0350=0x0410. А далее они идут по порядку, кроме буквы Ё.		
	}

return word;	//Возвращаем юникод, далее его нужно преобразовать в hex ascii код.
				//Сделаем это в функции text_unicode_hex.
}


//Функция преобразуещая юникод в hex ascii и отправля один символ.//
//Символ отправляется 4-мя байтами.//
void text_unicode_hex(uint16_t word)
{

	char data_tx[4];

	uint8_t byte;

		byte=word>>12;
		byte=byte+0x30;
		data_tx[0]=byte;

		byte=word>>8;
		byte&=0x0F;
		byte=byte+0x30;
		data_tx[1]=byte;

		byte=word>>4;
		byte&=0x0F;
		byte=byte+0x30;
		data_tx[2]=byte;

		byte=word;
		byte&=0x0F;
		if(byte<0x0A)
		{
			byte=byte+0x30;
			data_tx[3]=byte;
		}
			else
			{
				byte=byte+0x37;
				data_tx[3]=byte;
			}

	for(uint8_t tx=0; tx<4; tx++) usart0_write(data_tx[tx]);	//Отправка 4 байт, это есть один символ.
}


int sms_tx(uint16_t num_tel, uint16_t num_sms)
{

	usart0_clear_tx_buffer();	//Clear Tx buffer.
	usart0_clear_rx_buffer();	//Clear Rx buffer.

	CMGF1;			//Макрос уставливает режим PDU.
	
	uint8_t status=1;
	while(status)
	{
		status=ok();
		if(status=='4') return 0;
	}

	SMS;			//Говорим модему, что хотим послать SMS.

	_delay_ms(100);

	if(num_tel==0) NUMBER0_UCS2; //На номер #0.
	if(num_tel==1) NUMBER1_UCS2; //На номер #1.
	if(num_tel==2) NUMBER2_UCS2; //На номер #2.
	if(num_tel==3) NUMBER3_UCS2; //На номер #3.


	ENTER_SMS;		//После этой команды вводим текст смски.

	while(!(usart0_rx_len()));	//Ждем приглашение от модуля.
	while(usart0_read()!='>');	//Как только прийдет ">" можно будет вводить текст смс.

	usart0_clear_tx_buffer();	//Clear Tx buffer.

	if(num_sms==0)
	{
		uint8_t len=0;

		for(uint16_t i=ADDR_EEP_SMS0; i<(0x47+ADDR_EEP_SMS0); i++)
		{
			if(eeprom_read_byte(i))	len++;
		}

		char text_sms[len];
		uint8_t ii=0;
		for(uint16_t i=ADDR_EEP_SMS0; i<(len+ADDR_EEP_SMS0); i++)
		{
			text_sms[ii]=eeprom_read_byte(i);
			ii++;
		}

		uint16_t text_ucs2[len];

		for(uint16_t i=0; i<len; i++) text_ucs2[i]=text_unicode(text_sms[i]);
		for(uint16_t i=0; i<len; i++) text_unicode_hex(text_ucs2[i]);

	}


	if(num_sms==1)
	{
		uint8_t len=0;

		for(uint16_t i=ADDR_EEP_SMS1; i<(0x47+ADDR_EEP_SMS1); i++)
		{
			if(eeprom_read_byte(i))	len++;
		}

		char text_sms[len];
		uint8_t ii=0;
		for(uint16_t i=ADDR_EEP_SMS1; i<(len+ADDR_EEP_SMS1); i++)
		{
			text_sms[ii]=eeprom_read_byte(i);
			ii++;
		}

		uint16_t text_ucs2[len];

		for(uint16_t i=0; i<len; i++) text_ucs2[i]=text_unicode(text_sms[i]);
		for(uint16_t i=0; i<len; i++) text_unicode_hex(text_ucs2[i]);

	}
	if(num_sms==2)
	{
		uint8_t len=0;

		for(uint16_t i=ADDR_EEP_SMS2; i<(0x47+ADDR_EEP_SMS2); i++)
		{
			if(eeprom_read_byte(i))	len++;
		}

		char text_sms[len];
		uint8_t ii=0;
		for(uint16_t i=ADDR_EEP_SMS2; i<(len+ADDR_EEP_SMS2); i++)
		{
			text_sms[ii]=eeprom_read_byte(i);
			ii++;
		}

		uint16_t text_ucs2[len];

		for(uint16_t i=0; i<len; i++) text_ucs2[i]=text_unicode(text_sms[i]);
		for(uint16_t i=0; i<len; i++) text_unicode_hex(text_ucs2[i]);

	}

	if(num_sms==3)
	{
		uint8_t len=0;

		for(uint16_t i=ADDR_EEP_SMS3; i<(0x47+ADDR_EEP_SMS3); i++)
		{
			if(eeprom_read_byte(i))	len++;
		}

		char text_sms[len];
		uint8_t ii=0;
		for(uint16_t i=ADDR_EEP_SMS3; i<(len+ADDR_EEP_SMS3); i++)
		{
			text_sms[ii]=eeprom_read_byte(i);
			ii++;
		}

		uint16_t text_ucs2[len];

		for(uint16_t i=0; i<len; i++) text_ucs2[i]=text_unicode(text_sms[i]);
		for(uint16_t i=0; i<len; i++) text_unicode_hex(text_ucs2[i]);

	}


	END_SMS;					//Макрос завершающий отправку смс.

	
	while(ok());
	_delay_ms(100);				//Ждем ОК.

return 1;

}

//INTERRUPT//

ISR(TIMER0_OVF_vect){

	if(dallas(0x33))
	{
		uint8_t data1[8]={0x01, 0x55, 0xAA, 0x55, 0x24, 0xAE, 0x05, 0x28};

		
		PORT_LED&=~LED;

		for(uint8_t i=0; i<8; i++)
		{
			if(data_dallas[i]!=data1[i]) 	PORT_LED|=LED;
		}

	}

}

//////////////////////////////////////////////////////////////////////////////
