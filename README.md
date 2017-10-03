DESCRIPTION
===========
* MCUSH is the short name for MCU-SHell.
* A practical project that enhance microcontroller serial port command line interface with shell features like in many linux systems.
* The goal of the project is to provide a firmware template that will expand commands for new applications, so it should be designed with simple API and easy to use.
* MCUSH is heavily based on FreeRTOS, the shell task runs as a stand-alone task with middle level priority.
* Test suites and templates written in python are privided to ease the test work and create CLI/GUI applications.
* Hosted on github: <https://github.com/pengshulin/mcush>


FEATURES
========
* SUPPORT:
  * call command functions with arguments in form of argc and argv
  * arguments parser with short option '-' and long option '--'
  * low memory requirement, suitable for some tiny chips
  * prompt hook function for customize
  * history command list 
  * multi-line input
  * may port to different architectures, but currently it runs on CORTEX-M only
* NOT SUPPORT:
  * lineedit features require multi-bytes (such as array keys, shell colors...) not supported, only support simple features that require single-byte (such as BACKSPACE, Ctrl-A/B/C/D/E/F/K/N/P)


ROADMAP
=======
* run the basic shell on STM32 chips
* add commands to control more peripherals
* use USB virtual com port driver provided by ST
* test embedded dynamic languages (eg. lua/picolisp/tinyscheme...)
* add file system support (FatFS/spiffs...)
* add file utils (ls,cp,rm,cat,...) for file system
* add bootloader and upgrade support


ENVIRONMENT
===========
* arm-none-eabi-gcc, newlibc, arm-none-eabi-gdb
* scons + personal MCU build scripts
  <https://github.com/pengshulin/site_scons>
* STLink v2 + st-flash(flasher) + st-util(gdb server)
  <https://github.com/texane/stlink>
* openocd
* develope and test on Debian/Ubuntu machines


LICENSE
=======
* source codes:
  * As this is my personal software toolkit, you can use it freely only for non-commerical use.
  * Commerical use is NOT AUTHORIZED!!!
* binary executables:
  * images provided in build directory are free as BSD


AUTHOR
======
Peng Shulin <trees_peng@163.com>
Shanghai, China 2017
 
