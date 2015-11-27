



	#include "GPS.h"
	
	
uint8_t gps_status(char *pDataRX, uint8_t Len)
{

	for(uint8_t i=0; i<Len; i++)
	{
		if((pDataRX[i]=='C') && (pDataRX[i+1]==','))
		{
			if(((i+1)>Len) && ((i+13)>Len))
				return 1;

			if(pDataRX[i+13]=='A')
				return 0;
		}

	}

	return 1;
}


uint8_t gps_time(char *pDataTX, char *pDataRX, uint8_t Len)
{
	for(uint8_t i=0; i<Len; i++)
	{
		if((pDataRX[i]=='C') && (pDataRX[i+1]==','))
		{
			if(((i+1)>Len) || ((i+7)>Len))
				return 1;

			pDataTX[0]=pDataRX[i+2];
			pDataTX[1]=pDataRX[i+3];
			pDataTX[2]=':';
			pDataTX[3]=pDataRX[i+4];
			pDataTX[4]=pDataRX[i+5];
			pDataTX[5]=':';
			pDataTX[6]=pDataRX[i+6];
			pDataTX[7]=pDataRX[i+7];

			return 0;
		}

	}

	return 1;

}

uint8_t gps_latitude(char *pDataTX, char *pDataRX, uint8_t Len)
{
	for(uint8_t i=0; i<Len; i++)
	{
		if((pDataRX[i]=='C') && (pDataRX[i+1]==','))
		{
			
			for(uint8_t n=0; n<11; n++)
				pDataTX[n]=pDataRX[n+22]; //22

			if((pDataTX[10]!='N'))
				return 1;

			return 0;
		}
	}

	return 1;
}


uint8_t gps_longitude(char *pDataTX, char *pDataRX, uint8_t Len)
{
	for(uint8_t i=0; i<Len; i++)
	{
		if((pDataRX[i]=='C') && (pDataRX[i+1]==','))
		{
			
			for(uint8_t n=0; n<11; n++)
				pDataTX[n]=pDataRX[n+35]; //22

			if((pDataTX[10]!='E'))
				return 1;

			return 0;
		}
	}

	return 1;
}



