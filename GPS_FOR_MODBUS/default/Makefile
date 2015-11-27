###############################################################################
# Makefile for the project GPS_FOR_MODBUS
###############################################################################

## General Flags
PROJECT = GPS_FOR_MODBUS
MCU = atmega162
TARGET = GPS_FOR_MODBUS.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99   -DF_CPU=8000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=GPS_FOR_MODBUS.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = main.o GPS.o USART.o MODBUS_SLAVE.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) GPS_FOR_MODBUS.hex GPS_FOR_MODBUS.eep GPS_FOR_MODBUS.lss size

## Compile
main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

GPS.o: ../GPS.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

USART.o: ../USART.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

MODBUS_SLAVE.o: ../MODBUS_SLAVE.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) GPS_FOR_MODBUS.elf dep/* GPS_FOR_MODBUS.hex GPS_FOR_MODBUS.eep GPS_FOR_MODBUS.lss GPS_FOR_MODBUS.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

