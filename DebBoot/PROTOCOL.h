
		#include <avr/io.h>

		#include <util/crc16.h>


	// ��������� ��������� ���������� � ������ ���������.

	#define CMD_READ	0x03

	#define CMD_READ	0x16

	////////////////////////////////////////////////


	// ��������� ����� ����������.

	#define BAUD_RATE		19200

	#define SELECT_USART	0	//!!! ����� ������ USART.

	#define USART_DELAY_MS 0

	#define ENABLE_RE_DE	1
	#define RE_DE 		PINB1
	#define PORT_RE_DE 	PORTB
	#define DDR_RE_DE	DDRB

	////////////////////////////////////////////////


	// ����� ������ ����������. !!! ��� BOOT_SIMPLE, ��� BOOT_WAIT.

	#define BOOT_SIMPLE		0	// ���� �� ������.
	#define BOOT_WAIT		1	// ���� �� ��������.

	////////////////////////////////////////////////


	// �������� ��� BOOT_WAIT mode � ��������, ��� BOOT_WAIT=0 ������������.
	#define WAIT_VALUE 5

	////////////////////////////////////////////////


	// ����� ����� ��� ������ ����� � ���������. //

	#define INPORT		PORTB
	#define INLDDR		DDRB
	#define INLPIN		PINB
	#define INLPNUM		PINB1

	////////////////////////////////////////////////


	// ����� ����� ��� ���������� ������ ����������.

	#define ENABLE_BOOT_LED		0
	#define OUTPORT			PORTB
	#define OUTDDR			DDRB
	#define OUTPNUM			PINB0

	////////////////////////////////////////////////



void protocol_init_usart(uint8_t boot_id);
