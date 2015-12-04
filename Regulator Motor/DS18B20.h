
#ifndef DS18B20_H
   #define DS18B20_H


	#define PORT_DS18B20 	PORTC
	#define PIN_DS18B20 	PINC
	#define DS18B20		PC5
	#define DDR_DS18B20	DDRC

	int DS18B20_Start_Converts(void);
	int DS18B20_Temperature(char *ptrTemperature);
        


#endif
