<!--
  Title: AvrUsb500v3
  Description: An STK500v2-compatible AVR microcontroller programmer
  Author: clpalmer
  -->
AvrUsb500v3
===========
AvrUsb500v3 is an STK500v2-compatible AVR microcontroller programmer based on the AvrUsb500v2 project here:  
[tuxgraphics](http://tuxgraphics.org/electronics "tuxgraphics")


Hardware
--------

The hardware for this project has been modified from the original as follows:
 * The FT232RL chip was replaced by an MCP2200 from Microchip
 * The self-programming feature was removed (another programmer is required to program the ATMega88 so if you don't have access to one, you're better to use avrusb500v2 from tuxgraphics)
 * The target connector was replaced with a standard 6-pin IDC
 * Target voltage is read from target VCC pin, not reset pin
 * A 3.3V AM1117-3.3 regulator was added to accomodate 3.3V targets
 * An SN74AC125N buffer was added to level shift the MOSI, RST, SCK lines to accomodate 3.3V targets
 * The ATMega8 was replaced with the more recent ATMega88 and must be run at 18.432MHz
 * A micro-USB connector was used to reduce footprint
 * USB TX/RX LEDs were added
 * The board was laid out to fit a Hammond 1551S enclosure using some SMT components (SSOP, 1206)
 * A 3-way ON-OFF-ON selector switch was added to:
    * Power the target w/ 5V
    * Power the target w/ 3.3V
    * Use the voltage from the self-powered target to run the MOSI, RST, SCK lines

The schematics and layout can be viewed/edited using the freeware version of DipTrace here:  
[DipTrace Downloads](https://diptrace.com/download/download-diptrace/ "DipTrace Downloads")
	


Software
--------

The software remains largely unchanged, with pieces rewritten to accomodate hardware changes or to
my style while understanding the existing implementation.

A pre-compiled version (avrusb500v3.hex) is available that can be directly flashed to the ATMega88
to avoid installing the SDK and building the SW.


MCP2200 Configuration
---------------------

Configuring the MCP2200 can be done using the MCP2200 Configuration Utility available from [Microchip](http://www.microchip.com/wwwproducts/en/en546923 "Microchip") under Documentation->Software->MCP220 Configuration Utility

Steps:
  * Install and run the MCP2200 Configuration Utility available
  * Connect the avrusb500v3 to the PC using the USB cable
  * Ensure the configuration utility says connected in the status bar
  * Change Baud Rate to 115200
  * Click the checkbox to Enable Tx/Rx LEDs
  * Click the radio button to Toggle LEDs (or choose your preferred function)
  * Click Configure button
  * The Output window should show Device Configured and Device Verified


ATMega88 Configuration
----------------------

The ATMega88 must be programmed with the avrusb500v3.hex file and the fuses must be set as defined in
the Makefile. If you have access to AVRDude programming utility (easily available in linux):

  * Define your external programmer and port (DUDEHW, DUDEPORT) at the top of the Makefile
  * Connect the external programmer to the PROGRAMMER 6-pin connector
  * Verify the connectivity by reading the existing fuses:
```
	make rf
```
  * Load the firmware:
```
	make ld
```
  * Program the new fuses:
```
	make wf
```

At this point, your programmer should be functional and visible to your programming tool of choice
as an STK500v2 compatible programmer on the MCP2200's configured COM port.

If you don't have access to avrdude, you can use your programming SW of choice and use it to load
the avrusb500v3.hex file and program the high and low fuse byte values listed in the Makefile.

The fuses should be set to HIGH=0xDF, LOW=0xE6 as follows (output from AVR Studio):
```
	BOOTSZ = 1024W_0C00
	BOOTRST = [ ]
	RSTDISBL = [ ]
	DWEN = [ ]
	SPIEN = [X]
	WDTON = [ ]
	EESAVE = [ ]
	BODLEVEL = DISABLED
	CKDIV8 = [ ]
	CKOUT = [ ]
	SUT_CKSEL = EXTFSXTAL_1KCK_14CK_0MS

	EXTENDED = 0xF9 (valid)
	HIGH = 0xDF (valid)
	LOW = 0xE6 (valid)
```


AVRDude + avr-gcc on Ubuntu
---------------------------

AVRDude is available as a package in the standard debian distrubution. You can install it with:
```
	sudo apt install avrdude
```
Compiling the avrusb500v3.hex from from source (as well as other projects you create) will require
the standard build tools and avr-gcc:
```
	sudo apt install build-essential gcc-avr binutils-avr avr-libc
```
Once installed, you can build the avrusb500v3.hex file by running 'make' in the source folder.


Updating the SW Version via COM port
------------------------------------

This should not be required, as the SW version is pre-configured to 2.0a which should be fine for
AVR Studio.

If desired, it can be updated over a simple serial port connection:

  * Connect the programmer to the PC via USB
  * Open a serial terminal (using putty, gtkterm, anything else) to the MCP2200's COM port
  * Set the serial settings to 115200 Baud, 8-bits, 1 stop-bit, NO flow control
  * Hit return twice and you should see (in this example, it was changed to 2.b):
```
	avrusb500v2-1.5

	Enter SW Version Major in hex [2]: 2
	Enter SW Version Minor in hex [a]: b

	OK, my SW version is now: 2.0b (hex)
	Ready. Just close the terminal. No reset needed.
```

CLKOUT
------

The CLKOUT pin on the board provides a clock signal with about 1MHz which can be used to recover a
device which was accidently programmed to external clock or wrong crystal type.

Procedure:
  * Connect the clock output to XTAL1 pin on the target device
  * Program the target fuses with a very low SCK frequency (ie. avrdude option -B 20)

ie. To recover an ATMega8 use:
```
	avrdude -p m8 -c stk500v2 -P /dev/ttyUSB0 -b 115200 -u -v -U lfuse:w:0xe1:m -B 20
```
The low speed (set with -B 20) stays until you change it or unplug the usb connector.


History
-------

2018-03-20:  avrusb500v3-1.0 first public version based on avrusb500v2-1.5


Author/Copyright/LICENSE
------------------------

AvrUsb500v3 - Clancy Palmer  
Original AvrUsb500v2 (and all the hard work!) - Guido Socher - http://tuxgraphics.org  
Idea for terminal mode by Florin-Viorel Petrov.  
64K-Word flash addressing by Andrzej Wolski.

Copyright: GPLv2 (see LICENSE and http://www.gnu.org/licenses/gpl.html )
