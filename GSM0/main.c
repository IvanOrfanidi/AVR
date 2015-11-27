

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


	#define  STATUS			PD2				//Port INPUT.
	#define  BUZ			(1<<PC7)		//OUTPUT.
	#define  PWR_KEY		(1<<PD6)				//Port OUTPUT(-_-).



	#define  LED_GREEN		PA5				//Port OUTPUT.
	#define  LED_RED		PA4				//Port OUTPUT.
	#define  PORT_LED		PORTA
	#define  EN_SIM			(1<<PD3)
	#define  EN_FTDI		(1<<PB6)


	#define RELAY_LED 			(1<<PC6)		//OUTPUT
	#define RELAY_BUZ 			(1<<PC1)		//OUTPUT
	#define RELAY_U1 			(1<<PC0)		//OUTPUT
	#define RELAY_U2			(1<<PD7)		//OUTPUT
	#define DTR					(1<<PD5)		//Port OUTPUT.
	#define RST					(1<<PD4)		//Port OUTPUT.

	#define PORT_OUT			PORTC



	#define IN3	 			PB1				//INPUT
	#define IN4 			PB0				//INPUT
	#define BOOT_RST		PB3				//INPUT
	#define PC_CONNECT		PB4				//INPUT
	#define UPS				PB5				//INPUT
	#define PIN_INPUT		PINB			//
	
	#define IN1				PA0				//INPUT
	#define IN2				PA3				//INPUT

	#define PIN_IN12		PINA			//INPUT
	#define PIN_IN34		PINB			//INPUT




	//Макросы работающие с GSM модулем SIM300.//
	#define AT			for(uint8_t n=0; n<sizeof(at); n++)	usart0_write(at[n]);	//AT.
	#define ATE0 		for(uint8_t n=0; n<sizeof(echo_off); n++)	usart0_write(echo_off[n]);
	#define ATE1 		for(uint8_t n=0; n<sizeof(echo_on); n++)	usart0_write(echo_on[n]);
	#define ATV1 		for(uint8_t n=0; n<sizeof(atv_on); n++)	usart0_write(atv_on[n]);
	#define ATV0 		for(uint8_t n=0; n<sizeof(atv_off); n++)	usart0_write(atv_off[n]);

	#define NUMBER0_UCS2	for(uint16_t n=ADDR_EEP_NUM0; n<(sizeof(number0)+ADDR_EEP_NUM0); n++) number0_unicode(n);
	#define NUMBER1_UCS2	for(uint16_t n=ADDR_EEP_NUM1; n<(sizeof(number0)+ADDR_EEP_NUM1); n++) number1_unicode(n);
	#define NUMBER2_UCS2	for(uint16_t n=ADDR_EEP_NUM2; n<(sizeof(number0)+ADDR_EEP_NUM2); n++) number2_unicode(n);
	#define NUMBER3_UCS2	for(uint16_t n=ADDR_EEP_NUM3; n<(sizeof(number0)+ADDR_EEP_NUM3); n++) number3_unicode(n);

	#define	CMGF1		for(uint8_t n=0; n<sizeof(format_text); n++) usart0_write(format_text[n]);			 //Макрос уставливает режим PDU.
	#define CSCS_UCS2	for(uint8_t n=0; n<sizeof(te_format_ucs2); n++) usart0_write(te_format_ucs2[n]);
	#define CMGR		for(uint8_t n=0; n<sizeof(sms_read); n++) usart0_write(sms_read[n]);
	#define CMGD		for(uint8_t n=0; n<sizeof(sms_del); n++) usart0_write(sms_del[n]);

	#define CSCB0 	for(uint8_t n=0; n<sizeof(at_cscb_off); n++) usart0_write(at_cscb_off[n]);

	#define NUMBER0_GSM	for(uint16_t n=ADDR_EEP_NUM0; n<(sizeof(number0)+ADDR_EEP_NUM0); n++) usart0_write(eeprom_read_byte(n));
	#define NUMBER1_GSM	for(uint16_t n=ADDR_EEP_NUM1; n<(sizeof(number1)+ADDR_EEP_NUM1); n++) usart0_write(eeprom_read_byte(n));
	#define NUMBER2_GSM	for(uint16_t n=ADDR_EEP_NUM2; n<(sizeof(number2)+ADDR_EEP_NUM2); n++) usart0_write(eeprom_read_byte(n));
	#define NUMBER3_GSM	for(uint16_t n=ADDR_EEP_NUM3; n<(sizeof(number3)+ADDR_EEP_NUM3); n++) usart0_write(eeprom_read_byte(n));

	#define SMS_MAX_LEN 52


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


	#define TIMEOUT_DALLAS	100

	const char at[]={'A', 'T', 0x0D, 0x0A};
	const char echo_off[]={'A','T','E', '0', 0x0D, 0x0A};
	const char echo_on[]={'A','T','E', '1', 0x0D, 0x0A};

	const char atv_off[]={'A','T','V', '0', 0x0D, 0x0A};
	const char atv_on[]={'A','T','V', '1', 0x0D, 0x0A};
	const char format_text[]={'A','T','+','C','M','G','F','=','1', 0x0D, 0x0A};

	const char sms_end[]={0x1A};

	const char te_format_ucs2[]={'A','T','+','C','S','C','S','=','"','U','C','S','2','"', 0x0D, 0x0A};

	const char at_cscb_off[]={'A','T','+','C','S','C','B','=','0', 0x0D, 0x0A};

	const char sms_read[]={'A','T','+','C','M','G','R','=','1', 0x0D, 0x0A};
	const char sms_del[]={'A','T','+','C','M','G','D','=','1', 0x0D, 0x0A};
		
	const char zone_on[]={'0','0','3','0'};
	const char zone_of[]={'0','0','3','1'};

	char password_eep[]EEMEM={'1','2','3','4','5','6','7','8'};
	char number0[]EEMEM={'+','0','0','0','0','0','0','0','0','0','0','0'};
	char number1[]EEMEM={'+','0','0','0','0','0','0','0','0','0','0','0'};
	char number2[]EEMEM={'+','0','0','0','0','0','0','0','0','0','0','0'};
	char number3[]EEMEM={'+','0','0','0','0','0','0','0','0','0','0','0'};


	char text_sms0[]EEMEM="                                                    "; // Текст нашей smsки 53 символа.
	char text_sms1[]EEMEM="                                                    "; //Ровно 53 символа.
	char text_sms2[]EEMEM="                                                    "; //53 символа.
	char text_sms3[]EEMEM="                                                    "; //Не более 53 символов.

	uint8_t temp_sreg;

	uint8_t alarm_s1;
	uint8_t alarm_s2;
	uint8_t alarm_in3;
	uint8_t alarm_in4;
	uint8_t alarm_time_s1;
	uint8_t alarm_time_in3;
	uint8_t alarm_time_in4;

	uint8_t alarm_info_in1;
	uint8_t alarm_info_in2;
	uint8_t alarm_info_in3;
	uint8_t alarm_info_in4;

	uint8_t sec_s1;
	uint8_t sec_on_s1;
	uint8_t sec_on_in3;
	uint8_t sec_on_in4;
	uint8_t sec_s2;
	uint8_t sec_u1;
	uint8_t sec_u2;
	uint8_t sec_in3;
	uint8_t sec_in4;


	uint8_t s2_out_u1;
	uint8_t s2_out_u2;

	uint8_t ups_ok;

	uint16_t time_sec_u1;
	uint16_t time_sec_in3;
	uint16_t time_sec_in4;

	uint16_t time_alarm_in3;
	uint16_t time_alarm_in4;
	uint16_t time_alarm_u1;
	uint16_t time_alarm_visible;


	uint16_t count_u1;
	uint16_t count_u2;

	uint8_t sim_sleep;

	uint8_t dallas_timeout_true;
	uint16_t dallas_timeout;

void init(void)
{
	// Port A initialization

	DDRA=(1<<LED_GREEN)|(1<<LED_RED)|(0<<IN1)|(0<<IN2);

	PORTA=(0<<LED_GREEN)|(0<<LED_RED)|(0<<IN1)|(1<<IN2);

	//////////////////////////////////////////////////////////////////////////////

	// Port B initialization

	DDRB=(EN_FTDI)|(EN_FTDI)|(0<<IN3)|(0<<IN4)|(0<<PC_CONNECT)|(0<<BOOT_RST)|(0<<UPS);

	PORTB=(1<<IN3)|(1<<IN4)|(1<<PC_CONNECT)|(1<<BOOT_RST)|(1<<UPS);

	//////////////////////////////////////////////////////////////////////////////

	// Port C initialization

	DDRC=(BUZ)|(RELAY_LED)|(RELAY_BUZ)|(RELAY_U1);

	PORTC=(RELAY_LED)|(RELAY_BUZ)|(RELAY_U1);
	//////////////////////////////////////////////////////////////////////////////

	// Port D initialization

	DDRD=(0<<STATUS)|(PWR_KEY)|(EN_SIM)|(RELAY_U2)|(DTR)|(RST);

	PORTD=(1<<STATUS)|(RELAY_U2)|(EN_SIM)|(DTR)|(RST);

	//////////////////////////////////////////////////////////////////////////////


	//Analog Comparator initialization.

		ACSR|=_BV(ACD);		//Disable Comparator.

	/////////////////////////////////////////////////////////////////////


	//ADC initialization.

	sei();							//Enable Interrupt for ADC seting.

	ADCSRA|=_BV(ADEN);				//Enable ADC.
	ADCSRA|=_BV(ADPS1);				//fclkADC=fclkIO/4.

	ADMUX=(0<<REFS0)|(1<<REFS1);	//Uref=AVcc.

	cli();							//Disable Interrupt.


	//Init Timer0//


	/////////////////////////////////////////////////////////////////////

	//Init Timer1//
	
	OCR1A=0x1C20;			//Interrupt 1c.
	TIMSK|=(1<<OCIE1A);

	/////////////////////////////////////////////////////////////////////

	//Init Timer2//


	/////////////////////////////////////////////////////////////////////

}

void zerro_var()
{
	count_u1=0;
	count_u2=0;
	sec_s1=0;
	sec_on_s1=0;
	sec_on_in3=0;
	sec_s2=0;
	sec_u1=0;
	sec_u2=0;

	sim_sleep=0;

	sec_in3=0;
	sec_in4=0;


	time_sec_u1=0;
	time_sec_in3=0;

	time_alarm_u1=0;
	time_alarm_in3=0;
	time_alarm_in4=0;

	alarm_info_in1=0;
	alarm_info_in2=0;
	alarm_info_in3=0;
	alarm_info_in4=0;

	alarm_s1=0;
	alarm_s2=0;
	alarm_in3=0;
	alarm_in4=0;
	alarm_time_s1=0;

	alarm_time_in3=0;
	time_alarm_in3=0;

	alarm_time_in4=0;
	time_alarm_in4=0;
	
	time_alarm_visible=0;

	dallas_timeout_true=0;
	dallas_timeout=0;
}

void init_var()
{
	count_u1=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+0);			//1
	count_u2=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+1);			//2
	sec_s1=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+2);			//3
	sec_on_s1=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+3);			//4
	sec_on_in3=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+4);		//5
	sec_s2=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+5);			//6
	sec_u1=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+6);			//7
	sec_u2=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+7);			//8

	sec_in3=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+8);			//9
	sec_in4=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+9);			//10


	time_sec_u1=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+10);		//11
	time_sec_in3=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+11);		//12
	time_sec_in4=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+12);		//13

	time_alarm_u1=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+13);	//14
	time_alarm_in3=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+14);	//15
	time_alarm_in4=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+15);	//16

	alarm_info_in1=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+16);	//17
	alarm_info_in2=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+17);	//18
	alarm_info_in3=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+18);	//19
	alarm_info_in4=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+19);	//20

	alarm_s1=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+20);			//21
	alarm_s2=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+21);			//22
	alarm_in3=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+22);		//23
	alarm_in4=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+23);		//24
	alarm_time_s1=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+24);	//25

	alarm_time_in3=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+25);	//26
	time_alarm_in3=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+26);	//27

	alarm_time_in4=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+27);	//28
	time_alarm_in4=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+28);	//29
	
	time_alarm_visible=eeprom_read_byte(ADDR_EEP_SAVE_CONFIG+29);	//30

	ups_ok=0;
}

int AtdNumber(uint16_t num)
{
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	char data[]={'A','T','D',0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,';', 0x0D, 0x0A};
	
	uint8_t n=3;
	for(uint16_t i=num*12+ADDR_EEP_NUM0; i<(sizeof(number0)+num*12+ADDR_EEP_NUM0); i++)
	{
		data[n]=eeprom_read_byte(i);
		n++;
	}

	for(uint8_t i=0; i<sizeof(data); i++)
	{
		usart0_write(data[i]);
		_delay_ms(10);
	}

	while(!(usart0_rx_len()));

	
	uint8_t byte=usart0_read();

	if(byte!='0') return byte;

	for(uint16_t i=0; i<55*100; i++)
	{
		if(usart0_rx_len())
		{
			byte=usart0_read();
			if((byte=='7')||(byte=='3')||(byte=='4')||(byte=='8')) return byte;

			
		}
		_delay_ms(10);

	}
	
	char ath0[]={'A','T','H', 0x0D, 0x0A};

	for(uint8_t i=0; i<sizeof(ath0); i++)
	{
		usart0_write(ath0[i]);
		_delay_ms(10);
	}
	
	if(WhileResponse('0','4')) return 1;

	return 0;
}


int WhileResponse(uint16_t response, uint16_t error)
{
	uint8_t byte;
	while(1)
	{
		while(!(usart0_rx_len()));	

		byte=usart0_read();
		if(byte==response) return 0;
		if(byte==error) return 1;
			
	}
	_delay_ms(10);
}

int main()
{

	init();

//	while(1);

	zerro_var();

	init_var();

	usart0_init();

	sei();
	
	if(!(PIN_INPUT & (1<<BOOT_RST))) ResetConfig();

	_delay_ms(1000);
	_delay_ms(1000);

	while(at_ok())			//Проверяем готовность модуля командой AT.
	{
		PORTD|=(PWR_KEY);

		_delay_ms(1500);

		PORTD&=~(PWR_KEY);

		_delay_ms(1000);
		_delay_ms(1000);
		_delay_ms(1000);
		_delay_ms(1000);
		_delay_ms(1000);

		//Если все правильно и скорости совпадают вернтся ответ "0".

		at_ok();

		_delay_ms(1000);
	
		ATE0;	//Макрос отключающий эхо от модуля.

		_delay_ms(1000);

		ATV0;	//Макрос отключающий словесный формат ответа. Т.е. теперь будут приходить коды ответа.

		_delay_ms(1000);

	}

	if(PIN_INPUT & (1<<PC_CONNECT)) ConnectPC();

//	ATE1;

	_delay_ms(1000);
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();
	_delay_ms(1000);


//	ATV1;

	_delay_ms(1000);
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();
	_delay_ms(1000);


	CSCB0;

	_delay_ms(1000);
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();
	_delay_ms(1000);

	CMGD;

	_delay_ms(1000);
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();
	_delay_ms(1000);

	GetTimeSIM();

	_delay_ms(1000);
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();
	_delay_ms(1000);

//	for(uint8_t i=0; i<sizeof(data_time); i++) usart0_write(data_time[i]);

//	while(1);

	CMGF1;			//Макрос уставливает режим PDU.

	_delay_ms(1000);
		
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	_delay_ms(1000);

	CSCS_UCS2;		//Макрос устанавливает кодировку Юникод. По умолчанию GSM.

	_delay_ms(1000);

	PORT_OUT|=(BUZ);

	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	_delay_ms(1000);

	PORT_OUT&=~(BUZ);

//	AtdNumber(2);
//	SmsTx(2,3);
	_delay_ms(1000);

//	PORT_LED|=(1<<LED_GREEN);
//	while(1);

	START_TIMER1;

	while(1)
	{

		sei();
		
#if 0
		if(usart0_rx_len())
		{

			SMSRxData();

			_delay_ms(1000);
			usart0_clear_tx_buffer();
			usart0_clear_rx_buffer();


			while(usart0_rx_len()) usart0_clear_rx_buffer();
		 	
		}
#endif

		if(dallas(0x33)) 
		{

			if(Identification() && !(dallas_timeout_true)) 
			{
				dallas_timeout_true=1;

				//Alarm In1//
				if((sec_u1) && !(alarm_s1) && !(alarm_time_s1) && !(sec_on_s1))
				{
					sec_u1=0;
					sec_on_s1=0;
					time_sec_u1=0;
					alarm_time_s1=0;
					time_alarm_u1=0;

					sec_in3=0;
					sec_on_in3=0;
					time_sec_in3=0;
					alarm_time_in3=0;
					time_alarm_in3=0;

					sec_in4=0;
					sec_on_in4=0;
					time_sec_in4=0;
					alarm_time_in4=0;
					time_alarm_in4=0;

					alarm_info_in1=0;
					alarm_info_in2=0;
					alarm_info_in3=0;
					alarm_info_in4=0;

					PORT_OUT|=(RELAY_BUZ);
					PORT_OUT|=(RELAY_LED);
					PORT_LED&=~(1<<LED_RED);
					PORT_LED&=~(1<<LED_GREEN);
					

				}

				else

				{
					sec_u1=1;
				}


				if((alarm_time_s1) && !(alarm_s1) && !(sec_on_s1) && !(sec_u1))
				{
					sec_u1=0;
					sec_on_s1=0;
					time_sec_u1=0;
					alarm_time_s1=0;
					time_alarm_u1=0;

					sec_in3=0;
					sec_on_in3=0;
					time_sec_in3=0;
					alarm_time_in3=0;
					time_alarm_in3=0;

					sec_in4=0;
					sec_on_in4=0;
					time_sec_in4=0;
					alarm_time_in4=0;
					time_alarm_in4=0;

					alarm_info_in1=0;
					alarm_info_in2=0;
					alarm_info_in3=0;
					alarm_info_in4=0;

					PORT_OUT|=(RELAY_BUZ);
					PORT_OUT|=(RELAY_LED);
					PORT_LED&=~(1<<LED_RED);
					PORT_LED&=~(1<<LED_GREEN);
					
				}
					
				if((sec_on_s1) && !(alarm_s1))
				{
					sec_u1=0;
					sec_on_s1=0;
					time_sec_u1=0;
					alarm_time_s1=0;
					time_alarm_u1=0;

					sec_in3=0;
					sec_on_in3=0;
					time_sec_in3=0;
					alarm_time_in3=0;
					time_alarm_in3=0;

					sec_in4=0;
					sec_on_in4=0;
					time_sec_in4=0;
					alarm_time_in4=0;
					time_alarm_in4=0;

					alarm_info_in1=0;
					alarm_info_in2=0;
					alarm_info_in3=0;
					alarm_info_in4=0;

					PORT_OUT|=(RELAY_BUZ);
					PORT_OUT|=(RELAY_LED);
					PORT_LED&=~(1<<LED_RED);
					PORT_LED&=~(1<<LED_GREEN);
					

				}
					
				if(alarm_s1)
				{
					alarm_s1=0;
					alarm_time_s1=0;
					time_alarm_u1=0;
					sec_u1=0;
					sec_on_s1=0;
					time_sec_u1=0;

					sec_in3=0;
					sec_on_in3=0;
					time_sec_in3=0;
					alarm_time_in3=0;
					time_alarm_in3=0;

					sec_in4=0;
					sec_on_in4=0;
					time_sec_in4=0;
					alarm_time_in4=0;
					time_alarm_in4=0;

					time_alarm_visible=0;

					alarm_info_in1=0;
					alarm_info_in2=0;
					alarm_info_in3=0;
					alarm_info_in4=0;

					PORT_OUT|=(RELAY_BUZ);
					PORT_OUT|=(RELAY_LED);
					PORT_LED&=~(1<<LED_RED);
					PORT_LED&=~(1<<LED_GREEN);

					uint8_t temp_config_out_u1[4];
					for(uint8_t i=0; i<sizeof(temp_config_out_u1); i++)	temp_config_out_u1[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_OUT_U1);
		

					uint8_t temp_config_out_u2[4];
					for(uint8_t i=0; i<sizeof(temp_config_out_u2); i++)	temp_config_out_u2[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_OUT_U2);

					if(temp_config_out_u1[0] & 1)
					{
						if(temp_config_out_u1[1]==1) PORT_OUT|=(RELAY_U1);
						if(temp_config_out_u2[1]==2)
						{
							count_u1=0;
							PORT_OUT|=(RELAY_U1);
						}
					}

					if(temp_config_out_u2[0] & 1)
					{
						if(temp_config_out_u2[1]==1) PORTD|=(RELAY_U2);
						if(temp_config_out_u2[1]==2)
						{
							count_u2=0;
							PORTD|=(RELAY_U2);
						}
					}

					for(uint16_t n=0; n<50000; n++) asm volatile ("nop");
					

				}

///////////////////////////////////////////////////////////////////////////////////////////////

				//Alarm In3//
				if((sec_in3) && !(alarm_in3) && !(alarm_time_in3) && !(sec_on_in3))
				{
					sec_in3=0;
					sec_on_in3=0;
					time_sec_in3=0;
					alarm_time_in3=0;
					time_alarm_in3=0;

					sec_u1=0;
					sec_on_s1=0;
					time_sec_u1=0;
					alarm_time_s1=0;
					time_alarm_u1=0;

					sec_in4=0;
					sec_on_in4=0;
					time_sec_in4=0;
					alarm_time_in4=0;
					time_alarm_in4=0;

					PORT_OUT|=(RELAY_BUZ);
					PORT_OUT|=(RELAY_LED);
					PORT_LED&=~(1<<LED_RED);
					PORT_LED&=~(1<<LED_GREEN);

					alarm_info_in1=0;
					alarm_info_in2=0;
					alarm_info_in3=0;
					alarm_info_in4=0;
					

				}

				else

				{
					sec_in3=1;
				}


				if((alarm_time_in3) && !(alarm_in3) && !(sec_on_in3) && !(sec_in3))
				{
					sec_in3=0;
					sec_on_in3=0;
					time_sec_in3=0;
					alarm_time_in3=0;
					time_alarm_in3=0;

					sec_u1=0;
					sec_on_s1=0;
					time_sec_u1=0;
					alarm_time_s1=0;
					time_alarm_u1=0;

					sec_in4=0;
					sec_on_in4=0;
					time_sec_in4=0;
					alarm_time_in4=0;
					time_alarm_in4=0;

					alarm_info_in1=0;
					alarm_info_in2=0;
					alarm_info_in3=0;
					alarm_info_in4=0;


					PORT_OUT|=(RELAY_BUZ);
					PORT_OUT|=(RELAY_LED);
					PORT_LED&=~(1<<LED_RED);
					PORT_LED&=~(1<<LED_GREEN);
					
				}
					
				if((sec_on_in3) && !(alarm_in3))
				{
					sec_in3=0;
					sec_on_in3=0;
					time_sec_in3=0;
					alarm_time_in3=0;
					time_alarm_in3=0;

					sec_u1=0;
					sec_on_s1=0;
					time_sec_u1=0;
					alarm_time_s1=0;
					time_alarm_u1=0;

					sec_in4=0;
					sec_on_in4=0;
					time_sec_in4=0;
					alarm_time_in4=0;
					time_alarm_in4=0;

					alarm_info_in1=0;
					alarm_info_in2=0;
					alarm_info_in3=0;
					alarm_info_in4=0;

					PORT_OUT|=(RELAY_BUZ);
					PORT_OUT|=(RELAY_LED);
					PORT_LED&=~(1<<LED_RED);
					PORT_LED&=~(1<<LED_GREEN);
					
				}
					

				if(alarm_in3)
				{
					alarm_in3=0;
					sec_in3=0;
					sec_on_in3=0;
					time_sec_in3=0;
					alarm_time_in3=0;
					time_alarm_in3=0;

					sec_u1=0;
					sec_on_s1=0;
					time_sec_u1=0;
					alarm_time_s1=0;
					time_alarm_u1=0;

					sec_in4=0;
					sec_on_in4=0;
					time_sec_in4=0;
					alarm_time_in4=0;
					time_alarm_in4=0;

					time_alarm_visible=0;

					alarm_info_in1=0;
					alarm_info_in2=0;
					alarm_info_in3=0;
					alarm_info_in4=0;

					PORT_OUT|=(RELAY_BUZ);
					PORT_OUT|=(RELAY_LED);
					PORT_LED&=~(1<<LED_RED);
					PORT_LED&=~(1<<LED_GREEN);

					uint8_t temp_config_out_u1[4];
					for(uint8_t i=0; i<sizeof(temp_config_out_u1); i++)	temp_config_out_u1[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_OUT_U1);
		

					uint8_t temp_config_out_u2[4];
					for(uint8_t i=0; i<sizeof(temp_config_out_u2); i++)	temp_config_out_u2[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_OUT_U2);

					if(temp_config_out_u1[0] & 4)
					{
						if(temp_config_out_u1[1]==1) PORT_OUT|=(RELAY_U1);
						if(temp_config_out_u2[1]==2)
						{
							count_u1=0;
							PORT_OUT|=(RELAY_U1);
						}
					}

					if(temp_config_out_u2[0] & 4)
					{
						if(temp_config_out_u2[1]==1) PORTD|=(RELAY_U2);
						if(temp_config_out_u2[1]==2)
						{
							count_u2=0;
							PORTD|=(RELAY_U2);
						}
					}

					
				for(uint16_t n=0; n<50000; n++) asm volatile ("nop");
				

				}

//////////////////////////////////////////////////////////////////////////////////////////////

				//Alarm In4//				
				if((sec_in4) && !(alarm_in4) && !(alarm_time_in4) && !(sec_on_in4))
				{
					sec_in3=0;
					sec_on_in3=0;
					time_sec_in3=0;
					alarm_time_in3=0;
					time_alarm_in3=0;

					sec_u1=0;
					sec_on_s1=0;
					time_sec_u1=0;
					alarm_time_s1=0;
					time_alarm_u1=0;

					sec_in4=0;
					sec_on_in4=0;
					time_sec_in4=0;
					alarm_time_in4=0;
					time_alarm_in4=0;

					PORT_OUT|=(RELAY_BUZ);
					PORT_OUT|=(RELAY_LED);
					PORT_LED&=~(1<<LED_RED);
					PORT_LED&=~(1<<LED_GREEN);

					alarm_info_in1=0;
					alarm_info_in2=0;
					alarm_info_in3=0;
					alarm_info_in4=0;

				}

				else

				{
					sec_in4=1;
				}

				
				if((alarm_time_in4) && !(alarm_in4) && !(sec_on_in4) && !(sec_in4))
				{
					sec_in3=0;
					sec_on_in3=0;
					time_sec_in3=0;
					alarm_time_in3=0;
					time_alarm_in3=0;

					sec_u1=0;
					sec_on_s1=0;
					time_sec_u1=0;
					alarm_time_s1=0;
					time_alarm_u1=0;

					sec_in4=0;
					sec_on_in4=0;
					time_sec_in4=0;
					alarm_time_in4=0;
					time_alarm_in4=0;

					alarm_info_in1=0;
					alarm_info_in2=0;
					alarm_info_in3=0;
					alarm_info_in4=0;


					PORT_OUT|=(RELAY_BUZ);
					PORT_OUT|=(RELAY_LED);
					PORT_LED&=~(1<<LED_RED);
					PORT_LED&=~(1<<LED_GREEN);
					
				}
					
				if((sec_on_in4) && !(alarm_in4))
				{
					sec_in3=0;
					sec_on_in3=0;
					time_sec_in3=0;
					alarm_time_in3=0;
					time_alarm_in3=0;

					sec_u1=0;
					sec_on_s1=0;
					time_sec_u1=0;
					alarm_time_s1=0;
					time_alarm_u1=0;

					sec_in4=0;
					sec_on_in4=0;
					time_sec_in4=0;
					alarm_time_in4=0;
					time_alarm_in4=0;

					PORT_OUT|=(RELAY_BUZ);
					PORT_OUT|=(RELAY_LED);
					PORT_LED&=~(1<<LED_RED);
					PORT_LED&=~(1<<LED_GREEN);
				
				}
					
				if(alarm_in4)
				{
					alarm_in4=0;

					sec_in3=0;
					sec_on_in3=0;
					time_sec_in3=0;
					alarm_time_in3=0;
					time_alarm_in3=0;

					sec_u1=0;
					sec_on_s1=0;
					time_sec_u1=0;
					alarm_time_s1=0;
					time_alarm_u1=0;

					sec_in4=0;
					sec_on_in4=0;
					time_sec_in4=0;
					alarm_time_in4=0;
					time_alarm_in4=0;

					time_alarm_visible=0;

					alarm_info_in1=0;
					alarm_info_in2=0;
					alarm_info_in3=0;
					alarm_info_in4=0;

					PORT_OUT|=(RELAY_BUZ);
					PORT_OUT|=(RELAY_LED);
					PORT_LED&=~(1<<LED_RED);
					PORT_LED&=~(1<<LED_GREEN);

					uint8_t temp_config_out_u1[4];
					for(uint8_t i=0; i<sizeof(temp_config_out_u1); i++)	temp_config_out_u1[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_OUT_U1);
		

					uint8_t temp_config_out_u2[4];
					for(uint8_t i=0; i<sizeof(temp_config_out_u2); i++)	temp_config_out_u2[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_OUT_U2);

					if(temp_config_out_u1[0] & 8)
					{
						if(temp_config_out_u1[1]==1) PORT_OUT|=(RELAY_U1);
						if(temp_config_out_u2[1]==2)
						{
							count_u1=0;
							PORT_OUT|=(RELAY_U1);
						}

					}

					if(temp_config_out_u2[0] & 8)
					{
						if(temp_config_out_u2[1]==1) PORTD|=(RELAY_U2);
						if(temp_config_out_u2[1]==2)
						{
							count_u2=0;
							PORTD|=(RELAY_U2);
						}

					}

				for(uint16_t n=0; n<50000; n++) asm volatile ("nop");

				}

//////////////////////////////////////////////////////////////////////////////////////////////


				if((alarm_s2) && !(PIN_IN12 & (1<<IN2)))
				{
					alarm_s2=0;
					time_alarm_visible=0;

					if(s2_out_u1) count_u1=0;
					if(s2_out_u2) count_u2=0;
					PORT_OUT|=(RELAY_BUZ);
					PORT_OUT|=(RELAY_LED);
					PORT_LED&=~(1<<LED_RED);

					uint8_t temp_config_out_u1[4];
					for(uint8_t i=0; i<sizeof(temp_config_out_u1); i++)	temp_config_out_u1[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_OUT_U1);
		

					uint8_t temp_config_out_u2[4];
					for(uint8_t i=0; i<sizeof(temp_config_out_u2); i++)	temp_config_out_u2[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_OUT_U2);

					if(temp_config_out_u1[0] & 2)
					{
						if(temp_config_out_u1[1]==1) PORT_OUT|=(RELAY_U1);
						if(temp_config_out_u2[1]==2)
						{
							count_u1=0;
							PORT_OUT|=(RELAY_U1);
						}

					}

					if(temp_config_out_u2[0] & 2)
					{
						if(temp_config_out_u2[1]==1) PORTD|=(RELAY_U2);
						if(temp_config_out_u2[1]==2)
						{
							count_u2=0;
							PORTD|=(RELAY_U2);
						}

					}

					for(uint16_t n=0; n<50000; n++) asm volatile ("nop");
				}

			}

			//for(uint16_t n=0; n<50000; n++) asm volatile ("nop");
		
		}

///////////////////////////////////////////////////////////////////////////////////////////////

		if(dallas_timeout_true)
		{
			dallas_timeout++;
			if(dallas_timeout>TIMEOUT_DALLAS)
			{
				dallas_timeout_true=0;
				dallas_timeout=0;
			}
		}


///////////////////////////////////////////////////////////////////////////////////////////////

		if(PIN_IN12 & (1<<IN2)) alarm_s2=1;

///////////////////////////////////////////////////////////////////////////////////////////////

		if(!(PIN_INPUT & (1<<UPS)))
		{
			if(!(PIN_INPUT & (1<<UPS)) && !(sim_sleep))
			{
				for(uint8_t i=0; i<3; i++) _delay_ms(1000);

				if(!(PIN_INPUT & (1<<UPS))) sim_sleep=SleepSIM(1);
			}			

		}
		else
		{
			if((PIN_INPUT & (1<<UPS)) && sim_sleep)
			{
				for(uint8_t i=0; i<10; i++) _delay_ms(1000);
				if(PIN_INPUT & (1<<UPS)) sim_sleep=SleepSIM(0);
			}
		}

///////////////////////////////////////////////////////////////////////////////////////////////

		if(sec_on_s1)
		{
			if(!(alarm_s2) && !(alarm_in3) && !(alarm_in4))
			{
				PORT_LED|=(1<<LED_GREEN);
				if(!(alarm_s1)) PORT_OUT&=~(RELAY_LED);
			}
			
			ADCSRA|=_BV(ADSC);				//Start ADC.

			while (ADCSRA & (1<<ADSC)); 		//Wait for the AD conversion to complete.

			if((ADC<0x0064) || (ADC>0x0280))
			{	
				alarm_time_s1=1;
			}
			 	
		}

/////////////////////////////////////////////////////////////////////

		if(eeprom_read_byte(ADDR_EEP_CONFIG_U1+3)==2)
		{
			if(eeprom_read_byte(ADDR_EEP_CONFIG_U1+4)==1)
			{
				if(PIN_IN34 & (1<<IN3)) alarm_in3=1;
			}

			if(eeprom_read_byte(ADDR_EEP_CONFIG_U1+4)==2)
			{
				if(!(PIN_IN34 & (1<<IN3))) alarm_in3=1;
			}

		}


		if(sec_on_in3)
		{
			if(!(alarm_s2) && !(alarm_s1) && !(alarm_in4))
			{
				
					PORT_LED|=(1<<LED_GREEN);
					if(!(alarm_in3)) PORT_OUT&=~(RELAY_LED);
			}

			if(eeprom_read_byte(ADDR_EEP_CONFIG_U1+4)==1)
			{
				if(PIN_IN34 & (1<<IN3)) alarm_time_in3=1;
			}

			if(eeprom_read_byte(ADDR_EEP_CONFIG_U1+4)==2)
			{
				if(!(PIN_IN34 & (1<<IN3))) alarm_time_in3=1;
			}

		}



			
/////////////////////////////////////////////////////////////////////

		if(eeprom_read_byte(ADDR_EEP_CONFIG_U2+3)==2)
		{
			if(eeprom_read_byte(ADDR_EEP_CONFIG_U2+4)==1)
			{
				if(PIN_IN34 & (1<<IN4)) alarm_in4=1;
			}

			if(eeprom_read_byte(ADDR_EEP_CONFIG_U2+4)==2)
			{
				if(!(PIN_IN34 & (1<<IN4))) alarm_in4=1;
			}

		}


		if(sec_on_in4)
		{
			if(!(alarm_s2) && !(alarm_s1) && !(alarm_in3))
			{	
					PORT_LED|=(1<<LED_GREEN);
					if(!(alarm_in4)) PORT_OUT&=~(RELAY_LED);
			}

			if(eeprom_read_byte(ADDR_EEP_CONFIG_U2+4)==1)
			{
				if(PIN_IN34 & (1<<IN4)) alarm_time_in4=1;
			}

			if(eeprom_read_byte(ADDR_EEP_CONFIG_U2+4)==2)
			{
				if(!(PIN_IN34 & (1<<IN4))) alarm_time_in4=1;
			}

		}

	}


}

//////////////////////////////////////////////////////////////////////////////
int SleepSIM(uint16_t mode)
{

	if(mode)
	{		
		STOP_TIMER0;
		STOP_TIMER1;
		STOP_TIMER2;

		sei();

		
		if(!(RegGSM()))
		{
			_delay_ms(100);

			SmsTx(0,4);
			_delay_ms(100);

			SmsTx(1,4);
			_delay_ms(100);

			SmsTx(2,4);
			_delay_ms(100);

			SmsTx(3,4);
			_delay_ms(100);

			_delay_ms(500);
		}
		

		PORT_LED|=(1<<LED_RED);

		_delay_ms(500);

		while(!(at_ok()))			//Проверяем готовность модуля командой AT.
		{
			PORTD|=(PWR_KEY);

			_delay_ms(1500);

			PORTD&=~(PWR_KEY);

			_delay_ms(1000);
			_delay_ms(1000);
		}

//		START_TIMER0;
		START_TIMER1;
//		START_TIMER2;

		return 1;	

	}

	else

	{
		
		STOP_TIMER0;
		STOP_TIMER1;
		STOP_TIMER2;

		sei();

		while(at_ok())			//Проверяем готовность модуля командой AT.
		{
			PORTD|=(PWR_KEY);

			_delay_ms(1500);

			PORTD&=~(PWR_KEY);

			for(uint8_t i=0; i<5; i++) _delay_ms(1000);


			//Если все правильно и скорости совпадают вернтся ответ "0".

			at_ok();

			_delay_ms(1000);
	
			ATE0;	//Макрос отключающий эхо от модуля.

			_delay_ms(1000);

			ATV0;	//Макрос отключающий словесный формат ответа. Т.е. теперь будут приходить коды ответа.

			_delay_ms(1000);
		}

		
		for(uint8_t i=0; i<10; i++) _delay_ms(1000);

		if(!(RegGSM()))
		{
			_delay_ms(100);

			SmsTx(0,5);
			_delay_ms(100);

			SmsTx(1,5);
			_delay_ms(100);

			SmsTx(2,5);
			_delay_ms(100);

			SmsTx(3,5);
			_delay_ms(100);
		
			_delay_ms(100);
		}
		
//		START_TIMER0;
		START_TIMER1;
//		START_TIMER2;

		return 0;	
	}


}

int RegGSM()	//0- OK
{	

	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();
	
	sei();

	const char reg_gsm[]={'A','T','+','C','R','E','G','?', 0x0D, 0x0A};

	for(uint8_t n=0; n<sizeof(reg_gsm); n++)
	{
		usart0_write(reg_gsm[n]);
		_delay_ms(10);
	}


	WhileResponse(',',0xFF);

	while(!usart0_rx_len());

	uint8_t byte=usart0_read();

	if(byte=='1') return 0;
		
	return byte;

}


int at_ok()
{
		usart0_clear_rx_buffer();
		usart0_clear_tx_buffer();

		AT;

		_delay_ms(1000);

		if(!(usart0_rx_len())) return 1;


		if(usart0_read()=='0') return 0;
		
		usart0_clear_rx_buffer();
		return 1;

	
}

//////////////////////////////////////////////////////////////////////////////

int ok()
{
	_delay_ms(1000);

	if(!(usart0_rx_len())) return 1;

	if(usart0_read()=='0') return 0;
	
	return usart0_read();
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

int SmsTx(uint16_t num, uint16_t sms)
{

	usart0_clear_tx_buffer();	//Clear Tx buffer.
	usart0_clear_rx_buffer();	//Clear Rx buffer.

	char data_num[]={'A','T','+','C','M','G','S','=','"','0','0','2','B','0','0','3','0','0','0','3','0','0','0','3','0','0','0','3','0','0','0','3','0','0','0','3','0','0','0','3','0','0','0','3','0','0','0','3','0','0','0','3','0','0','0','3','0','"', 0x0D, 0x0A};

	uint8_t n=16;
	for(uint16_t i=num*12+ADDR_EEP_NUM0+1; i<(sizeof(number0)+num*12+ADDR_EEP_NUM0); i++)
	{
		data_num[n]=eeprom_read_byte(i);
		n+=4;
	}

	for(uint8_t i=0; i<sizeof(data_num); i++)
	{
		usart0_write(data_num[i]);
		_delay_ms(10);			
	}

//

	uint8_t byte;
	while(1)
	{
		while(!(usart0_rx_len()));	//Ждем приглашение от модуля.

		byte=usart0_read();
		if(byte=='4') return byte;	//Как только прийдет ">" можно будет вводить текст смс.
		if(byte=='>') break;
	}

	usart0_clear_tx_buffer();	//Clear Tx buffer.
	_delay_ms(1000);			

	usart0_write(0x08);


//	char data[]={'0','4','1','1','0','4','1','2','0','4','1','3','0','4','1','4'};
//	for(uint8_t i=0; i<sizeof(data); i++) usart0_write(data[i]);


	if(sms==0)
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


	if(sms==1)
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



	if(sms==2)
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

	if(sms==3)
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


	if(sms==4)
	{
		char data[]="Power OFF!";
		char text_sms[sizeof(data)];

		for(uint8_t i=0; i<(sizeof(data)-1); i++) text_sms[i]=data[i];

		uint16_t text_ucs2[sizeof(text_sms)];

		for(uint16_t i=0; i<sizeof(text_sms); i++) text_ucs2[i]=text_unicode(text_sms[i]);
		for(uint16_t i=0; i<sizeof(text_sms); i++) text_unicode_hex(text_ucs2[i]);

	}

	if(sms==5)
	{
		char data[]="Power ON!";
		char text_sms[sizeof(data)];

		for(uint8_t i=0; i<(sizeof(data)-1); i++) text_sms[i]=data[i];

		uint16_t text_ucs2[sizeof(text_sms)];

		for(uint16_t i=0; i<sizeof(text_sms); i++) text_ucs2[i]=text_unicode(text_sms[i]);
		for(uint16_t i=0; i<sizeof(text_sms); i++) text_unicode_hex(text_ucs2[i]);

	}

	_delay_ms(100);			

	usart0_write(0x1A);

	usart0_clear_tx_buffer();	//Clear Tx buffer.

	if(WhileResponse(':','4')) return 1;

	_delay_ms(10);

	return 0;

}


//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////

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

	for(uint8_t tx=0; tx<4; tx++)
	{
		usart0_write(data_tx[tx]);	//Отправка 4 байт, это есть один символ.
		_delay_ms(10);
	}
	_delay_ms(10);
}

//////////////////////////////////////////////////////////////////////////////

#if 0

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

	if(sms_ok=='0') return 1;

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

return 0;
		
}

#endif

//////////////////////////////////////////////////////////////////////////////


int Identification()
{
	uint8_t temp_dallas_key[8]; //Код наоборот.
		
	for(uint8_t Z=0; Z<70; Z++)	//Всего 70 ключей.
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
	STOP_TIMER0;
	STOP_TIMER1;
	STOP_TIMER2;

	PORTD&=~(EN_SIM);
	PORTB|=(EN_FTDI);

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

			uint8_t password[8];


			for(uint8_t i=0; i<sizeof(password); i++) password[i]=eeprom_read_byte(i+ADDR_EEP_PAS);


			uint8_t pas_ok=1;
			for(uint8_t i=0; i<sizeof(password); i++)
			{
				if(temp_password[i+1]!=password[i]) pas_ok=0;
			}

			if(pas_ok)
			{
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
				if(temp_byte=='C') SetTimePC();

				usart0_clear_rx_buffer();
				usart0_clear_tx_buffer();
			}

			else
			{
				usart0_write('0');
			}

		}

	}

	PORTB&=~(EN_FTDI);
	PORTD|=(EN_SIM);

//	_delay_ms(1000);

//	SetTimeSIM();
	
//	_delay_ms(1000);

	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

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

		//Определяем есть ли ключ в памяти.//
		for(uint8_t i=0; i<70; i++)
		{

			uint16_t add_key;
			uint8_t key_new=1;
			for(uint8_t ii=0; ii<sizeof(data_dallas); ii++)
			{
				add_key=((ADDR_EEP_DALLAS_START+(i*sizeof(data_dallas)))+ii);

				if(data_dallas[ii]!=eeprom_read_byte(add_key)) key_new=0;

				if(!(key_new)) break;
			} 
	
			if(key_new)
			{
				usart0_write('3');
				usart0_write('1');
				_delay_ms(100);

				usart0_clear_tx_buffer();
				usart0_clear_rx_buffer();
				return 0;
			}
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

if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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


if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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

if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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


if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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

if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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

if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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
if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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
if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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


//SMS U1.
if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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
if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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
if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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
if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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
if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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
if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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
if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

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

//Пишем config_time_alarm_us.
if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

WriteConfigTimeAlarmUS:

	while(usart0_rx_len()!=4); //Ждем 4 байт конфига config_alarm_time_us.

	for(uint8_t i=0; i<4; i++)
	{
		temp_byte=usart0_read();
		eeprom_update_byte((ADDR_EEP_CONFIG_TIME_ALARM_US+i), temp_byte);
	}

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<4; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_TIME_ALARM_US+i)));	//Читаем config_s2.

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteConfigTimeAlarmUS;


//Пишем config_time_alarm_visible.
if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

WriteConfigTimeAlarmVisible:

	while(usart0_rx_len()!=1);		//Ждем 1 байт config_time_alarm_visible.

	temp_byte=usart0_read();
	eeprom_update_byte((ADDR_EEP_CONFIG_TIME_ALARM_VISIBLE), temp_byte);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	usart0_write(eeprom_read_byte(ADDR_EEP_CONFIG_TIME_ALARM_VISIBLE));

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteConfigTimeAlarmVisible;
	
	_delay_ms(200);
	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();
	_delay_ms(200);
//	PORT_LED|=(1<<LED_GREEN);

}




void ReadAll()
{
	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<3; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_S1+i)));	//Читаем config_s1.

	
	_delay_ms(200);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);


	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<3; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_S2+i)));	//Читаем config_s2.

	_delay_ms(200);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);


	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<sizeof(number0); i++) usart0_write(eeprom_read_byte((ADDR_EEP_NUM0+i)));	//Читаем первый номер.

	_delay_ms(200);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);


	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<sizeof(number1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_NUM1+i)));	//Читаем второй номер.

	_delay_ms(200);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);


	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<sizeof(number2); i++) usart0_write(eeprom_read_byte((ADDR_EEP_NUM2+i)));	//Читаем третий номер.

	_delay_ms(200);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);


	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<sizeof(number3); i++) usart0_write(eeprom_read_byte((ADDR_EEP_NUM3+i)));	//Читаем четвертый номер.

	_delay_ms(200);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);


	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<(sizeof(text_sms0)-1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_SMS0+i)));	//Читаем первое SMS.

	_delay_ms(800);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<(sizeof(text_sms1)-1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_SMS1+i)));	//Читаем второе SMS.

	_delay_ms(800);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<(sizeof(text_sms2)-1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_SMS2+i)));	//Читаем третье SMS.

	_delay_ms(800);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<(sizeof(text_sms3)-1); i++) usart0_write(eeprom_read_byte((ADDR_EEP_SMS3+i)));	//Читаем четвертое SMS.

	_delay_ms(800);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();


	for(uint8_t i=0; i<5; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_U1+i)));	//Читаем config_u1.

	_delay_ms(200);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);


	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<5; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_U2+i)));	//Читаем config_u2.

	_delay_ms(200);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);


	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();


	for(uint8_t i=0; i<4; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_OUT_U1+i)));	//Читаем config_out_s1.

	_delay_ms(200);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);


	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<4; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_OUT_U2+i)));	//Читаем config_out_s2.

	_delay_ms(200);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);


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

	_delay_ms(200);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);


	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();


	for(uint8_t i=0; i<4; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_TIME_US+i)));	//Читаем config_time_us.

	_delay_ms(200);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);


	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<4; i++) usart0_write(eeprom_read_byte((ADDR_EEP_CONFIG_TIME_ALARM_US+i)));	//Читаем config_time_us.

	_delay_ms(200);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();


	usart0_write(eeprom_read_byte(ADDR_EEP_CONFIG_TIME_ALARM_VISIBLE));

	_delay_ms(200);

	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();
}


void SetTimePC()
{
	//Пишем сначала дату и затем время.
	if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);
	
	uint8_t temp_date[10];
	uint8_t temp_byte;

	//Пишем Дату//
	WriteDate:

	while(usart0_rx_len()!=10);		//Ждем 10 байт текущей даты.
	
	for(uint8_t i=0; i<sizeof(temp_date); i++) temp_date[i]=usart0_read();

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	//Заносим дату в глобальные переменные//
	cday[0]=temp_date[0];
	cday[1]=temp_date[1];

	cmonth[0]=temp_date[3];
	cmonth[1]=temp_date[4];

	cyear[0]=temp_date[8];
	cyear[1]=temp_date[9];

	//Формируем ответ ПК//
	temp_date[0]=cday[0];
	temp_date[1]=cday[1];

	temp_date[2]='.';

	temp_date[3]=cmonth[0];
	temp_date[4]=cmonth[1];

	temp_date[5]='.';

	temp_date[6]='2';
	temp_date[7]='0';

	temp_date[8]=cyear[0];
	temp_date[9]=cyear[1];
	//////////////////////
	
	for(uint8_t i=0; i<sizeof(temp_date); i++) usart0_write(temp_date[i]);

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteDate;

	//Пишем время//
	WriteTime:

	while(usart0_rx_len()!=8);		//Ждем 8 байт текущего времени.

	uint8_t temp_time[8];

	for(uint8_t i=0; i<sizeof(temp_time); i++) temp_time[i]=usart0_read();

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	//Заносим  в глобальные переменные//
	chours[0]=temp_time[0];
	chours[1]=temp_time[1];

	cmin[0]=temp_time[3];
	cmin[1]=temp_time[4];

	csec[0]=temp_time[6];
	csec[1]=temp_time[7];

	//Формируем ответ ПК//
	temp_time[0]=chours[0];
	temp_time[1]=chours[1];

	temp_time[2]=':';

	temp_time[3]=cmin[0];
	temp_time[4]=cmin[1];

	temp_time[5]=':';

	temp_time[6]=csec[0];
	temp_time[7]=csec[1];
	//////////////////////
	
	for(uint8_t i=0; i<sizeof(temp_time); i++) usart0_write(temp_time[i]);

	_delay_ms(200);
	while(!(usart0_rx_len()));
	temp_byte=usart0_read();
	if(temp_byte=='0') goto WriteTime;
	
	_delay_ms(200);

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	_delay_ms(200);

	PORTB&=~(EN_FTDI);
	PORTD|=(EN_SIM);

	_delay_ms(500);

	SetTimeSIM();
	
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	PORTD&=~(EN_SIM);
	PORTB|=(EN_FTDI);

}


/////////////////////////////////////////////

void ResetConfig()
{
	
	PORT_LED|=(1<<LED_GREEN);

	for(uint8_t i=0; i<ADDR_EEP_NUM0; i++) eeprom_update_byte(ADDR_EEP_PAS+i, (i+0x31));

	for(uint16_t i=ADDR_EEP_NUM0; i<(ADDR_EEP_NUM3+12); i++) 
	{
		if((i==ADDR_EEP_NUM0)||(i==ADDR_EEP_NUM1)||(i==ADDR_EEP_NUM2)||(i==ADDR_EEP_NUM3))
		{
			eeprom_update_byte(i, '+');
		}
		else
		{
			eeprom_update_byte(i, '0');
		}
	}

	for(uint16_t i=ADDR_EEP_SMS0; i<(ADDR_EEP_SMS3+53); i++) eeprom_update_byte(i, ' ');

	for(uint16_t i=ADDR_EEP_CONFIG_U1; i<ADDR_EEP_CONFIG_TIME_ALARM_VISIBLE; i++) eeprom_update_byte(i, 0x00);

	eeprom_update_byte(ADDR_EEP_CONFIG_TIME_ALARM_VISIBLE, 0x14);

	for(uint16_t i=ADDR_EEP_DALLAS_START; i<ADDR_EEP_DALLAS_END; i++) eeprom_update_byte(i, 0xFF);

	zerro_var();

	SaveConfig();

	while(1)
	{
		if(!(PORT_LED & (1<<LED_RED))) 
		{
			PORT_LED|=(1<<LED_RED);
			PORT_OUT|=(BUZ);
		}

		else

		{
			PORT_LED&=~(1<<LED_RED);
			PORT_OUT&=~(BUZ);
		}

		for(uint32_t n=0; n<30000; n++) asm volatile ("nop");
	}

}

/////////////////////////////////////////////

void SaveConfig()
{

	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+0, count_u1);					//1
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+1, count_u2);					//2
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+2, sec_s1);						//3
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+3, sec_on_s1);					//4
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+4, sec_on_in3);					//5
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+5, sec_s2);						//6
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+6, sec_u1);						//7
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+7, sec_u2);						//8
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+8, sec_in3);					//9
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+9, sec_in4);					//10
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+10, time_sec_u1);				//11
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+11, time_sec_in3);				//12
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+12, time_sec_in4);				//13
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+13, time_alarm_u1);				//14
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+14, time_alarm_in3);			//15
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+15, time_alarm_in4);			//16
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+16, alarm_info_in1);			//17
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+17, alarm_info_in2);			//18
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+18, alarm_info_in3);			//19
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+19, alarm_info_in4);			//20
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+20, alarm_s1);					//21
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+21, alarm_s2);					//22
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+22, alarm_in3);					//23
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+23, alarm_in4);					//24
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+24, alarm_time_s1);				//25
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+25, alarm_time_in3);			//26
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+26, time_alarm_in3);			//27
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+27, alarm_time_in4);			//28
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+28, time_alarm_in4);			//29
	eeprom_update_byte(ADDR_EEP_SAVE_CONFIG+29, time_alarm_visible);		//30

}


int AlarmIn1()
{
	if(alarm_info_in1) return 1;

	if(sim_sleep) return 2;

	if(RegGSM()) return 3;
	_delay_ms(100);

	PORT_LED|=(1<<LED_RED);

	alarm_info_in1=1;

	STOP_TIMER1;

	sei();

	uint8_t temp_config_s1[3];
	for(uint8_t i=0; i<sizeof(temp_config_s1); i++) temp_config_s1[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_S1);

	if(temp_config_s1[1]!=0) GetTimeSIM();	//Получаем время из Real Time Clock.
	_delay_ms(100);

	//Отправляем СМС.
	if(temp_config_s1[1] & 1)
	{
		SmsTx(0,0);
		_delay_ms(100);

	}

	if(temp_config_s1[1] & 2)
	{
		SmsTx(1,0);
		_delay_ms(100);
	}

	if(temp_config_s1[1] & 4)
	{
		SmsTx(2,0);
		_delay_ms(100);
	}
	if(temp_config_s1[1] & 8)
	{
		SmsTx(3,0);
		_delay_ms(100);
	}

	_delay_ms(1000);

	//Звоним на номера.
	if(temp_config_s1[0] & 1)
	{
		AtdNumber(0);
		_delay_ms(200);

	}

	if(temp_config_s1[0] & 2)
	{

		AtdNumber(1);
		_delay_ms(200);

	}

	if(temp_config_s1[0] & 4)
	{

		AtdNumber(2);
		_delay_ms(200);

	}

	if(temp_config_s1[0] & 8)
	{
		AtdNumber(3);
		_delay_ms(200);

	}

	cli();

	START_TIMER1;

	return 0;

}


int AlarmIn2()
{
	if(alarm_info_in2) return 1;

	if(sim_sleep) return 2;

	if(RegGSM()) return 3;
	_delay_ms(100);

	PORT_LED|=(1<<LED_RED);

	alarm_info_in2=1;

	STOP_TIMER1;
	
	sei();

	uint8_t temp_config_s2[3];
	for(uint8_t i=0; i<sizeof(temp_config_s2); i++) temp_config_s2[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_S2);

	PORT_LED|=(1<<LED_RED);

	if(temp_config_s2[1]!=0) GetTimeSIM();	//Получаем время из Real Time Clock.
	_delay_ms(100);

	//Отправляем СМС.
	if(temp_config_s2[1] & 1)
	{
		SmsTx(0,1);
		_delay_ms(100);
	}

	if(temp_config_s2[1] & 2)
	{
		SmsTx(1,1);
		_delay_ms(100);
	}

	if(temp_config_s2[1] & 4)
	{
		SmsTx(2,1);
		_delay_ms(100);
	}
	if(temp_config_s2[1] & 8)
	{
		SmsTx(3,1);
		_delay_ms(100);
	}

	_delay_ms(1000);

	//Звоним на номера.
	if(temp_config_s2[0] & 1)
	{
		AtdNumber(0);
		_delay_ms(200);
	}

	if(temp_config_s2[0] & 2)
	{
		AtdNumber(1);
		_delay_ms(200);
	}

	if(temp_config_s2[0] & 4)
	{
		AtdNumber(2);
		_delay_ms(200);
	}

	if(temp_config_s2[0] & 8)
	{
		AtdNumber(3);
		_delay_ms(200);
	}

	cli();

	START_TIMER1;

	return 0;

}

int AlarmIn3()
{

	if(alarm_info_in3) return 1;

	if(sim_sleep) return 2;

	if(RegGSM()) return 3;
	_delay_ms(100);


	PORT_LED|=(1<<LED_RED);

	alarm_info_in3=1;

	STOP_TIMER1;

	sei();

	uint8_t temp_config_in3[2];
	for(uint8_t i=0; i<sizeof(temp_config_in3); i++) temp_config_in3[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_U1);

	if(temp_config_in3[1]!=0) GetTimeSIM();	//Получаем время из Real Time Clock.
	_delay_ms(100);

	//Отправляем СМС.
	if(temp_config_in3[1] & 1)
	{
		SmsTx(0,2);
		_delay_ms(100);
	}

	if(temp_config_in3[1] & 2)
	{
		SmsTx(1,2);
		_delay_ms(100);
	}

	if(temp_config_in3[1] & 4)
	{
		SmsTx(2,2);
		_delay_ms(100);
	}
	if(temp_config_in3[1] & 8)
	{
		SmsTx(3,2);
		_delay_ms(100);
	}

	_delay_ms(1000);

	//Звоним на номера.
	if(temp_config_in3[0] & 1)
	{
		AtdNumber(0);
		_delay_ms(200);
	}

	if(temp_config_in3[0] & 2)
	{
		AtdNumber(1);
		_delay_ms(200);
	}

	if(temp_config_in3[0] & 4)
	{
		AtdNumber(2);
		_delay_ms(200);
	}

	if(temp_config_in3[0] & 8)
	{
		AtdNumber(3);
		_delay_ms(200);
	}

	cli();

	START_TIMER1;

	return 0;
}


uint8_t AlarmIn4()
{

	if(alarm_info_in4) return 1;

	if(sim_sleep) return 2;

	if(RegGSM()) return 3;
	_delay_ms(100);

	PORT_LED|=(1<<LED_RED);

	alarm_info_in4=1;

	STOP_TIMER1;

	sei();

	uint8_t temp_config_in4[2];
	for(uint8_t i=0; i<sizeof(temp_config_in4); i++) temp_config_in4[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_U2);

	if(temp_config_in4[1]!=0) GetTimeSIM();	//Получаем время из Real Time Clock.
	_delay_ms(100);

	//Отправляем СМС.
	if(temp_config_in4[1] & 1)
	{
		SmsTx(0,3);
		_delay_ms(100);
	}

	if(temp_config_in4[1] & 2)
	{
		SmsTx(1,3);
		_delay_ms(100);
	}

	if(temp_config_in4[1] & 4)
	{
		SmsTx(2,3);
		_delay_ms(100);
	}
	if(temp_config_in4[1] & 8)
	{
		SmsTx(3,3);
		_delay_ms(100);
	}

	_delay_ms(1000);

	//Звоним на номера.
	if(temp_config_in4[0] & 1)
	{
		AtdNumber(0);
		_delay_ms(200);
	}

	if(temp_config_in4[0] & 2)
	{
		AtdNumber(1);
		_delay_ms(200);
	}

	if(temp_config_in4[0] & 4)
	{
		AtdNumber(2);
		_delay_ms(200);
	}

	if(temp_config_in4[0] & 8)
	{
		AtdNumber(3);
		_delay_ms(200);
	}

	cli();

	START_TIMER1;

	return 0;

}


void AlarmVisible()
{
	uint8_t temp_config_out_u1[4];
	for(uint8_t i=0; i<sizeof(temp_config_out_u1); i++)	temp_config_out_u1[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_OUT_U1);
		

	uint8_t temp_config_out_u2[4];
	for(uint8_t i=0; i<sizeof(temp_config_out_u2); i++)	temp_config_out_u2[i]=eeprom_read_byte(i+ADDR_EEP_CONFIG_OUT_U2);

//////////////////////////////////////////////////////////////

	if(alarm_s2)
	{
		AlarmIn2();

		if(temp_config_out_u1[0] & 2)
		{
			if(temp_config_out_u1[1]==1) PORT_OUT&=~(RELAY_U1);
			if(temp_config_out_u1[1]==2)
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

		if(temp_config_out_u2[0] & 2)
		{
			if(temp_config_out_u2[1]==1) PORTD&=~(RELAY_U2);
			if(temp_config_out_u2[1]==2)
			{
				if(count_u2<=(temp_config_out_u2[2]+temp_config_out_u2[3]))
				{
					PORTD&=~(RELAY_U2);
					count_u2++;
				}
				else
				{
					PORTD|=(RELAY_U2);
				}
			}
		}
	}

//////////////////////////////////////////////////////////////

	if(alarm_s1)
	{
		AlarmIn1();

		if(temp_config_out_u1[0] & 1)
		{
			if(temp_config_out_u1[1]==1) PORT_OUT&=~(RELAY_U1);
			if(temp_config_out_u1[1]==2)
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

		if(temp_config_out_u2[0] & 1)
		{
			if(temp_config_out_u2[1]==1) PORTD&=~(RELAY_U2);
			if(temp_config_out_u2[1]==2)
			{
				if(count_u2<=(temp_config_out_u2[2]+temp_config_out_u2[3]))
				{
					PORTD&=~(RELAY_U2);
					count_u2++;
				}
				else
				{
					PORTD|=(RELAY_U2);
				}
			}
		}

	}

//////////////////////////////////////////////////////////////

	if(alarm_in3)
	{
		AlarmIn3();
		
		if(temp_config_out_u1[0] & 4)
		{
			if(temp_config_out_u1[1]==1) PORT_OUT&=~(RELAY_U1);
			if(temp_config_out_u1[1]==2)
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

		if(temp_config_out_u2[0] & 4)
		{
			if(temp_config_out_u2[1]==1) PORTD&=~(RELAY_U2);
			if(temp_config_out_u2[1]==2)
			{
				if(count_u2<=(temp_config_out_u2[2]+temp_config_out_u2[3]))
				{
					PORTD&=~(RELAY_U2);
					count_u2++;
				}
				else
				{
					PORTD|=(RELAY_U2);
				}
			}
		}

	}


/////////////////////////////////////////////////////////////////

	if(alarm_in4)
	{
		AlarmIn4();
		
		if(temp_config_out_u1[0] & 8)
		{
			if(temp_config_out_u1[1]==1) PORT_OUT&=~(RELAY_U1);
			if(temp_config_out_u1[1]==2)
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

		if(temp_config_out_u2[0] & 8)
		{
			if(temp_config_out_u2[1]==1) PORTD&=~(RELAY_U2);
			if(temp_config_out_u2[1]==2)
			{
				if(count_u2<=(temp_config_out_u2[2]+temp_config_out_u2[3]))
				{
					PORTD&=~(RELAY_U2);
					count_u2++;
				}
				else
				{
					PORTD|=(RELAY_U2);
				}
			}
		}

	}


/////////////////////////////////////////////////////////////////

	if((alarm_in3) || (alarm_s1) || (alarm_s2) || (alarm_in4))
	{
		PORT_LED|=(1<<LED_RED);
		PORT_OUT&=~(RELAY_BUZ);
		if(PORT_OUT & (RELAY_LED)) PORT_OUT&=~(RELAY_LED); else PORT_OUT|=(RELAY_LED);
		
		uint8_t temp_config_time_alarm_visible=0;
		uint16_t temp_time_alarm_visible=0;
		temp_config_time_alarm_visible=eeprom_read_byte(ADDR_EEP_CONFIG_TIME_ALARM_VISIBLE);
		temp_time_alarm_visible=temp_config_time_alarm_visible*60;

		if(temp_time_alarm_visible<=time_alarm_visible)
		{
						alarm_s1=0;
						alarm_time_s1=0;
					//	time_alarm_u1=0;
					//	sec_u1=0;
					//	sec_on_s1=0;
					//	time_sec_u1=0;

					//	sec_in3=0;
					//	sec_on_in3=0;
					//	time_sec_in3=0;
						alarm_in3=0;
						alarm_time_in3=0;
					//	time_alarm_in3=0;

						alarm_in4=0;
						alarm_time_in4=0;
					//	time_alarm_in4=0;

						alarm_s2=0;

						count_u1=0;
						count_u2=0;

						PORT_OUT|=(RELAY_BUZ);
						PORT_OUT&=~(RELAY_LED);

						PORT_OUT|=(RELAY_U1);
						PORTD|=(RELAY_U2);

						PORT_LED&=~(1<<LED_RED);
						PORT_LED|=(1<<LED_GREEN);

						time_alarm_visible=0;

		}
		else
		{
			time_alarm_visible++;
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
			sec_u1=0;

			if(eeprom_read_byte(ADDR_EEP_CONFIG_U1+3)==1)
			{
				sec_on_in3=1;
			}

			if(eeprom_read_byte(ADDR_EEP_CONFIG_U2+3)==1)
			{
				sec_on_in4=1;
			}


			
		}
		else
		{
			
			if(PORT_LED & (1<<LED_GREEN)) PORT_LED&=~(1<<LED_GREEN); else PORT_LED|=(1<<LED_GREEN);

			time_sec_u1++;

		}	
				
	}

}

void ClockAlarm()
{

	if(alarm_time_s1)
	{
		uint8_t temp_config_alarm_time_us=0;
		uint16_t temp_time_alarm_u1=0;	
		temp_config_alarm_time_us=eeprom_read_byte(ADDR_EEP_CONFIG_TIME_ALARM_US+0);
		temp_time_alarm_u1=temp_config_alarm_time_us*60;

		if(temp_time_alarm_u1<=time_alarm_u1)
		{
			alarm_s1=1;
		}
		else
		{
			time_alarm_u1++;
		}

	}


////////////////////////////////////////////////////////////////////////////////////////


	if(alarm_time_in3)
	{
		uint8_t temp_config_time_alarm_us=0;
		uint16_t temp_time_alarm_in3=0;	
		temp_config_time_alarm_us=eeprom_read_byte(ADDR_EEP_CONFIG_TIME_ALARM_US+0);
		temp_time_alarm_in3=temp_config_time_alarm_us*60;

		if(temp_time_alarm_in3<=time_alarm_in3)
		{
			alarm_in3=1;
		}
		else
		{
			time_alarm_in3++;
		}

	}
	
///////////////////////////////////////////////////


	if(alarm_time_in4)
	{
		uint8_t temp_config_time_alarm_us=0;
		uint16_t temp_time_alarm_in4=0;	
		temp_config_time_alarm_us=eeprom_read_byte(ADDR_EEP_CONFIG_TIME_ALARM_US+0);
		temp_time_alarm_in4=temp_config_time_alarm_us*60;

		if(temp_time_alarm_in4<=time_alarm_in4)
		{
			alarm_in4=1;
		}
		else
		{
			time_alarm_in4++;
		}

	}
	
	if(((alarm_time_in3) || (alarm_time_s1) || (alarm_time_in4)) && (!(alarm_s1) && !(alarm_in3) && !(alarm_in4)))
	{
		if(PORT_LED & (1<<LED_RED)) PORT_LED&=~(1<<LED_RED); else PORT_LED|=(1<<LED_RED);
	}


}




ISR(TIMER1_COMPA_vect)				//Interrupt 1c.
{
	TCNT1=0x0000;

	AlarmVisible();

	ClockSecU1();

	ClockAlarm();

	if(!(sim_sleep)) SaveConfig();

}


