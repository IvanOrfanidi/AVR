
		#include <avr/io.h>

		#include <util/crc16.h>


	// Настройка адресации устройства и логики протокола.

	#define CMD_READ	0x03

	#define CMD_READ	0x16

	////////////////////////////////////////////////


	// Настройки порта загрузчика.

	#define BAUD_RATE		19200

	#define SELECT_USART	0	//!!! Выбор номера USART.

	#define USART_DELAY_MS 0

	#define ENABLE_RE_DE	1
	#define RE_DE 		PINB1
	#define PORT_RE_DE 	PORTB
	#define DDR_RE_DE	DDRB

	////////////////////////////////////////////////


	// Выбор режима загрузчика. !!! Или BOOT_SIMPLE, или BOOT_WAIT.

	#define BOOT_SIMPLE		0	// Вход по кнопке.
	#define BOOT_WAIT		1	// Вход по ожиданию.

	////////////////////////////////////////////////


	// Выдержка для BOOT_WAIT mode в секундах, при BOOT_WAIT=0 игнорируется.
	#define WAIT_VALUE 5

	////////////////////////////////////////////////


	// Выбор порта для кнопки входа в загрузчик. //

	#define INPORT		PORTB
	#define INLDDR		DDRB
	#define INLPIN		PINB
	#define INLPNUM		PINB1

	////////////////////////////////////////////////


	// Выбор порта для индикатора работы загрузчика.

	#define ENABLE_BOOT_LED		0
	#define OUTPORT			PORTB
	#define OUTDDR			DDRB
	#define OUTPNUM			PINB0

	////////////////////////////////////////////////



void protocol_init_usart(uint8_t boot_id);
