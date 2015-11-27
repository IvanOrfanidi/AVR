

#define SCK_FREQ 64
#define CLOCK_POLARITY 1
#define CLOCK_PHASE 1
#define DATA_ORDER 1
#define MASTER_INTERRUPT_ENABLE 0
#define SLAVE_INTERRUPT_ENABLE 0


#define SCK PB7
#define MISO PB6
#define MOSI PB5
#define SS PB4


	uint8_t byte;


void spi_master_init();
void spi_slave_init();
