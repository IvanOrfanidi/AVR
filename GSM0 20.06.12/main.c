

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
	#include "RTC.h"
	#include "BCD.h"
	#include "NTP.h"
	#include "EEPCONFIG.h"


	#define  RESET_SIM		PD3				//Port OUTPUT(-_-).
	#define  NETLIGHT		PD2				//Port INPUT.
	#define  BUZ			(1<<PD4)		//OUTPUT.
	#define  PWR_KEY		PD5				//Port OUTPUT(-_-).
	#define	 KEY0			PD7				//INPUT.
	#define  PORT_IO		PORTD


	#define  LED_GREEN		PA5				//Port OUTPUT.
	#define  LED_RED		PA4				//Port OUTPUT.
	#define  PORT_LED		PORTA
	#define  PIN_KEY		PIND


	#define RELAY_LED 			(1<<PC4)		//OUTPUT
	#define RELAY_BUZ 			(1<<PC3)		//OUTPUT
	#define RELAY_U1 			(1<<PC2)		//OUTPUT
	#define RELAY_U2			(1<<PC1)		//OUTPUT
	#define DTR					(1<<PC0)		//Port OUTPUT.
	#define PORT_OUT			PORTC

	#define USB_RST			PC7				//OUTPUT

	#define USER0 			PB1				//INPUT
	#define USER1 			PB0				//INPUT
	#define PC_CONNECT		PB4				//INPUT
	#define PIN_INPUT		PINB			//
	
	#define S1				PA0				//INPUT
	#define S2				PA3				//INPUT
	
	#define PIN_S			PINA			//INPUT




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

	#define START_TIMER1 TCCR1B=(1<<CS12)|(0<<CS11)|(1<<CS10);	//Timer1 start clock Fcpu/1024.
	#define STOP_TIMER1 TCCR1B=(0<<CS12)|(0<<CS11)|(0<<CS10);	//Timer1 stop clock.

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


	//КЛЮЧИ ДАЛЛАСА, ВСЕГО 70 КЛЮЧЕЙ//
	#define ADDR_EEP_DALLAS_START 	0x01CF
	#define ADDR_EEP_DALLAS_END		0x03FF



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
	char number0[]EEMEM={'+','0','0','0','0','0','0','0','0','0','0','0'};
	char number1[]EEMEM={'+','7','9','5','1','6','8','3','1','1','4','6'};
	char number2[]EEMEM={'+','7','9','5','1','6','8','3','1','1','4','6'};
	char number3[]EEMEM={'+','7','9','5','1','6','8','3','1','1','4','6'};


	char text_sms0[]EEMEM="AБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЭЮЯабвгдежзийклмнопрсту"; // Текст нашей smsки 53 символа.
	char text_sms1[]EEMEM="BБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЭЮЯабвгдежзийклмнопрсту"; //Ровно 53 символа.
	char text_sms2[]EEMEM="CБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЭЮЯабвгдежзийклмнопрсту"; //53 символа.
	char text_sms3[]EEMEM="DБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЭЮЯабвгдежзийклмнопрсту"; //Не более 53 символов.

	uint8_t temp_sreg;

	uint8_t alarm_s1;
	uint8_t alarm_s2;
	uint8_t alarm_u1;
	uint8_t alarm_u2;
	uint8_t alarm_time;
	uint8_t sec_s1;
	uint8_t sec_on_s1;
	uint8_t sec_s2;
	uint8_t sec_u1;
	uint8_t sec_u2;

	uint8_t s2_out_u1;
	uint8_t s2_out_u2;

	uint16_t time_sec_u1;
	uint16_t time_alarm_u1;

	uint16_t count_u1;
	uint16_t count_u2;


void init(void){


	count_u1=0;
	count_u2=0;
	sec_s1=0;
	sec_on_s1=0;
	sec_s2=0;
	sec_u1=0;
	sec_u2=0;

	time_sec_u1=0;
	time_alarm_u1=0;
	alarm_s1=0;
	alarm_s2=0;
	alarm_u1=0;
	alarm_u2=0;
	alarm_time=0;

	// Port A initialization

	DDRA=(1<<LED_GREEN)|(1<<LED_RED);

	PORTA=(0<<LED_GREEN)|(0<<LED_RED)|(1<<S2);

	//////////////////////////////////////////////////////////////////////////////

	// Port B initialization

	DDRB=(1<<PB2);

	PORTB=(1<<PC_CONNECT)|(1<<USER0)|(1<<USER1);

	//////////////////////////////////////////////////////////////////////////////

	// Port C initialization

	DDRC=(RELAY_LED)|(RELAY_BUZ)|(RELAY_U1)|(RELAY_U2)|(DTR)|(1<<USB_RST);

	PORTC=(RELAY_LED)|(RELAY_BUZ)|(RELAY_U1)|(RELAY_U2)|(0<<USB_RST);//|(DTR);

	//////////////////////////////////////////////////////////////////////////////

	// Port D initialization

	DDRD=(1<<RESET_SIM)|(0<<NETLIGHT)|(1<<PWR_KEY)|(BUZ)|(0<<KEY0);

	PORTD=(0<<RESET_SIM)|(1<<NETLIGHT)|(0<<PWR_KEY)|(1<<KEY0);

	//////////////////////////////////////////////////////////////////////////////


	//Analog Comparator initialization.

		ACSR|=_BV(ACD);		//Disable Comparator.

	/////////////////////////////////////////////////////////////////////


	//ADC initialization.

	sei();					//Enable Interrupt for ADC seting.

	ADCSRA|=_BV(ADEN);		//Enable ADC.
	ADCSRA|=_BV(ADPS1);		//fclkADC=fclkIO/4.

	ADMUX=(0<<REFS0)|(1<<REFS1);		//Uref=AVcc.

	cli();					//Disable Interrupt.


	//Init Timer0//


	/////////////////////////////////////////////////////////////////////

	//Init Timer1//
	
	OCR1A=0x1C20;
	TIMSK|=(1<<OCIE1A);

	/////////////////////////////////////////////////////////////////////

	//Init Timer2//


	/////////////////////////////////////////////////////////////////////

}

int main()
{


	init();

	usart0_init();

	sei();


#if 0

	for(uint8_t p=0; p<5; p++) _delay_ms(1000);

	while(at_ok())			//Проверяем готовность модуля командой AT.
	{
		PORT_IO|=(1<<PWR_KEY);
	for(uint8_t p=0; p<2; p++) _delay_ms(1000);
		PORT_IO&=(0<<PWR_KEY);
	for(uint8_t p=0; p<5; p++) _delay_ms(1000);
		PORT_IO|=(1<<PWR_KEY);

		//Если все правильно и скорости совпадают вернтся ответ "0".

		at_ok();
									
		ATV0;	//Макрос отключающий словесный формат ответа. Т.е. теперь будут приходить коды ответа.

			_delay_ms(1000);
		
		ATE0;	//Макрос отключающий эхо от модуля.

			_delay_ms(1000);
	}

	CMGD;
	_delay_ms(1000);

	GetTimeSIM();
	_delay_ms(1000);

	GetTimeSIM();
	_delay_ms(1000);



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


	for(uint8_t i=0; i<sizeof(data_time); i++) usart0_write(data_time[i]);

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
#endif


	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	START_TIMER1;

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
		 	
		}

		if(dallas(0x33)) 
		{

			if(Identification()) 
			{
				if(!(alarm_s2) && !(alarm_u1) && !(alarm_u2))
				{
					if(sec_u1)
					{

						PORT_LED|=(1<<LED_GREEN);
						_delay_ms(500);
						_delay_ms(500);
						_delay_ms(500);
						sec_u1=0;


					}
					else
					{

						PORT_LED&=~(1<<LED_GREEN);

						_delay_ms(500);
						_delay_ms(500);
						_delay_ms(500);
						_delay_ms(500);
						sec_u1=1;

						time_sec_u1=0;

					}

				}

				
				if((alarm_s2) && !(PIN_S & (1<<S2)))
				{
					alarm_s2=0;
					if(s2_out_u1) count_u1=0;
					if(s2_out_u2) count_u2=0;
					_delay_ms(500);
					_delay_ms(500);

				}

			}

		}

		if(PIN_INPUT & (1<<PC_CONNECT)) ConnectPC();

		if(PIN_S & (1<<S2))
		{
			AlarmS2();
		}



		if(sec_on_s1)
		{
			PORT_LED|=(1<<LED_GREEN);
			if(!(alarm_s1)) PORT_OUT&=~(RELAY_LED);

			
			ADCSRA|=_BV(ADSC);				//Start ADC.

			while (ADCSRA & (1<<ADSC)); 		//Wait for the AD conversion to complete.

			if((ADC<0x0064) || (ADC>0x0300))
			{	
				alarm_s1=1;
				START_TIMER1;
			}
			 	
		}

		else

		{
			if(!(sec_u1)) PORT_LED&=~(1<<LED_GREEN);
			alarm_s1=0;
		}


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

//////////////////////////////////////////////////////////////////////////////

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


			PORT_LED&=~(1<<LED_RED);

			for(uint8_t p=0; p<5; p++) _delay_ms(1000);

			usart0_clear_rx_buffer();
			usart0_clear_tx_buffer();

		//	ATD;
		//	NUMBER0_GSM;
		//	ENTER_ATD;
			


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


	if(!(tel_true[0]))	//Если SMS не из нашего списка, удаляем её  и выходим.
	{
		CMGD;
		return 0;
	}


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

	
	PORT_LED&=~(1<<LED_RED);



//	ATE1;

//	_delay_ms(1000);

//	for(uint8_t i=0; i<sizeof(temp_text_sms); i++) usart0_write(temp_text_sms[i]);

//	while(1);
//	CMGD;		//Delete SMS.

return 1;
		
}



//////////////////////////////////////////////////////////////////////////////


int Identification()
{

	uint8_t temp_dallas_key[8]; //Код наоборот.
		
	for(uint8_t Z=0; Z<70; Z++)
	{		

		for(uint8_t i=0; i<sizeof(temp_dallas_key); i++) temp_dallas_key[i]=eeprom_read_byte((ADDR_EEP_DALLAS_START+(Z*8))+i);


		uint8_t dallas_ok=1;

		for(uint8_t i=0; i<8; i++)
		{
			if(data_dallas[i]!=temp_dallas_key[i])
			{
			 	dallas_ok=0;
				break;
			}
		}

		if(dallas_ok) 	return 1;
	}

		return 0;
}

//////////////////////////////////////////////////////////////////////////////

void ConnectPC()
{
	STOP_TIMER1;
	STOP_TIMER0;
	STOP_TIMER2;


	while(PIN_INPUT & (1<<PC_CONNECT))
	{
		PORT_LED|=(1<<LED_RED);
		PORT_LED|=(1<<LED_GREEN);


		if(usart0_rx_len())
		{

			for(uint16_t i=0; i<20000; i++) asm volatile ("nop");
						
							
			PORT_LED&=~(1<<LED_GREEN);

			uint8_t temp_password[9];

			for(uint8_t i=0; i<sizeof(temp_password); i++) temp_password[i]=usart0_read();
			
			usart0_clear_rx_buffer();
			usart0_clear_tx_buffer();

			if(temp_password[0]!='1') usart0_write('0');

			uint8_t password[8]={'1','2','3','4','5','6','7','8'};
			
			uint8_t pas_ok=1;
			for(uint8_t i=0; i<sizeof(password); i++)
			{
				if(temp_password[i+1]!=password[i]) pas_ok=0;
			}
			
			if(!(pas_ok))
			{
				usart0_write('0');
				break;
			} 

				usart0_write('1');
			
		
			while(!(usart0_rx_len()));

				uint8_t temp_byte=usart0_read();

				usart0_clear_rx_buffer();
				usart0_clear_tx_buffer();


				if(temp_byte=='2') AddNewKey();
				if(temp_byte=='3') EraseBaseKey();
				if(temp_byte=='4') NewPassword();
				if(temp_byte=='5') WriteAll();
				if(temp_byte=='B') ReadAll();

				//PORT_LED|=(1<<LED_GREEN);

				usart0_clear_rx_buffer();
				usart0_clear_tx_buffer();


		}

	}

	PORT_LED&=~(1<<LED_RED);
	PORT_LED&=~(1<<LED_GREEN);

	START_TIMER1;

}

void AddNewKey()
{


	uint8_t dal_ok=0;

	while(!(dal_ok))
	{
		dal_ok=dallas(0x33);

		if(usart0_rx_len()) break;

		_delay_ms(500);	//Ждет пока не поднесут ключ.
	}

	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	
	if(!(dal_ok))
	{
		usart0_write('0');
		usart0_write('1');
	}

	else

	{
		dal_ok=0;
		for(uint8_t i=0; i<sizeof(data_dallas); i++)
		{
			if(data_dallas[i]!=0x00) 
			{
				dal_ok=1;
				break;
			}
		}

		if(!(dal_ok))
		{
			usart0_write('0');
			usart0_write('1');
			_delay_ms(100);

			usart0_clear_tx_buffer();
			usart0_clear_rx_buffer();
			return 0;
		}



		uint16_t add_ff;
		uint8_t add_num=0;
		for(add_ff=ADDR_EEP_DALLAS_START; add_ff<ADDR_EEP_DALLAS_END; add_num++)
		{

			if(eeprom_read_byte(add_ff)==0xFF) break;
			add_ff=add_ff+8;


		}
	
		for(uint8_t i=0; i<sizeof(data_dallas); i++) eeprom_update_byte((add_ff+i), data_dallas[i]);

		usart0_write('1');
		usart0_write(add_num+1);

		_delay_ms(100);

		usart0_clear_tx_buffer();
		usart0_clear_rx_buffer();

	}
}


void EraseBaseKey()
{

	for(uint16_t i=ADDR_EEP_DALLAS_START; i<ADDR_EEP_DALLAS_END; i++) eeprom_update_byte(i, 0xFF);
	usart0_write('1'); //Говорим компу что отчистили базу.

}

void NewPassword()
{

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();


	while(usart0_rx_len()!=8);	//Ждем нового пароля.

	uint8_t new_password[8];

	for(uint8_t i=0; i<sizeof(new_password); i++) new_password[i]=usart0_read();

	for(uint8_t i=0; i<sizeof(new_password); i++) eeprom_update_byte((ADDR_EEP_PAS+i), new_password[i]);

	usart0_write('1');	//Говорим компу что переписали пароль.


}


void WriteAll()
{

PORT_LED|=(1<<LED_GREEN);

WriteNum0:

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	while(usart0_rx_len()!=12);	//Ждем первого номера. 

	uint8_t temp_byte;

	for(uint8_t i=0; i<12; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_NUM0+i), temp_byte);
	}
	
	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<sizeof(number0); i++) usart0_write(eeprom_read_byte((ADDR_EEP_NUM0+i)));	//Читаем первый номер.
	
	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteNum0;



WriteNum1:

	while(usart0_rx_len()!=12);	//Ждем 2 номера. 

	for(uint8_t i=0; i<12; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_NUM1+i), temp_byte);
	}
	
	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<sizeof(number1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_NUM1+i)));	//Читаем первый номер.
	
	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteNum1;


WriteNum2:

	while(usart0_rx_len()!=12);	//Ждем 3 номера. 

	for(uint8_t i=0; i<12; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_NUM2+i), temp_byte);
	}
	
	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<sizeof(number2); i++) usart0_write(eeprom_read_byte((ADDR_EEP_NUM2+i)));	//Читаем первый номер.
	
	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteNum2;



WriteNum3:

	while(usart0_rx_len()!=12);	//Ждем 2 номера. 

	for(uint8_t i=0; i<12; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_NUM3+i), temp_byte);
	}
	
	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<sizeof(number3); i++) usart0_write(eeprom_read_byte((ADDR_EEP_NUM3+i)));	//Читаем первый номер.
	
	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteNum3;



//Пишем SMS//

WriteSMS0:
	while(usart0_rx_len()!=52);	//1 SMS.

	for(uint8_t i=0; i<52; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_SMS0+i), temp_byte);
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<(sizeof(text_sms0)-1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_SMS0+i)));	//Читаем первое SMS.

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteSMS0;


WriteSMS1:
	while(usart0_rx_len()!=52);	//1 SMS.

	for(uint8_t i=0; i<52; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_SMS1+i), temp_byte);
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<(sizeof(text_sms1)-1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_SMS1+i)));	//Читаем первое SMS.

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteSMS1;

//Пишем config_s1.
WriteConfigS1:
	while(usart0_rx_len()!=3); //Ждем 3 байт конфига S1

	for(uint8_t i=0; i<3; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_CONFIG_S1+i), temp_byte);
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<3; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_S1+i)));	//Читаем config_s1.

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteConfigS1;


//Пишем config_s2.
WriteConfigS2:
	while(usart0_rx_len()!=3); //Ждем 3 байт конфига S1

	for(uint8_t i=0; i<3; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_CONFIG_S2+i), temp_byte);
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<3; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_S2+i)));	//Читаем config_s2.

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteConfigS2;


//SMS U1
WriteSMS2:
	while(usart0_rx_len()!=52);	//2 SMS.

	for(uint8_t i=0; i<52; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_SMS2+i), temp_byte);
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<(sizeof(text_sms2)-1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_SMS2+i)));	//Читаем первое SMS.

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteSMS2;

//SMS U2
WriteSMS3:
	while(usart0_rx_len()!=52);	//3 SMS.

	for(uint8_t i=0; i<52; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_SMS3+i), temp_byte);
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<(sizeof(text_sms3)-1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_SMS3+i)));	//Читаем первое SMS.

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteSMS3;


//Пишем config_u1.
WriteConfigU1:
	while(usart0_rx_len()!=5); //Ждем 5 байт конфига u1

	for(uint8_t i=0; i<5; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_CONFIG_U1+i), temp_byte);
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<5; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_U1+i)));	//Читаем config_s2.

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteConfigU1;


//Пишем config_u2.
WriteConfigU2:
	while(usart0_rx_len()!=5); //Ждем 5 байт конфига u2

	for(uint8_t i=0; i<5; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_CONFIG_U2+i), temp_byte);
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<5; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_U2+i)));	//Читаем config_s2.

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteConfigU2;


//Пишем config_out_u1.
WriteConfigOutU1:
	while(usart0_rx_len()!=4); //Ждем 4 байт конфига out_u1

	for(uint8_t i=0; i<4; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_CONFIG_OUT_U1+i), temp_byte);
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<4; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_OUT_U1+i)));	//Читаем config_s2.

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteConfigOutU1;

//Пишем config_out_u2.
WriteConfigOutU2:
	while(usart0_rx_len()!=4); //Ждем 4 байт конфига out_u2

	for(uint8_t i=0; i<4; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_CONFIG_OUT_U2+i), temp_byte);
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<4; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_OUT_U2+i)));	//Читаем config_s2.

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteConfigOutU2;


//Пишем config_time_us.
WriteConfigTimeUS:
	while(usart0_rx_len()!=4); //Ждем 4 байт конфига config_time_us.

	for(uint8_t i=0; i<4; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_CONFIG_TIME_US+i), temp_byte);
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<4; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_TIME_US+i)));	//Читаем config_s2.

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteConfigTimeUS;

//Пишем config_alarm_time_us.
WriteConfigAlarmTimeUS:
	while(usart0_rx_len()!=4); //Ждем 4 байт конфига config_alarm_time_us.

	for(uint8_t i=0; i<4; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_CONFIG_ALARM_TIME_US+i), temp_byte);
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<4; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_ALARM_TIME_US+i)));	//Читаем config_s2.

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteConfigAlarmTimeUS;


	_delay_ms(200);
	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();
	_delay_ms(200);
//	PORT_LED|=(1<<LED_GREEN);

}




void ReadAll()
{
	PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<3; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_S1+i)));	//Читаем config_s1.

	
	_delay_ms(300);

	PORT_LED&=~(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<3; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_S2+i)));	//Читаем config_s2.

	_delay_ms(300);

	PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<sizeof(number0); i++) usart0_write(eeprom_read_byte((ADDR_EEP_NUM0+i)));	//Читаем первый номер.

	_delay_ms(300);

	PORT_LED&=~(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<sizeof(number1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_NUM1+i)));	//Читаем второй номер.

	_delay_ms(300);

	PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<sizeof(number2); i++) usart0_write(eeprom_read_byte((ADDR_EEP_NUM2+i)));	//Читаем третий номер.

	_delay_ms(300);

	PORT_LED&=~(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<sizeof(number3); i++) usart0_write(eeprom_read_byte((ADDR_EEP_NUM3+i)));	//Читаем четвертый номер.

	_delay_ms(300);

	PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<(sizeof(text_sms0)-1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_SMS0+i)));	//Читаем первое SMS.

	_delay_ms(1000);

	PORT_LED&=~(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<(sizeof(text_sms1)-1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_SMS1+i)));	//Читаем второе SMS.

	_delay_ms(1000);

	PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<(sizeof(text_sms2)-1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_SMS2+i)));	//Читаем третье SMS.

	_delay_ms(1000);

	PORT_LED&=~(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<(sizeof(text_sms3)-1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_SMS3+i)));	//Читаем четвертое SMS.

	_delay_ms(1000);

	PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();


	for(uint8_t i=0; i<5; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_U1+i)));	//Читаем config_u1.

	_delay_ms(300);

	PORT_LED&=~(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<5; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_U2+i)));	//Читаем config_u2.

	_delay_ms(300);

	PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();


	for(uint8_t i=0; i<4; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_OUT_U1+i)));	//Читаем config_out_s1.

	_delay_ms(300);

	PORT_LED&=~(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<4; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_OUT_U2+i)));	//Читаем config_out_s2.

	_delay_ms(300);

	PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	//Определяем количество ключей в памяти//
	uint16_t add_ff;
	uint8_t add_num=0;
	for(add_ff=ADDR_EEP_DALLAS_START; add_ff<ADDR_EEP_DALLAS_END; add_num++)
	{
		if(eeprom_read_byte(add_ff)==0xFF) break;
		add_ff=add_ff+8;
	}

	usart0_write(add_num);	//Отправляем количество ключей в памяти.

	_delay_ms(300);

	PORT_LED&=~(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();


	for(uint8_t i=0; i<4; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_TIME_US+i)));	//Читаем config_time_us.

	_delay_ms(300);

	PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<4; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_ALARM_TIME_US+i)));	//Читаем config_time_us.

	_delay_ms(300);

	PORT_LED&=~(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

}


void AlarmS1()
{
	 
//	STOP_TIMER0;
//	STOP_TIMER1;
//	STOP_TIMER2;

	uint8_t temp_config_s1[3];
	for(uint8_t i=0; i<sizeof(temp_config_s1); i++) temp_config_s1[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_S1);

	//Отправляем СМС.
	if(temp_config_s1[0] & 1)
	{
	//	sms_tx(0,1);
	//	for(uint8_t p=0; p<1; p++) _delay_ms(1000);
	}

	if(temp_config_s1[0] & 2)
	{
	//	sms_tx(1,1);
	//	for(uint8_t p=0; p<1; p++) _delay_ms(1000);
	}

	if(temp_config_s1[0] & 4)
	{
	//	sms_tx(2,1);
	//	for(uint8_t p=0; p<1; p++) _delay_ms(1000);
	}
	if(temp_config_s1[0] & 8)
	{
	//	sms_tx(3,1);
	//	for(uint8_t p=0; p<1; p++) _delay_ms(1000);
	}

	//Звоним на номера.
	if(temp_config_s1[0] & 1)
	{
	//	ATD;
	//	NUMBER0_GSM;
	//	ENTER_ATD;
	//	while(!(usart0_rx_len()));

	//	_delay_ms(200);

	}

	if(temp_config_s1[0] & 2)
	{
	//	ATD;
	//	NUMBER1_GSM;
	//	ENTER_ATD;
	//	while(!(usart0_rx_len()));

	//	_delay_ms(200);


	}

	if(temp_config_s1[0] & 4)
	{
	//	ATD;
	//	NUMBER2_GSM;
	//	ENTER_ATD;

	//	while(!(usart0_rx_len()));

	//	_delay_ms(200);

	}

	if(temp_config_s1[0] & 8)
	{
	//	ATD;
	//	NUMBER3_GSM;
	//	ENTER_ATD;
	}

//	START_TIMER0;
//	START_TIMER1;
//	START_TIMER2;

}


void AlarmS2()
{

	if(alarm_s2) return 0;

//	STOP_TIMER0;
//	STOP_TIMER1;
//	STOP_TIMER2;

	alarm_s2=1;

	uint8_t temp_config_s2[3];
	for(uint8_t i=0; i<sizeof(temp_config_s2); i++) temp_config_s2[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_S2);

	//Отправляем СМС.
	if(temp_config_s2[1] & 1)
	{
	//	sms_tx(0,1);
	//	for(uint8_t p=0; p<1; p++) _delay_ms(1000);
	}

	if(temp_config_s2[1] & 2)
	{
	//	sms_tx(1,1);
	//	for(uint8_t p=0; p<1; p++) _delay_ms(1000);
	}

	if(temp_config_s2[1] & 4)
	{
	//	sms_tx(2,1);
	//	for(uint8_t p=0; p<1; p++) _delay_ms(1000);
	}
	if(temp_config_s2[1] & 8)
	{
	//	sms_tx(3,1);
	//	for(uint8_t p=0; p<1; p++) _delay_ms(1000);
	}


	//Звоним на номера.
	if(temp_config_s2[0] & 1)
	{
	//	ATD;
	//	NUMBER0_GSM;
	//	ENTER_ATD;
	//	while(!(usart0_rx_len()));

	//	_delay_ms(200);

	}

	if(temp_config_s2[0] & 2)
	{
	//	ATD;
	//	NUMBER1_GSM;
	//	ENTER_ATD;

	//	while(!(usart0_rx_len()));

	//	_delay_ms(200);


	}

	if(temp_config_s2[0] & 4)
	{
	//	ATD;
	//	NUMBER2_GSM;
	//	ENTER_ATD;

	//	while(!(usart0_rx_len()));

	//	_delay_ms(200);

	}

	if(temp_config_s2[0] & 8)
	{
	//	ATD;
	//	NUMBER3_GSM;
	//	ENTER_ATD;
	}


	START_TIMER1;

//	START_TIMER0;
//	START_TIMER1;
//	START_TIMER2;

}



void AlarmVisibleOutU12()
{
	uint8_t temp_config_out_u1[4];
	for(uint8_t i=0; i<sizeof(temp_config_out_u1); i++)	temp_config_out_u1[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_OUT_U1);
		

	uint8_t temp_config_out_u2[4];
	for(uint8_t i=0; i<sizeof(temp_config_out_u2); i++)	temp_config_out_u2[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_OUT_U2);


	if((alarm_s2) || (((alarm_s1)||(alarm_u1)||(alarm_u2)) && (alarm_time)))
	{

		if(alarm_s1) AlarmS1();

		PORT_LED|=(1<<LED_RED);
		PORT_OUT&=~(RELAY_BUZ);


		if(PORT_OUT & (RELAY_LED)) PORT_OUT&=~(RELAY_LED); else PORT_OUT|=(RELAY_LED);

		if((temp_config_out_u1[0] & 1) || (temp_config_out_u1[0] & 2))
		{
			s2_out_u1=1;

			if(temp_config_out_u1[1]==1)
			{
				PORT_OUT&=~(RELAY_U1);
			}
			else
			{
				if(count_u1<=(temp_config_out_u1[2]+temp_config_out_u1[3]))
				{
					PORT_OUT&=~(RELAY_U1);
					count_u1++;
				}
				else
				{
					PORT_OUT|=(RELAY_U1);
				}

				
			}
		}
		else
		{
			s2_out_u1=0;
		}


		if((temp_config_out_u1[0] & 1) || (temp_config_out_u1[0] & 2))
		{
			s2_out_u2=1;

			if(temp_config_out_u2[1]==1)
			{
				PORT_OUT&=~(RELAY_U2);
			}
			else
			{
				if(count_u2<=(temp_config_out_u2[2]+temp_config_out_u2[3]))
				{
					PORT_OUT&=~(RELAY_U2);
					count_u2++;
				}
				else
				{
					PORT_OUT|=(RELAY_U2);
				}

				
			}
		}
		else
		{
			s2_out_u2=0;
		}



	}
	else
	{

		PORT_OUT|=(RELAY_U1);
		PORT_OUT|=(RELAY_U2);
		PORT_OUT|=(RELAY_BUZ);

		if(!(sec_u1) && !(sec_s1) && !(sec_s2))
		{
			PORT_LED&=~(1<<LED_RED);
			PORT_OUT|=(RELAY_LED);
		} 

	}
	

}

void ClockSecU1()
{
	if(sec_u1)
	{
		uint8_t temp_config_time_us=0;
		uint16_t temp_time_sec_u1;
		temp_config_time_us=eeprom_read_byte(ADDR_EEP_CONFIG_TIME_US+0);
		temp_time_sec_u1=temp_config_time_us*60;
		
		if(temp_time_sec_u1<=time_sec_u1)
		{
			sec_on_s1=1;
		}
		else
		{
			
			if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

			time_sec_u1++;

		}	
				
	}

		else
	{
		sec_on_s1=0;
	}	

}


void ClockAlarm()
{
	if((alarm_s1)||(alarm_u1)||(alarm_u2))
	{
		uint8_t temp_config_alarm_time_us=0;
		uint16_t temp_alarm_time_u1=0;	
		temp_config_alarm_time_us=eeprom_read_byte(ADDR_EEP_CONFIG_ALARM_TIME_US+0);
		temp_alarm_time_u1=temp_config_alarm_time_us*60;

		if(temp_alarm_time_u1<=time_alarm_u1)
		{
			alarm_time=1;
		}
		else
		{
			if(PORT_LED & (1<<LED_RED)) PORT_LED&=~(1<<LED_RED); else PORT_LED|=(1<<LED_RED);

			time_alarm_u1++;
		}
	}
	else
	{
		time_alarm_u1=0;
		alarm_time=0;
	}

}

ISR(TIMER1_COMPA_vect)				//Interrupt 1c.
{
	TCNT1=0x0000;

	AlarmVisibleOutU12();

	ClockSecU1();

	ClockAlarm();

}


