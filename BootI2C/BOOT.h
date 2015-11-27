
	#include <avr/boot.h>
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>


	void boot_write_page (uint32_t page, uint8_t *buf, uint8_t len);

	void boot_read_page (uint32_t page, uint8_t *buf, uint8_t len);
	
	void boot_erase_page (uint32_t page);



