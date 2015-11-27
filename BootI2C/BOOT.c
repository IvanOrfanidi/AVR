

#include "BOOT.h"


void boot_write_page (uint32_t page, uint8_t *buf, uint8_t len)
{
	
	eeprom_busy_wait(); 			// �������, ���������� ���������� ������

	boot_page_erase_safe (page);			// ��������� �������� �������� ������ ��������. ����������: page � ��� ����� �����, � �� �����.		

	boot_spm_busy_wait(); 			// �������, ���� �� ���������� ���������� ������ � ������ ��������.



	for (uint32_t i=0; i<SPM_PAGESIZE; i+=2)
	{
		// ��������� ������� �� �������� �������

		uint16_t data=0;
		
		if(len>i+1)
		{
			data = buf[i+1]<<8;
	
			data|=buf[i];
		}
		else
		{
			if(len>i)
			{
				data=buf[i];
				data|=0xFF00;
				
			}
			else
			{
				data=0xFFFF;
			}
		}

		boot_page_fill_safe ((page + i), data);		// ���������� �� ��������� ����� �������� �� ���������� ������ page �������� ������ data.
													// ����������: address � ��� ����� �����, data � ��� 16-������ �������� (�����).
													// AVR �������� ������ ��������, �� ������ ��� ���������� �����!
													// ���������� ������������� ������� ����������� � ���, ����� �� ��������� � ���������
													// ����������� �������� address �� 2, � � data �������� ����� �� 2 �����, ��� ������ �����.

		asm volatile ("wdr");
	}

	boot_page_write_safe (page);		// ��������� ������ ������ ���������� page �������� ������ ��������.

	boot_spm_busy_wait(); 				// �������, ���� �� ���������� ���������� ������ � ������ ��������.

	boot_rww_enable();					// �������� ������ � RWW ������ ������ ��������.
										// ��� �����, ���� ����� ���������� ������ ���� ����������� ������� � �������� ���������.

}

void boot_erase_page(uint32_t page)
{
	eeprom_busy_wait(); 
	boot_page_erase_safe(page);		// ��������� �������� �������� ������ ��������. ����������: address � ��� ����� �����, � �� �����.
	boot_spm_busy_wait();			// �������, ���� �� ���������� ���������� ������ � ������ ��������.

	boot_rww_enable();				// �������� ������ � RWW ������ ������ ��������.
									// ��� �����, ���� ����� ���������� ������ ���� ����������� ������� � �������� ���������.

}

void boot_read_page (uint32_t page, uint8_t *buf, uint8_t len)
{
	eeprom_busy_wait();
	boot_spm_busy_wait();

	for(uint8_t i=0; i<len; i++)
	{

		buf[i]=pgm_read_byte(page+i);

		asm volatile ("wdr");
	}

	
}




