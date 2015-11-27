


	#define UPE 2
	#define DOR 3
	#define FE 4


	#define FRAMING_ERROR (1<<FE)
	#define PARITY_ERROR (1<<UPE)
	#define DATA_OVERRUN (1<<DOR)

	#define TIMEOUT_REQ 10000

	#define message_size_buffer 256

	char gps_time[6];
	char time[3];

	char gps_latitude[11];
	char gps_longitude[11];
		
	void gps_init();

	int gps_stat();

	int gps_status();


	char gps_message[message_size_buffer];

