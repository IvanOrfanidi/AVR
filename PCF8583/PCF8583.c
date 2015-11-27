

	#include "PCF8583.h"

	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>

	
uint8_t GetDataTimeRTC(char *pDateTime, uint8_t len)
{
	uint8_t year_bcd=TWIReadByte(SLAVE_ADDR_RTC,1,ADDR_YEUR);

	pDateTime[0]=(year_bcd>>4)+0x30;
	pDateTime[1]=(year_bcd & 0x0F)+0x30;

	pDateTime[2]='/';

	uint8_t mon_bcd=TWIReadByte(SLAVE_ADDR_RTC,1,ADDR_MON);

	pDateTime[3]=(mon_bcd>>4)+0x30;
	pDateTime[4]=(mon_bcd & 0x0F)+0x30;

	pDateTime[5]='/';

	uint8_t day_bcd=TWIReadByte(SLAVE_ADDR_RTC,1,ADDR_DAY);

	pDateTime[6]=(day_bcd>>4)+0x30;
	pDateTime[7]=(day_bcd & 0x0F)+0x30;

	pDateTime[8]=',';

	uint8_t hour_bcd=TWIReadByte(SLAVE_ADDR_RTC,1,ADDR_HOUR);

	pDateTime[9]=(hour_bcd>>4)+0x30;
	pDateTime[10]=(hour_bcd & 0x0F)+0x30;

	pDateTime[11]=':';

	uint8_t min_bcd=TWIReadByte(SLAVE_ADDR_RTC,1,ADDR_MIN);

	pDateTime[12]=(min_bcd>>4)+0x30;
	pDateTime[13]=(min_bcd & 0x0F)+0x30;

	pDateTime[14]=':';

	uint8_t sec_bcd=TWIReadByte(SLAVE_ADDR_RTC,1,ADDR_SEC);

	pDateTime[15]=(sec_bcd>>4)+0x30;
	pDateTime[16]=(sec_bcd & 0x0F)+0x30;

	
return 0;
}


uint8_t SetDataTimeRTC(const char *pDateTime)
{

	if((pDateTime[2]!='/') || (pDateTime[5]!='/') || (pDateTime[8]!=',') || (pDateTime[11]!=':') || (pDateTime[14]!=':')) return 2;

	uint8_t year_bcd=((pDateTime[0] & 0x0F)<<4);
	year_bcd|=(pDateTime[1] & 0x0F);

	if(TWIWriteByte(SLAVE_ADDR_RTC,1,ADDR_YEUR,year_bcd)) return 1;

	uint8_t mon_bcd=((pDateTime[3] & 0x0F)<<4);
	mon_bcd|=(pDateTime[4] & 0x0F);

	if(TWIWriteByte(SLAVE_ADDR_RTC,1,ADDR_MON,mon_bcd)) return 1;

	uint8_t day_bcd=((pDateTime[6] & 0x0F)<<4);
	day_bcd|=(pDateTime[7] & 0x0F);

	if(TWIWriteByte(SLAVE_ADDR_RTC,1,ADDR_DAY,day_bcd)) return 1;


	uint8_t hour_bcd=((pDateTime[9] & 0x0F)<<4);
	hour_bcd|=(pDateTime[10] & 0x0F);

	if(TWIWriteByte(SLAVE_ADDR_RTC,1,ADDR_HOUR,hour_bcd)) return 1;

	uint8_t min_bcd=((pDateTime[12] & 0x0F)<<4);
	min_bcd|=(pDateTime[13] & 0x0F);

	if(TWIWriteByte(SLAVE_ADDR_RTC,1,ADDR_MIN,min_bcd)) return 1;

	uint8_t sec_bcd=((pDateTime[15] & 0x0F)<<4);
	sec_bcd|=(pDateTime[16] & 0x0F);

	if(TWIWriteByte(SLAVE_ADDR_RTC,1,ADDR_SEC,sec_bcd)) return 1;


return 0;
}




uint8_t GetTimeRTC(char *pDateTime, uint8_t len)
{
	
	uint8_t hour_bcd=TWIReadByte(SLAVE_ADDR_RTC,1,ADDR_HOUR);

	pDateTime[0]=(hour_bcd>>4)+0x30;
	pDateTime[1]=(hour_bcd & 0x0F)+0x30;

	pDateTime[2]=':';

	uint8_t min_bcd=TWIReadByte(SLAVE_ADDR_RTC,1,ADDR_MIN);

	pDateTime[3]=(min_bcd>>4)+0x30;
	pDateTime[4]=(min_bcd & 0x0F)+0x30;

	pDateTime[5]=':';

	uint8_t sec_bcd=TWIReadByte(SLAVE_ADDR_RTC,1,ADDR_SEC);

	pDateTime[6]=(sec_bcd>>4)+0x30;
	pDateTime[7]=(sec_bcd & 0x0F)+0x30;


return 0;
}


uint8_t SetTimeRTC(const char *pDateTime)
{
	if((pDateTime[2]!=':') ||(pDateTime[5]!=':')) return 2;

	uint8_t hour_bcd=((pDateTime[0] & 0x0F)<<4);
	hour_bcd|=(pDateTime[1] & 0x0F);

	if(TWIWriteByte(SLAVE_ADDR_RTC,1,ADDR_HOUR,hour_bcd)) return 1;

	uint8_t min_bcd=((pDateTime[3] & 0x0F)<<4);
	min_bcd|=(pDateTime[4] & 0x0F);

	if(TWIWriteByte(SLAVE_ADDR_RTC,1,ADDR_MIN,min_bcd)) return 1;

	uint8_t sec_bcd=((pDateTime[6] & 0x0F)<<4);
	sec_bcd|=(pDateTime[7] & 0x0F);

	if(TWIWriteByte(SLAVE_ADDR_RTC,1,ADDR_SEC,sec_bcd)) return 1;


return 0;
}
