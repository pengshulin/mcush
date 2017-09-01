define init 
file Virtual_COM_Port_dbg.elf
# IROM 128k
mem 0x08000000 0x08020000 ro 32 cache
# IRAM 20k
mem 0x20000000 0x20005000 rw 32 nocache
# PERIPHERALS
mem 0x40000000 0x60000000 rw 32 nocache
# SYSTEM MEMORY
mem 0x1fff0000 0x1fff7a10 ro 32 nocache
# UID 
mem 0x1fff7a10 0x1fff7a1c ro 32 cache
# OPTION BYTES
mem 0x1fffc000 0x1fffc008 ro 32 nocache
# CORTEX-M3 PERIPHERALS
mem 0xe0000000 0x100000000 rw 32 nocache
end

define connect
shell killall -q st-util
shell sleep 0.5 
target remote localhost:4242
#target extended-remote localhost:4242
set remote hardware-watchpoint-limit 6
set remote hardware-breakpoint-limit 6
monitor reset
end

define reset
monitor reset
end

define run2main
tbreak main 
continue
end

define reset2main
reset
run2main

define loopstep
set verbose off
set confirm off
while (1)
step
end
end

set pagination off

init
connect

#source .syslog.py

run2main

