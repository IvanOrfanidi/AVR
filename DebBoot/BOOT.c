

#include "BOOT.h"


void boot_write_page (uint32_t page, uint8_t *buf, uint8_t len)
{
	
	eeprom_busy_wait(); 			// Ожидаем, завершения предыдущей записи

	boot_page_erase_safe (page);			// Выполняет стирание страницы памяти программ. Примечание: page – это адрес байта, а не слова.		

	boot_spm_busy_wait(); 			// Ожидает, пока не завершится инструкция записи в память программ.



	for (uint32_t i=0; i<SPM_PAGESIZE; i+=2)
	{
		// заполняем словами «в обратном порядке»

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

		boot_page_fill_safe ((page + i), data);		// Записывает во временный буфер страницы по указанному адресу page заданные данные data.
													// Примечание: address – это адрес байта, data – это 16-битное значение (слово).
													// AVR адресуют память побайтно, но заодин раз записывают слово!
													// Правильное использование макроса заключается в том, чтобы от обращения к обращению
													// увеличивать значение address на 2, а в data помещать сразу по 2 байта, как единое слово.

		asm volatile ("wdr");
	}

	boot_page_write_safe (page);		// Выполняет запись буфера вуказанную page страницу памяти программ.

	boot_spm_busy_wait(); 				// Ожидает, пока не завершится инструкция записи в память программ.

	boot_rww_enable();					// Включает доступ к RWW секции памяти программ.
										// Это нужно, если после самозаписи должен быть осуществлен переход к основной программе.

}

void boot_erase_page(uint32_t page)
{
	eeprom_busy_wait(); 
	boot_page_erase_safe(page);		// Выполняет стирание страницы памяти программ. Примечание: address – это адрес байта, а не слова.
	boot_spm_busy_wait();			// Ожидает, пока не завершится инструкция записи в память программ.

	boot_rww_enable();				// Включает доступ к RWW секции памяти программ.
									// Это нужно, если после самозаписи должен быть осуществлен переход к основной программе.

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




