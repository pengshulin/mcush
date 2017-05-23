.PHONY: clean flash flash2 reset reset2

all:
	scons

clean:
	scons -c

flash:
	emu_util_openocd flash
	#emu_util_openocd reset

flash2:
	emu_util_stlink flash

reset:
	emu_util_openocd reset

reset2:
	emu_util_stlink reset

