# MCUSH designed by Peng Shulin, all rights reserved.
.PHONY: clean localclean erase flash reset sterase stflash streset

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
sterase:
	@emu_util_stlink erase
stflash:
	@emu_util_stlink flash `scons --quiet name`.hex
streset:
	@emu_util_stlink reset


