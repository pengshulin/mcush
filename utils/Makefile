# MCUSH designed by Peng Shulin, all rights reserved.
.PHONY: clean flash flash2 reset reset2 stflash streset localclean

all:
	@scons
clean:
	@scons -c
localclean:
	rm -f *.elf *.lst *.bin *.hex *.map *.o


# openocd commands
erase:
	@emu_util_openocd erase
flash:
	@killall -q openocd && sleep 1 && env emu_util_openocd flash `scons --quiet name`.elf
reset:
	@emu_util_openocd reset


# st-flash commands
flash2:
	@emu_util_stlink flash `scons --quiet name`.bin
stflash:
	@emu_util_stlink flash `scons --quiet name`.bin
reset2:
	@emu_util_stlink reset
streset:
	@emu_util_stlink reset

