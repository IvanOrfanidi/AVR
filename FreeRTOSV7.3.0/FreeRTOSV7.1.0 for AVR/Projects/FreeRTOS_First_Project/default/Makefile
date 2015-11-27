###############################################################################
# Makefile for the project FreeRTOS_First_Project
###############################################################################

## General Flags
PROJECT = FreeRTOS_First_Project
MCU = atmega8
TARGET = FreeRTOS_First_Project.elf
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
LDFLAGS +=  -Wl,-Map=FreeRTOS_First_Project.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"D:\AVR\FreeRTOSV7.3.0\FreeRTOSV7.1.0 for AVR\Projects\FreeRTOS_First_Project\..\..\Source\include" -I"D:\AVR\FreeRTOSV7.3.0\FreeRTOSV7.1.0 for AVR\Projects\FreeRTOS_First_Project\..\..\Source\portable\GCC\ATMega323" -I"D:\AVR\FreeRTOSV7.3.0\FreeRTOSV7.1.0 for AVR\Projects\FreeRTOS_First_Project\." 

## Objects that must be built in order to link
OBJECTS = main.o list.o tasks.o heap_2.o port.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) FreeRTOS_First_Project.hex FreeRTOS_First_Project.eep FreeRTOS_First_Project.lss size

## Compile
main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

list.o: ../../../Source/list.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

tasks.o: ../../../Source/tasks.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

heap_2.o: ../../../Source/portable/MemMang/heap_2.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

port.o: ../../../Source/portable/GCC/ATMega323/port.c
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
	-rm -rf $(OBJECTS) FreeRTOS_First_Project.elf dep/* FreeRTOS_First_Project.hex FreeRTOS_First_Project.eep FreeRTOS_First_Project.lss FreeRTOS_First_Project.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

