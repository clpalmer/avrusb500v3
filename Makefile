#-------------------
# Compilation flags
CFLAGS=-g -DF_CPU=18432000UL -mmcu=atmega88 -Wall -Wstrict-prototypes -Os -mcall-prologues
#-------------------
# avrdude settings for programming the programmer
DUDEHW=dragon_isp
DUDEPORT=/dev/ttyACM0
DUDEBAUD=115200
DUDECMD=avrdude -p m88 -c $(DUDEHW) -P $(DUDEPORT) -b $(DUDEBAUD)
#-------------------
# ATMega88 fuses
# High(0xDF) - Default
# Low(0xE6) - Disable CLKDIV, external full-swing crystal
HIGHFUSE=0xdf
LOWFUSE=0xe6
#-------------------
.PHONY: all help ld wf rf
#-------------------
all: avrusb500v3.hex
#-------------------
help: 
	@echo "Print this help"
	@echo "  make help"
	@echo ""
	@echo "Compile all and show size of avrusb500v3.hex"
	@echo "  make all|show"
	@echo ""
	@echo "Load programmer software, write fuses and read fuses with external programmer"
	@echo "  make ld|wf|rf"
	@echo ""
	@echo "Delete all generated files"
	@echo "  make clean"
#-------------------
# main
show: main.out
	avr-objdump -d main.out
avrusb500v3.hex : main.out 
	avr-objcopy -R .eeprom -O ihex main.out avrusb500v3.hex 
	avr-size main.out
	@echo " "
	@echo "Expl.: data=initialized data, bss=uninitialized data, text=code"
	@echo " "
main.out : main.o uart.o spi.o timeout.o analog.o
	avr-gcc $(CFLAGS) -o main.out -Wl,-Map,main.map main.o uart.o spi.o timeout.o analog.o
main.o : main.c command.h spi.h uart.h timeout.h analog.h
	avr-gcc $(CFLAGS) -Os -c main.c
#-------------------
# timeout
timeout.o : timeout.c timeout.h
	avr-gcc $(CFLAGS) -Os -c timeout.c
#-------------------
# Analog
analog.o : analog.c analog.h
	avr-gcc $(CFLAGS) -Os -c analog.c
#-------------------
# SPI
spi.o : spi.c spi.h timeout.h
	avr-gcc $(CFLAGS) -Os -c spi.c
#-------------------
# UART
uart.o : uart.c uart.h timeout.h analog.h
	avr-gcc $(CFLAGS) -Os -c uart.c
#-------------------
# Load firmware with external programmer
ld: main.hex
	$(DUDECMD) -e -U flash:w:main.hex
#-------------------
# Set fuses with external programmer
wf: 
	echo "Set HIGH fuse to $(HIGHFUSE) and LOW fuse to $(LOWFUSE)..."
	$(DUDECMD) -u -v -U hfuse:w:$(HIGHFUSE):m -U lfuse:w:$(LOWFUSE):m
#-------------------
# Read fuses with external programmer
rf: 
	echo "HIGH fuse should be $(HIGHFUSE) and LOW fuse should be $(LOWFUSE)..."
	$(DUDECMD) -v -q
#-------------------
clean:
	rm -f *.o *.map *.out avrusb500v3.hex
#-------------------
