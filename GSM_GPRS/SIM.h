
#ifndef SIM_H
   #define SIM_H

	#include "USART.h"

// Input/Output functions.
	#include <avr/io.h>
	#include <util/delay.h>
	#include <avr/pgmspace.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

	#define DELAY_RESPONSE			200
	#define DELAY_PAUSE_AT_CMD		250
	#define MAX_COUNT_DELAY_RESPONSE	250
	#define TIME_DELAY_USSD_RESPONSE	10
	



	#define GSM
//	#define SIM_DATE
	#define BROADCAST_DISABLE
	#define IMEI_NUM_ENABLE
	#define AON_ENABLE

	int SignalQualityReport(char *ptrCSQ);
	int GetServiceProviderNameFromSIM(char *ptrResponseProviderName);
	int UnstructuredSupplementaryServiceData(const char *ptrUSSD, char *ptrResponseUSSD);
	int SendSMS(const char *ptrTelephoneNumber, const char *ptrTextSMS);
	int SimWriteCmd(const char *ptrCMD, char *ptrResponse);
	int AtOk(void);
	int ConfigGsmModules(void);
	int Ok(void);
	void GetTimeSIM(char *ptrTime);
	void GetDateSIM(char *ptrDate);
	int GsmReg(void);
	int SetTimeSIM(const char *ptrTime);
#ifdef SIM_DATE
	int SetDateSIM(const char *ptrDate);
#endif
#ifdef IMEI_NUM_ENABLE
	void SimIMEI(char *ptrIMEI);
#endif


enum SIM {
	OK, ERROR
};

#endif
