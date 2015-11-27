

	#include "NTP.h"
	#include "RTC.h"
	#include "BCD.h"


// МАКРОСЫ АТ КОМАНД ДЛЯ РАБОТЫ ПО GPRS //
	#define CGATT			for(uint8_t n=0; n<sizeof(gprs_reg); n++)	usart0_write(gprs_reg[n]);				//Регистрация в сети GPRS.
	#define CIPCSGP			for(uint8_t n=0; n<sizeof(gprs_mod); n++)	usart0_write(gprs_mod[n]);				//Вход в параметры GPRS.
	#define CDNSORIP		for(uint8_t n=0; n<sizeof(dns_or_ip); n++)	usart0_write(dns_or_ip[n]);				//Имя сервера в виде символов.
	#define CIPSTART		for(uint8_t n=0; n<sizeof(gprs_start); n++)	usart0_write(gprs_start[n]);			//Открываем соединение.
	#define CIPSTART_END	for(uint8_t n=0; n<sizeof(gprs_start_end); n++)	usart0_write(gprs_start_end[n]);	//Завершаем ввод, пишем порт 37.
	#define CIPSEND			for(uint8_t n=0; n<sizeof(send); n++)	usart0_write(send[n]);						//Хотим отправить сообщение.
	#define CIPSHUT			for(uint8_t n=0; n<sizeof(shut); n++)	usart0_write(shut[n]);						//Хотим отправить сообщение.

//	#define TIMEOUT_MAX 10


// ПЕРЕМЕННЫЕ НЕОБХОДИМЫЕ МАКРОСАМ //
	const char gprs_reg[]={'A','T','+','C','G','A','T','T','=','1', 0x0D, 0x0A}; //
	const char gprs_mod[]={'A','T','+','C','I','P','C','S','G','P','=','1',',','"'}; //
	const char dns_or_ip[]={'A','T','+','C','D','N','S','O','R','I','P','=','1',0x0D, 0x0A};				 // Доменное имя сервера.
	const char gprs_start[]={'A','T','+','C','I','P','S','T','A','R','T','=','"','U','D','P','"',',','"'};	 //
	const char gprs_start_end[]={'"',',','"','3','7','"', 0x0D, 0x0A};	 //37, 80, 
	const char send[]={'A','T','+','C','I','P','S','E','N','D', 0x0D, 0x0A};	 //
	const char shut[]={'A','T','+','C','I','P','S','H','U','T', 0x0D, 0x0A};	 //


	const char echo_on1[]={'A','T','E', '1', 0x0D, 0x0A};

	const char atv_on1[]={'A','T','V', '1', 0x0D, 0x0A};

	
	char apn_eeprom[]EEMEM="internet.beeline.ru ";				//APN.
	char user_name_eeprom[]EEMEM="beeline   ";					//Имя пользователя.
	char user_password_eeprom[]EEMEM="beeline   ";				//Пароль пользователя
	char domain_name1[]EEMEM="ntp1.vniiftri.ru              ";	//Имя NTP1 сервера.
	char domain_name2[]EEMEM="ntp2.vniiftri.ru              ";	//Имя NTP2 сервера.
	uint8_t config_gprs EEMEM=0b00000000;
	uint8_t time_zone EEMEM=4;


uint8_t GetTimeNTP(uint8_t domain_name)
{


	// Читаем APN //
		uint8_t len=0;
		for(uint8_t i=0; i<(ADDR_EEP_USER_NAME-ADDR_EEP_APN); i++)
		{
			if(eeprom_read_byte(i+ADDR_EEP_APN)==' ') break;
			len++;
		}
	
		char temp_apn[len];

		for(uint8_t i=0; i<len; i++)
		{
			temp_apn[i]=(eeprom_read_byte(i+ADDR_EEP_APN));
		}


	////////////////////////////////////////////////////////////////////////////////

	// Читаем имя пользователя //

		len=0;
		for(uint8_t i=0; i<(ADDR_EEP_USER_PAS-ADDR_EEP_USER_NAME); i++)
		{
			if(eeprom_read_byte(i+ADDR_EEP_USER_NAME)==' ') break;
			len++;
		}
	
		char temp_user_name[len];

		for(uint8_t i=0; i<len; i++)
		{
			temp_user_name[i]=(eeprom_read_byte(i+ADDR_EEP_USER_NAME));
		}

	////////////////////////////////////////////////////////////////////////////////


	// Читаем пароль пользователя //

		len=0;
		for(uint8_t i=0; i<(ADDR_EEP_USER_PAS_END-ADDR_EEP_USER_PAS); i++)
		{
			if(eeprom_read_byte(i+ADDR_EEP_USER_PAS)==' ') break;
			len++;
		}
	
		char temp_user_pasvord[len];

		for(uint8_t i=0; i<len; i++)
		{
			temp_user_pasvord[i]=(eeprom_read_byte(i+ADDR_EEP_USER_PAS));
		}


	////////////////////////////////////////////////////////////////////////////////

		// Читаем доменное имя NTP сервера//

			len=0;
			
			if(domain_name==1)
			{
				for(uint8_t i=0; i<(ADDR_EEP_DOMAIN_NAME_END1-ADDR_EEP_DOMAIN_NAME1); i++)
				{
					if(eeprom_read_byte(i+ADDR_EEP_DOMAIN_NAME1)==' ') break;
					len++;
				}
			}
			
			if(domain_name==2)
			{
				for(uint8_t i=0; i<(ADDR_EEP_DOMAIN_NAME_END2-ADDR_EEP_DOMAIN_NAME2); i++)
				{
					if(eeprom_read_byte(i+ADDR_EEP_DOMAIN_NAME2)==' ') break;
					len++;
				}
			}

			char temp_domain_name[len];

			if(domain_name==1)
			{		
				for(uint8_t i=0; i<len; i++)
				{
					temp_domain_name[i]=(eeprom_read_byte(i+ADDR_EEP_DOMAIN_NAME2));
				}
			}

			if(domain_name==2)
			{		
				for(uint8_t i=0; i<len; i++)
				{
					temp_domain_name[i]=(eeprom_read_byte(i+ADDR_EEP_DOMAIN_NAME2));
				}
			}

	////////////////////////////////////////////////////////////////////////////////
	
// Пробуем конект //

	CGATT;
	while(ok());
//	_delay_ms(2000);

	CIPCSGP;

	//Передаём APN.
	for(uint8_t n=0; n<sizeof(temp_apn); n++)	usart0_write(temp_apn[n]);		

	usart0_write('"');
	usart0_write(',');
	usart0_write('"');

	//Передаём Имя пользователя.
	for(uint8_t n=0; n<sizeof(temp_user_name); n++)	usart0_write(temp_user_name[n]);

	usart0_write('"');
	usart0_write(',');
	usart0_write('"');

	//Передаём Пароль пользователя.
	for(uint8_t n=0; n<sizeof(temp_user_pasvord); n++)	usart0_write(temp_user_pasvord[n]);

	usart0_write('"');
	usart0_write(0x0D);
	usart0_write(0x0A);

	while(ok());
//	_delay_ms(2000);

	CDNSORIP;
	while(ok());
//	_delay_ms(2000);

// Пробуем открыть соединение //
	CIPSTART;
	for(uint8_t n=0; n<sizeof(temp_domain_name); n++)	usart0_write(temp_domain_name[n]);	// Наш NTP сервер.
	CIPSTART_END;


	while(1)
	{
		while(!(usart0_rx_len()));
		uint8_t status=usart0_read();

		if(status=='9') return 9;
		if(status=='3') return 3;
		if(status=='4') return 4;
		if(status=='0') break;

	}


	while(1)
	{
		while(!(usart0_rx_len()));
		uint8_t status=usart0_read();

		if(status=='9') return 9;
		if(status=='3') return 3;
		if(status=='4') return 4;
		if(status=='8') break;

	}
	
	CIPSEND;
	
	//Ждем преглащения >.
	while(1)
	{
		while(!(usart0_rx_len()));
		uint8_t status=usart0_read();
		if(status=='>') break;


	}

	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	_delay_ms(500);

	usart0_write(0x1A);		//Отправляем пустой UDP запрос на NTP сервер.

		//Ждем SEND OK//	
		char status;

		while(1)
		{
			while(!(usart0_rx_len()));
			status=usart0_read();
			if(status=='K') break;
			if(status=='F') return status;
			if(status=='R') return status;
		}	


	usart0_clear_rx_buffer();

	status=0;
	while(status!=0x0A)
	{
		uint16_t timeout=10000;
		while(!(usart0_rx_len()))
		{
			timeout--;
			if(!(timeout))
			{
			 	CIPSHUT;		///Закрываем соединение по тайм ауту.
				_delay_ms(1000);
				return 0xFF;
			}

		}

		status=usart0_read();
	}
	
	usart0_clear_rx_buffer();

	uint8_t temp_time_ntp[4];
	
	for(uint8_t i=0; i<3; i++) _delay_ms(1000);

	if(!(usart0_rx_len()))
	{
		CIPSHUT;				//Закрываем соединение.
		_delay_ms(1000);
		return 0xFF;
		
	}

	for(uint8_t i=0; i<sizeof(temp_time_ntp); i++) temp_time_ntp[i]=usart0_read();

	CIPSHUT;				//Закрываем соединение.

	_delay_ms(100);

	uint32_t time_ntp=0;

	for(uint8_t i=0; i<sizeof(time_ntp); i++)
	{
		time_ntp|=temp_time_ntp[i];
		if(i!=3) time_ntp=time_ntp<<8;
	}

	uint32_t time_day_ntp=time_ntp/86400;

	uint32_t temp_sec_ntp=time_ntp-(time_day_ntp*86400);

	uint16_t temp_min_ntp=temp_sec_ntp/60;


	isec=temp_sec_ntp-(temp_min_ntp*60);

	ihours_ntp=temp_min_ntp/60;

	imin=temp_min_ntp-(ihours_ntp*60);

	ihours=ihours_ntp+eeprom_read_byte(&time_zone);
	
	if(ihours>23) ihours=ihours-24;

	uint8_t temp_csec=BCDFormat(isec);
	uint8_t temp_cmin=BCDFormat(imin);
	uint8_t temp_chours=BCDFormat(ihours);

	csec[0]=((temp_csec & 0xF0)>>4)+0x30;
	csec[1]=(temp_csec & 0x0F)+0x30;

	cmin[0]=((temp_cmin & 0xF0)>>4)+0x30;
	cmin[1]=(temp_cmin & 0x0F)+0x30;

	chours[0]=((temp_chours & 0xF0)>>4)+0x30;
	chours[1]=(temp_chours & 0x0F)+0x30;

	return 0;
}


