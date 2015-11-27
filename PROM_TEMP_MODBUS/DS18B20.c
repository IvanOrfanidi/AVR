

#include "DS18B20.h"

unsigned char Temp_H = 0, Temp_L = 0, OK_Flag = 0, temp_flag;


/*** ������������� DS18B20 ***/
unsigned char DS18B20_init(void)
{
	PORT_DS18B20&= ~(1 << DS18B20); // ������������� ������ �������

	DDR_DS18B20|= (1 << DS18B20);

	_delay_us(490);

	DDR_DS18B20&= ~(1 << DS18B20);
	_delay_us(68);

	OK_Flag = (PIN_DS18B20 & (1 << DS18B20)); // ����� ������� ����������� �������
	// ���� OK_Flag = 0 ������ ���������, OK_Flag = 1 ������ �� ���������
	_delay_us(422);

	return OK_Flag;
}

/*** ������� ������ ����� �� DS18B20 ***/
unsigned char read_18b20(void)
{
	unsigned char dat = 0;

	for(uint8_t i = 0; i < 8; i++) 
		{		
		DDR_DS18B20 |= (1 << DS18B20);
		_delay_us(2);        
		DDR_DS18B20 &= ~(1 << DS18B20);
		_delay_us(4);        
		dat = dat >> 1;	 
		   
		if(PIN_DS18B20 & (1 << DS18B20))
		{			
			dat|= 0x80; 
		}
		_delay_us(62);
	}	
	return dat;
}

/*** ������� ������ ����� � DS18B20 ***/
void write_18b20(unsigned char dat)
{
		
	for(uint8_t i = 0; i < 8; i++)
	{
		DDR_DS18B20 |= (1 << DS18B20);
		_delay_us(2);   
		   		
		if(dat & 0x01)
			DDR_DS18B20 &= ~(1 << DS18B20);	
			else
				DDR_DS18B20 |= (1 << DS18B20);

		dat = dat >> 1; 
		_delay_us(62); 

		DDR_DS18B20 &= ~(1 << DS18B20);
		_delay_us(2); 
	}	
}



int DS18B20_Start_Converts(void)
{

	if(OK_Flag == 1) // ���� ������ �� �������
		return ERROR_CONNECT;

	DS18B20_init();        // ������������� DS18B20

	write_18b20(0xCC);     // �������� ���� �������
	write_18b20(0x44);     // ������ �������������� ��������������

	return OK_CONNECT;
}

	
int DS18B20_Temperature(char *ptrTemperature)
{
	uint8_t tempint = 0,tempint1,tempint2,tempint3; // ���������� ��� ������ �������� �����������
	uint16_t temppoint = 0,temppoint1; // ���������� ��� �������� �������� �����������

	DS18B20_init();        // ������������� DS18B20

	write_18b20(0xCC);     // �������� ���� �������
	write_18b20(0xBE);     // ��������� ���������� ���

	Temp_L = read_18b20(); // ������ ������ 2 ����� ��������
	Temp_H = read_18b20();

	temp_flag = 1;         // ���� ����� ����������� ����� 1(����)

	if(Temp_H &(1 << 3))   // ��������� ��� ����� ����������� �� ��������� ������� 
	{			
		signed int tmp;
		temp_flag = 0;      // ���� ����� ����� 0(�����)
		tmp = (Temp_H << 8) | Temp_L;
		tmp = -tmp;
		Temp_L = tmp;
		Temp_H = tmp >> 8; 
	}		

	tempint = ((Temp_H << 4) & 0x70)|(Temp_L >> 4); // ��������� ����� �������� �����������
	tempint1 = tempint % 1000 / 100;  
	tempint2 = (tempint % 100 / 10) + 0x30;  
	tempint3 = (tempint % 10) + 0x30; 
		       
	temppoint = Temp_L & 0x0F; // ��������� ������� �������� �����������
	temppoint = temppoint * 625;       // �������� ����������� 
	temppoint1 = (temppoint / 1000) + 0x30;        
		
		
	if(temp_flag==0) // ���� ���� ����� ����������� ����� 0, � ������ ������� ������ �����
		tempint1 = '-';
		else
			tempint1 = '+';


	ptrTemperature[0]=tempint1;
	ptrTemperature[1]=tempint2;
	ptrTemperature[2]=tempint3;
	ptrTemperature[3]=temppoint1;

	return OK_CONNECT;

}


