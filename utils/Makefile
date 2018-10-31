# MCUSH designed by Peng Shulin, all rights reserved.
.PHONY: clean flash flash2 reset reset2 stflash streset

all:
	scons
clean:
	scons -c

# openocd commands
erase:
	emu_util_openocd erase
flash:
	killall -q openocd
	sleep 1
	env emu_util_openocd flash
	#emu_util_openocd reset
reset:
	emu_util_openocd reset


# st-flash commands
flash2:
	emu_util_stlink flash
stflash:
	emu_util_stlink flash
reset2:
	emu_util_stlink reset
streset:
	emu_util_stlink reset

