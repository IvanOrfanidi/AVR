

	#include "SIM.h"

	#define true      1
	#define false     0
	#define bool      _Bool



int SignalQualityReport(char *ptrCSQ)
{
	const char at_csq[]={'A','T','+','C','S','Q', 0x0D, 0x0A};

	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	//RX Command//
	for(uint8_t i=0; i<sizeof(at_csq); i++)
	{
		usart0_write(at_csq[i]);
		_delay_ms(USART0_DELAY_MS);
	}

	//Response.
	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	uint8_t Len = usart0_rx_len();
	uint8_t symbol = 0;
	uint8_t temp = 0;
	while(temp!=0x20)		//space
	{
		temp = usart0_read();
		symbol++;
		if(symbol==Len)	return ERROR;
	}		


	uint8_t LenCSQ = 0;
	temp = 0;
	while(temp!=',')
	{
		temp = usart0_read();
		ptrCSQ[LenCSQ] = temp;
		LenCSQ++;
	}

	ptrCSQ[LenCSQ-1] = 0;

	return OK;

}


int GetServiceProviderNameFromSIM(char *ptrResponseProviderName)
{
	const char at_cspn[]={'A','T','+','C','S','P','N','?', 0x0D, 0x0A};

	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	//RX Command//
	for(uint8_t i=0; i<sizeof(at_cspn); i++)
	{
		usart0_write(at_cspn[i]);
		_delay_ms(USART0_DELAY_MS);
	}


	//Response.
//	while(!(usart0_rx_len()));
//	_delay_ms(DELAY_RESPONSE);
	_delay_ms(DELAY_RESPONSE);
	
	uint8_t Len = usart0_rx_len();
	if(!(Len))	return ERROR;

	uint8_t symbol = 0;
	uint8_t temp = 0;
	while(temp!='"')
	{
		temp = usart0_read();
		symbol++;
		if(symbol==Len)	return ERROR;
	}		


	uint8_t LenResponseProviderName = 0;
	temp = 0;
	while(temp!='"')
	{
		temp = usart0_read();
		ptrResponseProviderName[LenResponseProviderName] = temp;
		LenResponseProviderName++;
	}

	ptrResponseProviderName[LenResponseProviderName-1] = 0;

	return OK;
	
}


int UnstructuredSupplementaryServiceData(const char *ptrUSSD, char *ptrResponseUSSD)
{
	uint8_t LenUSSD = 0;
	while(ptrUSSD[LenUSSD]!=0)
		LenUSSD++;

	const char at_cusd0[]={'A','T','+','C','U','S','D','=','1',',','"'};
	const char at_cusd1[]={'"', 0x0D, 0x0A};
	char USSD[LenUSSD + sizeof(at_cusd0) +  sizeof(at_cusd1)];

	for(uint8_t i=0; i<sizeof(at_cusd0); i++)
		USSD[i] = at_cusd0[i];	
	
	for(uint8_t i=sizeof(at_cusd0); i<(sizeof(at_cusd0) + LenUSSD); i++)
		USSD[i] = ptrUSSD[i - sizeof(at_cusd0)];	

	for(uint8_t i=(sizeof(at_cusd0) + LenUSSD); i<sizeof(USSD); i++)
		USSD[i] = at_cusd1[i - (sizeof(at_cusd0)+LenUSSD)];
	
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	//RX Command//
	for(uint8_t i=0; i<sizeof(USSD); i++)
	{
		usart0_write(USSD[i]);
		_delay_ms(USART0_DELAY_MS);
	}		

	for(uint8_t i=0; i<TIME_DELAY_USSD_RESPONSE; i++)	
		_delay_ms(1000);


	//Response.
	uint8_t Len = usart0_rx_len();
	if(!(Len))	return ERROR;

	uint8_t symbol = 0;
	uint8_t temp = 0;
	while(temp!='"')
	{
		temp = usart0_read();
		symbol++;
		if(symbol==Len)	return ERROR;
	}

	uint8_t LenResponseUSSD = 0;
	temp = 0;
	uint8_t space_count = 0;
	while(temp!='"')
	{
		temp = usart0_read();
		ptrResponseUSSD[LenResponseUSSD] = temp;
		LenResponseUSSD++;

		if((temp==0x2E)||(temp==0x30)) space_count++;
		if(space_count>1)
		{
			LenResponseUSSD++;
			temp = '"';
		}
	}

	ptrResponseUSSD[LenResponseUSSD-1] = 0;

	return OK;

}

int SendSMS(const char *ptrTelephoneNumber, const char *ptrTextSMS)
{

	uint8_t LenTelephoneNumber = 0;
	while(ptrTelephoneNumber[LenTelephoneNumber]!=0)
		LenTelephoneNumber++;

	const char at_cmgs0[]={'A','T','+','C','M','G','S','=','"'};
	const char at_cmgs1[]={'"', 0x0D, 0x0A};
	char CMD_Send_SMS[LenTelephoneNumber + sizeof(at_cmgs0) + sizeof(at_cmgs1)];


	for(uint8_t i=0; i<sizeof(at_cmgs0); i++)
		CMD_Send_SMS[i] = at_cmgs0[i];

	for(uint8_t i=sizeof(at_cmgs0); i<(sizeof(at_cmgs0)+(LenTelephoneNumber)); i++)
		CMD_Send_SMS[i] = ptrTelephoneNumber[i-sizeof(at_cmgs0)];

	for(uint8_t i=(sizeof(at_cmgs0)+(LenTelephoneNumber)); i<sizeof(CMD_Send_SMS); i++)
		CMD_Send_SMS[i] = at_cmgs1[i-(sizeof(at_cmgs0)+(LenTelephoneNumber))];


	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	//RX Command//
	for(uint8_t i=0; i<sizeof(CMD_Send_SMS); i++)
	{
		usart0_write(CMD_Send_SMS[i]);
		_delay_ms(USART0_DELAY_MS);
	}		

	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	uint8_t temp = 0;
	bool sms_true = false;
	uint8_t Len = usart0_rx_len();

	for(uint8_t i=0; i<Len; i++)
	{
		temp = usart0_read();

		if(temp=='>') sms_true = true;	
	}

	if(!(sms_true)) return ERROR;

	uint8_t LenTextSMS = 0;
	while(ptrTextSMS[LenTextSMS]!=0)
		LenTextSMS++;

	char TextSMS[LenTextSMS+1];

	for(uint8_t i=0; i<LenTextSMS; i++)
		TextSMS[i] = ptrTextSMS[i];
	
	TextSMS[sizeof(TextSMS)-1] = 0x1A; //End Text SMS.
	
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	//RX Text SMS//
	for(uint8_t i=0; i<sizeof(TextSMS); i++)
	{
		usart0_write(TextSMS[i]);
		_delay_ms(USART0_DELAY_MS);
	}		

	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	if(Ok()) return ERROR;

	return OK;

}



int SimWriteCmd(const char *ptrCMD, char *ptrResponse)
{
	uint8_t len_ptrCMD=0;
	while(ptrCMD[len_ptrCMD]!=0)
		len_ptrCMD++;

	uint8_t cmd[len_ptrCMD+2];
	for(uint8_t i=0; i<sizeof(cmd); i++)
		cmd[i]=ptrCMD[i];

	cmd[sizeof(cmd)-2] = 0x0D;
	cmd[sizeof(cmd)-1] = 0x0A;

	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();
	
	for(uint8_t i=0; i<sizeof(cmd); i++)
	{	
		usart0_write(cmd[i]);
		_delay_ms(USART0_DELAY_MS);
	}
	
	while(!(usart0_rx_len()));

	uint8_t LenResponse = 0;
	uint8_t LenResponseBack = 0;
	uint16_t count_delay_response = MAX_COUNT_DELAY_RESPONSE;
	
	while(count_delay_response)
	{
		if(LenResponse==LenResponseBack)
		{
			count_delay_response--;
		}
		else
		{
			LenResponseBack = LenResponse;
			count_delay_response = MAX_COUNT_DELAY_RESPONSE;
		}

		LenResponse = usart0_rx_len();
	}

	LenResponse = usart0_rx_len();

	if(!(LenResponse))	return ERROR;
	
	uint8_t n = 0;
	for(uint8_t i=0; i<LenResponse; i++)
		{
			uint8_t temp = usart0_read();
			if(!((temp==0x0D) || (temp==0x0A)))
			{
				ptrResponse[n] = temp;
				n++;
			}
		}

	ptrResponse[n-1] = 0; //End Response.

	return OK;
}

int ConfigGsmModules(void)
{
	const char echo_off[]={'A','T','E', '0', 0x0D, 0x0A};

	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	//Отключаем эхо от модуля.
	for(uint8_t i=0; i<sizeof(echo_off); i++)
	{	
		usart0_write(echo_off[i]);	//ATE0.
		_delay_ms(USART0_DELAY_MS);
	}

	
	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);



	if(Ok()) return ERROR;

	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	_delay_ms(DELAY_PAUSE_AT_CMD);


	//Отключаем словесный формат ответа. Т.е. теперь будут приходить коды ответа.
	const char atv_off[]={'A','T','V', '0', 0x0D};
	for(uint8_t i=0; i<sizeof(atv_off); i++)
	{	
		usart0_write(atv_off[i]);	//ATV0.
		_delay_ms(USART0_DELAY_MS);
	}
	
	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	if(Ok()) return ERROR;

	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	_delay_ms(DELAY_PAUSE_AT_CMD);



	//Устанавливаем текстовый режим SMS сообщений.
	const char format_text[]={'A','T','+','C','M','G','F','=','1', 0x0D, 0x0A};
	for(uint8_t i=0; i<sizeof(format_text); i++)
	{ 
		usart0_write(format_text[i]);
		_delay_ms(USART0_DELAY_MS);
	}
	
	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	if(Ok()) return ERROR;

	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	_delay_ms(DELAY_PAUSE_AT_CMD);

#ifdef UCS2	//Устанавливаем SMS кодировку Юникод (по умолчанию GSM).

	const char text_format_ucs2[]={'A','T','+','C','S','C','S','=','"','U','C','S','2','"', 0x0D, 0x0A};
	for(uint8_t i=0; i<sizeof(text_format_ucs2); i++)
	{ 
		usart0_write(text_format_ucs2[i]);
		_delay_ms(USART0_DELAY_MS);
	}
	
	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	if(Ok()) return ERROR;

#endif

#ifdef GSM	//Устанавливаем SMS кодировку GSM.

	const char text_format_gsm[]={'A','T','+','C','S','C','S','=','"','G','S','M','"', 0x0D, 0x0A};
	for(uint8_t i=0; i<sizeof(text_format_gsm); i++)
	{ 
		usart0_write(text_format_gsm[i]);
		_delay_ms(USART0_DELAY_MS);
	}
	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	if(Ok()) return ERROR;

#endif

#ifdef HEX	//Устанавливаем SMS кодировку HEX (по умолчанию GSM).
	
	const char text_format_hex[]={'A','T','+','C','S','C','S','=','"','H','E','X','"', 0x0D, 0x0A};
	for(uint8_t i=0; i<sizeof(text_format_hex); i++)
	{ 
		usart0_write(text_format_hex[i]);
		_delay_ms(USART0_DELAY_MS);
	}

	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	if(Ok()) return ERROR;

#endif

#ifdef BROADCAST_DISABLE
	//Отключаем широковещательный режим.
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	_delay_ms(DELAY_PAUSE_AT_CMD);

	const char message_disable[]={'A','T','+','C','S','C','B','=','1', 0x0D, 0x0A};
	for(uint8_t i=0; i<sizeof(message_disable); i++)
	{ 
		usart0_write(message_disable[i]);
		_delay_ms(USART0_DELAY_MS);
	}

	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	if(Ok()) return ERROR;

#endif

#ifdef AON_ENABLE
	//Enable Calling Line Identification Presentation.
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	_delay_ms(DELAY_PAUSE_AT_CMD);

	const char at_clip[]={'A','T','+','C','L','I','P','=','1', 0x0D, 0x0A};
	for(uint8_t i=0; i<sizeof(at_clip); i++)
	{ 
		usart0_write(at_clip[i]);
		_delay_ms(USART0_DELAY_MS);
	}

	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	if(Ok()) return ERROR;

#endif


	return OK;
}

int AtOk(void)
{
	usart0_clear_rx_buffer();
	usart0_clear_tx_buffer();

	const char at[]="AT\r\n";
	for(uint8_t i=0; i<strlen(at); i++)
	{	
		usart0_write(at[i]);	//AT.
		_delay_ms(USART0_DELAY_MS);
	}
	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	if(Ok()) {
	/*
		usart0_clear_rx_buffer();
		usart0_clear_tx_buffer();

		const char at[]={'A', 'T', 0x0D, 0x0A};
		for(uint8_t i=0; i<sizeof(at); i++)
		{	
			usart0_write(at[i]);	//AT.
			_delay_ms(USART0_DELAY_MS);
		}
		while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);
	*/
		return ERROR;
	}
		
	return OK;
}


int Ok(void)
{
	uint8_t temp = 0xFF;
	bool ok_true = false;
	uint8_t Len = usart0_rx_len();

	for(uint8_t i=0; i<Len; i++)
	{
		temp = usart0_read();

		if((temp=='0') | (temp=='K')) ok_true = true;	
	}

	if(!(ok_true))	return ERROR;

	return OK;
}


int GsmReg(void)
{
	const char at_creg[]={'A','T','+','C','R','E','G','?', 0x0D, 0x0A};

	for(uint8_t i=0; i<sizeof(at_creg); i++)
	{ 
		usart0_write(at_creg[i]);
		_delay_ms(USART0_DELAY_MS);
	}

	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);
	
	uint8_t temp = 0;
	while(temp!=',')
		temp = usart0_read();

	temp = usart0_read();
	if((temp=='1')||(temp=='5')) return OK;

	return ERROR;
			
}



void GetTimeSIM(char *ptrTime)
{
	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	const char at_get_time[]={'A','T','+','C','C','L','K','?', 0x0D, 0x0A};
	for(uint8_t i=0; i<sizeof(at_get_time); i++)
	{ 
		usart0_write(at_get_time[i]);
		_delay_ms(USART0_DELAY_MS);
	}
	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	uint8_t temp = 0;

	while(temp!=',')
		temp = usart0_read();
	
	for(uint8_t i=0; i<8; i++)
		ptrTime[i] = usart0_read();

}


void GetDateSIM(char *ptrDate)
{

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	const char at_get_time[]={'A','T','+','C','C','L','K','?', 0x0D, 0x0A};
	for(uint8_t i=0; i<sizeof(at_get_time); i++)
	{ 
		usart0_write(at_get_time[i]);
		_delay_ms(USART0_DELAY_MS);
	}

	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	uint8_t temp = 0;

	while(temp!='"')
		temp = usart0_read();
	
	for(uint8_t i=0; i<8; i++)
		ptrDate[i] = usart0_read();
	
}

int SetTimeSIM(const char *ptrTime)
{

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	char temp_date[8];

	GetDateSIM(temp_date);

	char at_set_time0[32] = {'A','T','+','C','C','L','K','=', '"'};
	char at_set_time1[] = {'+','0','0','"',0x0D,0x0A};

	for(uint8_t i=9; i<17; i++)
		at_set_time0[i]=temp_date[i-9];
	at_set_time0[17]=',';
	for(uint8_t i=18; i<26; i++)
		at_set_time0[i]=ptrTime[i-18];
	for(uint8_t i=26; i<sizeof(at_set_time0); i++)
		at_set_time0[i]=at_set_time1[i-26];

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<sizeof(at_set_time0); i++)
	{ 
		usart0_write(at_set_time0[i]);
		_delay_ms(USART0_DELAY_MS);
	}
	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	if(Ok()) return ERROR;

	return OK;

}

#ifdef SIM_DATE

int SetDateSIM(const char *ptrDate)
{

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	char temp_time[8];

	GetTimeSIM(temp_time);

	char at_set_date0[32] = {'A','T','+','C','C','L','K','=', '"'};
	char at_set_date1[] = {'+','0','0','"',0x0D,0x0A};

	for(uint8_t i=9; i<17; i++)
		at_set_date0[i]=ptrDate[i-9];
	at_set_date0[17]=',';
	for(uint8_t i=18; i<26; i++)
		at_set_date0[i]=temp_time[i-18];
	for(uint8_t i=26; i<sizeof(at_set_date0); i++)
		at_set_date0[i]=at_set_date1[i-26];

	usart0_clear_tx_buffer();
	usart0_clear_rx_buffer();

	for(uint8_t i=0; i<sizeof(at_set_date0); i++)
	{ 
		usart0_write(at_set_date0[i]);
		_delay_ms(USART0_DELAY_MS);
	}

	while(!(usart0_rx_len()));
	_delay_ms(DELAY_RESPONSE);

	if(ok()) return ERROR;

	return OK;

}

#endif


