<https://github.com/jensniels/ds18b20>

ds18b20
=======

Quite portable C driver for the ds18b20 temperature sensor, including a function that actually calculates the crc (in case you have a system where you can't fit a 256 byte crc table)

Note: There's no parasite power support

HOWTO: Everything in the 'common' directory should be easily portable, but it requires the owhal.h file for target and compiler specific stuff. owhal.h contains macros for reading and setting the onewire pin and also a function for microsecond delays. Obviously you also need a project that does something useful with the output, and to do so you probably also want to rewrite parts of ds18b20.c to suit your needs.

Included are example projects for the STM32F4-DISCOVERY Cortex M4 board and the Microchip PICkit 3 PIC18 44-pin demo board. Neither really does anything but they show how it works.

Legal stuff:
* Anything done by me:
   BSD 3-clause license ("New BSD License" or "Modified BSD License")

* Anything done by anybody else:
   As indicated in respective file
