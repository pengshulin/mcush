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
	env DEBUG=0 PORT=/dev/serial/by-id/usb-mcush_designed_by_pengshulin_mcush_virtual_port_066FFF545151714967046153-if00 /bin/sh -c "test -f prj2_dbg.elf && cd /home/trees/mcush/test && python -mTests gpio_set_high 2.1 || echo -n"
	sleep 1
	emu_util_openocd flash
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

