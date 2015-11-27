

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
	#include "USART.h"
	#include "Buzzer.h"
	#include "NTP.h"
	#include "RTC.h"
	#include "BCD.h"


	#define  FT232RL_RST	(1<<PB2)		//Port OUTPUT.
	#define  DCD			PA4				//Port INPUT.
	#define  DTR			(1<<PA5)		//Port OUTPUT.
	#define  CTS			PA6				//Port INPUT.
	#define  RTS			(1<<PA7)		//Port OUTPUT.

	#define RELAY1			(1<<PC7)
	#define RELAY2			(1<<PC6)
	#define PORT_OUT		PORTC


	#define  PWR_KEY		PA2				//Port OUTPUT(-_-).
	#define  LED			PD7				//Port OUTPUT.
	#define  PORT_LED		PORTD
	#define  PIN_KEY		PIND
	#define	 KEY0			PD3
	
	//Макросы работающие с GSM модулем SIM300.//
	#define AT			for(uint8_t n=0; n<sizeof(at); n++)	usart0_write(at[n]);	//AT.
	#define ATE0 		for(uint8_t n=0; n<sizeof(echo_off); n++)	usart0_write(echo_off[n]);
	#define ATE1 		for(uint8_t n=0; n<sizeof(echo_on); n++)	usart0_write(echo_on[n]);
	#define ATV1 		for(uint8_t n=0; n<sizeof(atv_on); n++)	usart0_write(atv_on[n]);
	#define ATV0 		for(uint8_t n=0; n<sizeof(atv_off); n++)	usart0_write(atv_off[n]);
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
	#define CMGR		for(uint8_t n=0; n<sizeof(sms_read); n++) usart0_write(sms_read[n]);
	#define CMGD		for(uint8_t n=0; n<sizeof(sms_del); n++) usart0_write(sms_del[n]);


	#define NUMBER0_GSM	for(uint16_t n=ADDR_EEP_NUM0; n<(sizeof(number0)+ADDR_EEP_NUM0); n++) usart0_write(eeprom_read_byte(n));
	#define NUMBER1_GSM	for(uint16_t n=ADDR_EEP_NUM1; n<(sizeof(number1)+ADDR_EEP_NUM1); n++) usart0_write(eeprom_read_byte(n));
	#define NUMBER2_GSM	for(uint16_t n=ADDR_EEP_NUM2; n<(sizeof(number2)+ADDR_EEP_NUM2); n++) usart0_write(eeprom_read_byte(n));
	#define NUMBER3_GSM	for(uint16_t n=ADDR_EEP_NUM3; n<(sizeof(number3)+ADDR_EEP_NUM3); n++) usart0_write(eeprom_read_byte(n));

	#define SMS_MAX_LEN 53


	//Макрос запускающий Таймер 0.
	#define START_TIMER0 TCCR0=(1<<CS02)|(0<<CS01)|(1<<CS00);	//Timer0 start clock.
	#define STOP_TIMER0 TCCR0=(0<<CS02)|(0<<CS01)|(0<<CS00);	//Timer0 stop clock.

	//Макрос запускающий Таймер 2.
	#define START_TIMER2 TCCR2=(0<<CS22)|(0<<CS21)|(1<<CS20);	//Timer2 start clock.
	#define STOP_TIMER2 TCCR2=(0<<CS22)|(0<<CS21)|(0<<CS20);	//Timer2 stop clock.


	//АДРЕСА ПАМЯТИ В ЕЕПРОМЕ//

	//ПАРОЛЬ//
	#define ADDR_EEP_PAS  0x0000
	//////////////////////////////////////////////////////////////////////////////////

	//НОМЕРА ТЕЛЕФОНОВ//
	#define ADDR_EEP_NUM0 0x0008
	#define ADDR_EEP_NUM1 0x0014
	#define ADDR_EEP_NUM2 0x0020
	#define ADDR_EEP_NUM3 0x002C
	//////////////////////////////////////////////////////////////////////////////////

	//ТЕКСТЫ СМСОК//
	#define ADDR_EEP_SMS0 0x0038
	#define ADDR_EEP_SMS1 0x006D
	#define ADDR_EEP_SMS2 0x00A2
	#define ADDR_EEP_SMS3 0x00D7
	//////////////////////////////////////////////////////////////////////////////////



	const char at[]={'A', 'T', 0x0D, 0x0A};
	const char echo_off[]={'A','T','E', '0', 0x0D, 0x0A};
	const char echo_on[]={'A','T','E', '1', 0x0D, 0x0A};

	const char atv_off[]={'A','T','V', '0', 0x0D, 0x0A};
	const char atv_on[]={'A','T','V', '1', 0x0D, 0x0A};
	const char format_text[]={'A','T','+','C','M','G','F','=','1', 0x0D, 0x0A};

	const char atd_enter[]={';', 0x0D, 0x0A};
	const char sms_enter[]={'"', 0x0D, 0x0A};

	const char sms_end[]={0x1A};
	const char atd[4]={'A','T','D'};

	const char sms_send[]={'A','T','+','C','M','G','S','=','"'};

	const char te_format_ucs2[]={'A','T','+','C','S','C','S','=','"','U','C','S','2','"', 0x0D, 0x0A};

	const char sms_read[]={'A','T','+','C','M','G','R','=','1', 0x0D, 0x0A};
	const char sms_del[]={'A','T','+','C','M','G','D','=','1', 0x0D, 0x0A};
	
	const char zone1[]={'0','0','3','1'};
	const char zone2[]={'0','0','3','2'};
	const char zone3[]={'0','0','3','3'};
	const char zone4[]={'0','0','3','4'};

	const char zone_on[]={'0','0','3','0'};
	const char zone_of[]={'0','0','3','1'};


	char password[]EEMEM={'1','2','3','4','5','6','7','8'};
	char number0[]EEMEM={'+','7','9','5','1','6','8','3','1','1','4','6'};
	char number1[]EEMEM={'+','7','9','5','1','6','8','3','1','1','4','6'};
	char number2[]EEMEM={'+','7','9','5','1','6','8','3','1','1','4','6'};
	char number3[]EEMEM={'+','7','9','5','1','6','8','3','1','1','4','6'};


	char text_sms0[]EEMEM="AБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЭЮЯабвгдежзийклмнопрсту"; // Текст нашей smsки.
	char text_sms1[]EEMEM="BБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЭЮЯабвгдежзийклмнопрсту"; // Текст нашей smsки.
	char text_sms2[]EEMEM="CБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЭЮЯабвгдежзийклмнопрсту"; // Текст нашей smsки.
	char text_sms3[]EEMEM="DБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЭЮЯабвгдежзийклмнопрсту"; // Текст нашей smsки.

	uint8_t temp_sreg;

void init(void){

	// Port A initialization

	DDRA=(1<<PA7)|(0<<PA6)|(1<<PA5)|(0<<PA4)|(0<<PA3)|(1<<PA2)|(0<<PA1)|(0<<PA0);

	PORTA=0xFF & (0<<PWR_KEY);

	//////////////////////////////////////////////////////////////////////////////

	PORTB=(1<<PB7);
	DDRB=0x00;


	PORTC=0x00;
	DDRC=(RELAY1)|(RELAY2);

	DDRD=(1<<LED)|(1<<PD4);
	PORTD=(1<<LED);


	//Analog Comparator initialization.

		ACSR|=_BV(ACD);		//Disable Comparator.

	/////////////////////////////////////////////////////////////////////

	//Init Timer0//

	TIMSK|=(1<<TOIE0);

	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////

	//Init Timer2//

	TIMSK|=(1<<TOIE2);

	/////////////////////////////////////////////////////////////////////

}

int main()
{

	init();

	usart0_init();

	sei();

	for(uint8_t p=0; p<5; p++) _delay_ms(1000);

	while(at_ok())			//Проверяем готовность модуля командой AT.
	{
		PORTA|=(1<<PWR_KEY);
	for(uint8_t p=0; p<2; p++) _delay_ms(1000);
		PORTA&=(0<<PWR_KEY);
	for(uint8_t p=0; p<5; p++) _delay_ms(1000);
		PORTA|=(1<<PWR_KEY);

		//Если все правильно и скорости совпадают вернтся ответ "0".

		at_ok();
									
		ATV0;	//Макрос отключающий словесный формат ответа. Т.е. теперь будут приходить коды ответа.

			_delay_ms(1000);
		
		ATE0;	//Макрос отключающий эхо от модуля.

			_delay_ms(1000);
	}

//	CMGD;
//	_delay_ms(1000);

	GetTimeSIM();
	_delay_ms(1000);

	GetTimeSIM();
	_delay_ms(1000);


//	if(GetTimeNTP(1)!=0) GetTimeNTP(2);
//	_delay_ms(1000);


//	SetTimeSIM();
//	while(ok());

//	GetTimeSIM();
//	_delay_ms(1000);

//	GetTimeSIM();

/*
	ATE1;
	_delay_ms(1000);

usart0_write(cyear[0]);
usart0_write(cyear[1]);

usart0_write(cmonth[0]);
usart0_write(cmonth[1]);

usart0_write(cday[0]);
usart0_write(cday[1]);



usart0_write(chours[0]);
usart0_write(chours[1]);

usart0_write(cmin[0]);
usart0_write(cmin[1]);

usart0_write(csec[0]);
usart0_write(csec[1]);


//	for(uint8_t i=0; i<sizeof(data_time); i++) usart0_write(data_time[i]);

	while(1);
*/

	CMGF1;			//Макрос уставливает режим PDU.

	while(ok());
	_delay_ms(1000);

	CSCS_UCS2;		//Макрос устанавливает кодировку Юникод. По умолчанию GSM.

	while(ok());	//Ждем ОК (! ОК прийдет уже не как "OK", а как "0").

	_delay_ms(1000);

//	ATE0;
//	_delay_ms(1000);

	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	PORT_OUT|=RELAY1;

	while(1)
	{

		sei();

		if(usart0_rx_len())
		{

			SMSRxData();

			_delay_ms(1000);
			usart0_clear_tx_buffer();
			usart0_clear_rx_buffer();


			while(usart0_rx_len()) usart0_clear_rx_buffer();
		 	
	
			PORT_LED|=(1<<LED);

		}



		if(!(PIN_KEY & (1<<KEY0))) Alarm(); else PORT_LED|=(1<<LED);

		if(dallas(0x33)) Identification();

	}


}

//////////////////////////////////////////////////////////////////////////////

int at_ok()
{
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

		AT;

		while(usart0_rx_len()==0);

		if(usart0_read()=='0') return 0;
		
		usart0_clear_rx_buffer();
		return 1;

	
}

//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////

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


//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////

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
		uint8_t len=SMS_MAX_LEN;

		char text_sms[(len+sizeof(data_time))];
		uint8_t ii=0;

		for(uint16_t i=ADDR_EEP_SMS0; i<(len+ADDR_EEP_SMS0); i++)
		{
			text_sms[ii]=eeprom_read_byte(i);
			ii++;
		}

		for(uint8_t i=0; i<sizeof(data_time); i++) text_sms[(i+len)]=data_time[i];



		uint16_t text_ucs2[sizeof(text_sms)];

		for(uint16_t i=0; i<sizeof(text_sms); i++) text_ucs2[i]=text_unicode(text_sms[i]);
		for(uint16_t i=0; i<sizeof(text_sms); i++) text_unicode_hex(text_ucs2[i]);

	}


	if(num_sms==1)
	{
		uint8_t len=SMS_MAX_LEN;

		char text_sms[(len+sizeof(data_time))];
		uint8_t ii=0;

		for(uint16_t i=ADDR_EEP_SMS1; i<(len+ADDR_EEP_SMS1); i++)
		{
			text_sms[ii]=eeprom_read_byte(i);
			ii++;
		}

		for(uint8_t i=0; i<sizeof(data_time); i++) text_sms[(i+len)]=data_time[i];



		uint16_t text_ucs2[sizeof(text_sms)];

		for(uint16_t i=0; i<sizeof(text_sms); i++) text_ucs2[i]=text_unicode(text_sms[i]);
		for(uint16_t i=0; i<sizeof(text_sms); i++) text_unicode_hex(text_ucs2[i]);

	}



	if(num_sms==2)
	{
		uint8_t len=SMS_MAX_LEN;

		char text_sms[(len+sizeof(data_time))];
		uint8_t ii=0;

		for(uint16_t i=ADDR_EEP_SMS2; i<(len+ADDR_EEP_SMS2); i++)
		{
			text_sms[ii]=eeprom_read_byte(i);
			ii++;
		}

		for(uint8_t i=0; i<sizeof(data_time); i++) text_sms[(i+len)]=data_time[i];



		uint16_t text_ucs2[sizeof(text_sms)];

		for(uint16_t i=0; i<sizeof(text_sms); i++) text_ucs2[i]=text_unicode(text_sms[i]);
		for(uint16_t i=0; i<sizeof(text_sms); i++) text_unicode_hex(text_ucs2[i]);

	}

	if(num_sms==3)
	{

		uint8_t len=SMS_MAX_LEN;

		char text_sms[(len+sizeof(data_time))];
		uint8_t ii=0;

		for(uint16_t i=ADDR_EEP_SMS3; i<(len+ADDR_EEP_SMS3); i++)
		{
			text_sms[ii]=eeprom_read_byte(i);
			ii++;
		}

		for(uint8_t i=0; i<sizeof(data_time); i++) text_sms[(i+len)]=data_time[i];



		uint16_t text_ucs2[sizeof(text_sms)];

		for(uint16_t i=0; i<sizeof(text_sms); i++) text_ucs2[i]=text_unicode(text_sms[i]);
		for(uint16_t i=0; i<sizeof(text_sms); i++) text_unicode_hex(text_ucs2[i]);

	}


	END_SMS;					//Макрос завершающий отправку смс.

	
	while(ok());
	_delay_ms(100);				//Ждем ОК.

return 1;

}


//////////////////////////////////////////////////////////////////////////////

int Alarm()
{

			STOP_TIMER0;

			GetTimeSIM();	//Получаем время из Real Time Clock.

			_delay_ms(1000);

		usart0_clear_rx_buffer();
		usart0_clear_tx_buffer();

			sms_tx(0,0);
						for(uint8_t p=0; p<5; p++) _delay_ms(1000);

		//	sms_tx(0,1);
		//				for(uint8_t p=0; p<5; p++) _delay_ms(1000);

		//	sms_tx(0,2);
		//				for(uint8_t p=0; p<5; p++) _delay_ms(1000);

		//	sms_tx(0,3);
		//				for(uint8_t p=0; p<5; p++) _delay_ms(1000);


			PORT_LED&=~(1<<LED);

			for(uint8_t p=0; p<5; p++) _delay_ms(1000);

			usart0_clear_rx_buffer();
			usart0_clear_tx_buffer();

		//	ATD;
		//	NUMBER0_GSM;
		//	ENTER_ATD;
			
			buzzer_timer(2100, 60000);

			START_TIMER0;
			PORT_OUT|=RELAY2;

	return 0;
}
//////////////////////////////////////////////////////////////////////////////


int SMSRxData()
{
	
	_delay_ms(1000);
	_delay_ms(1000);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	CMGR;
	
	uint8_t len=0;
	while(!(len)) len=usart0_rx_len();


	uint8_t sms_ok=usart0_read();

	if(sms_ok=='0') return 0;

	_delay_ms(1000);


	len=usart0_rx_len();

	char temp_sms[len];

	for(uint8_t i=0; i<sizeof(temp_sms); i++) temp_sms[i]=usart0_read();

	
	//Ищем номер входа номера тел.//
	uint8_t count=0;
	for(uint8_t i=0; i<sizeof(temp_sms); i++)
	{
		if(temp_sms[i]=='"') count++;
		if(count==3)
		{
			count=++i;
			break;
		}
	}

	//Копируем номер тел.//
	char temp_numer_ucs2[48];
	for(uint8_t i=0; i<sizeof(temp_numer_ucs2); i++)
	{
		temp_numer_ucs2[i]=temp_sms[i+count];
	}

	//Переводим формат номера из Юникода в GSM формат.//
	char temp_numer_gsm[12];
	for(uint8_t i=0; i<sizeof(temp_numer_gsm); i++)
	{
		temp_numer_gsm[i]=temp_numer_ucs2[(i*4)+3];

		if(temp_numer_gsm[i]=='B') temp_numer_gsm[i]='+';
	}
	

	//Проверяем полученый номер телефона из списка номеров тел//
	uint8_t tel_true[4]={1,1,1,1};

	for(uint8_t i=0; i<sizeof(temp_numer_gsm); i++)
	{
		if(temp_numer_gsm[i]!=eeprom_read_byte(i+ADDR_EEP_NUM0)) tel_true[0]=0;
	}

	for(uint8_t i=0; i<sizeof(temp_numer_gsm); i++)
	{
		if(temp_numer_gsm[i]!=eeprom_read_byte(i+ADDR_EEP_NUM1)) tel_true[1]=0;
	}

	for(uint8_t i=0; i<sizeof(temp_numer_gsm); i++)
	{
		if(temp_numer_gsm[i]!=eeprom_read_byte(i+ADDR_EEP_NUM2)) tel_true[2]=0;
	}

	for(uint8_t i=0; i<sizeof(temp_numer_gsm); i++)
	{
		if(temp_numer_gsm[i]!=eeprom_read_byte(i+ADDR_EEP_NUM3)) tel_true[3]=0;
	}



	for(uint8_t i=0; i<sizeof(tel_true); i++)
	{
		if(tel_true[i]) tel_true[0]=1;
	}


#if	0
	if(!(tel_true[0]))	//Если SMS не из нашего списка, удоляем её к жертям и выходим.
	{
		CMGD;
		return 0;
	}
#endif


	//Ищем номер входа текста SMS//
	count=0;
	for(uint8_t i=0; i<sizeof(temp_sms); i++)
	{
		if(temp_sms[i]=='"') count++;
		if(count==6)
		{
			count=++i;
			break;
		}
	}
	
	count=count+2; //Это 0x0D & 0x0A.

	//Читаем SMS//
	char temp_text_sms[8];
	for(uint8_t i=0; i<sizeof(temp_text_sms); i++)
	{
		temp_text_sms[i]=temp_sms[i+count];
	}
	
	//Проверяем ответ//
	for(uint8_t i=0; i<sizeof(zone1); i++)
	{
		if(temp_text_sms[i]!=zone1[1]);
	}

	
	PORT_LED&=~(1<<LED);



//	ATE1;

//	_delay_ms(1000);

//	for(uint8_t i=0; i<sizeof(temp_text_sms); i++) usart0_write(temp_text_sms[i]);

//	while(1);
//	CMGD;		//Delete SMS.

return 1;
		
}



//////////////////////////////////////////////////////////////////////////////


Identification()
{

		uint8_t data1[8]={0x01, 0x00, 0x00, 0x0A, 0x82, 0x76, 0x96, 0x7B};

		for(uint8_t i=0; i<8; i++)
		{
			if(data_dallas[i]!=data1[i]) 	PORT_LED|=LED;
		}


}

//////////////////////////////////////////////////////////////////////////////



