define init

file DemoLua_dbg.elf
# STM32L152RB
#    # IROM 128k
#    mem 0x08000000 0x08020000 ro 32 cache
#    # IRAM 16k
#    mem 0x20000000 0x20004000 rw 32 nocache
#    mem 0x22000000 0x24000000 rw 32 nocache
#    # EEPROM 4k
#    mem 0x08080000 0x08080FFF rw 8 nocache
# STM32F103ZE
    # IROM 512k
    mem 0x08000000 0x08080000 ro 32 cache
    # IRAM 80k
    mem 0x20000000 0x20014000 rw 32 nocache
    mem 0x22000000 0x24000000 rw 32 nocache
    # FSMC/bank1
    mem 0x60000000 0x64000000 rw 16 nocache
    # FSMC/bank2
    mem 0x64000000 0x68000000 rw 16 nocache
    # FSMC/bank3
    mem 0x68000000 0x6C000000 rw 16 nocache
    # FSMC/bank4
    mem 0x6C000000 0x70000000 rw 16 nocache


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
#target remote localhost:3333
#target extended-remote localhost:4242
#set remote hardware-watchpoint-limit 6
#set remote hardware-breakpoint-limit 6
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


init
connect

set pagination off
#source .syslog.py
#source .utils.py

run2main


