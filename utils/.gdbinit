python 
chip='stm32l152rb'
chip='stm32f407zg'
chip='stm32f103rb'
chip='stm32f103ze'
chip='lpc4337'

import glob
import os, os.path
try:
    dbg_file=glob.glob('*_dbg.elf')[0]
    print "found debug file %s..."% dbg_file
except: 
    dbg_file=None
    print "not debug file found."
end

define init_regs
python
chip_mem_configs = {

'stm32l152rb': [
[ 0x08000000, 0x08020000, 'ro', 32, True,  'IROM 128k' ],
[ 0x20000000, 0x20004000, 'rw', 32, False, 'IRAM 16k' ],
[ 0x22000000, 0x24000000, 'rw', 32, False, '' ],
[ 0x08080000, 0x08080FFF, 'rw', 8,  False, 'EEPROM 4k' ],
],

'stm32f103rb': [
[ 0x08000000, 0x08020000, 'ro', 32, True,  'FLASH 128k' ],
[ 0x20000000, 0x20005000, 'rw', 32, False, 'SRAM 20k' ],
],

'stm32f103ze': [
[ 0x08000000, 0x08080000, 'ro', 32, True,  'IROM 512k' ],
[ 0x20000000, 0x20014000, 'rw', 32, False, 'IRAM 80k' ],
[ 0x22000000, 0x24000000, 'rw', 32, False, '', ],
[ 0x60000000, 0x64000000, 'rw', 16, False, 'FSMC/bank1' ],
[ 0x64000000, 0x68000000, 'rw', 16, False, 'FSMC/bank2' ],
[ 0x68000000, 0x6C000000, 'rw', 16, False, 'FSMC/bank3' ],
[ 0x6C000000, 0x70000000, 'rw', 16, False, 'FSMC/bank4' ],
[ 0x40000000, 0x60000000, 'rw', 32, False, 'PERIPHERALS', ],
[ 0x1fff0000, 0x1fff7a10, 'ro', 32, False, 'SYSTEM MEMORY', ],
[ 0x1fff7a10, 0x1fff7a1c, 'ro', 32, True,  'UID', ],
[ 0x1fffc000, 0x1fffc008, 'ro', 32, False, 'OPTION BYTES', ],
[ 0xe0000000, 0x100000000,'rw', 32, False, 'CORTEX-M3 PERIPHERALS', ],
],

'stm32f407zg': [
[ 0x00000000, 0x00100000, 'ro', 32, True,  'IROM 1M', ],
[ 0x08000000, 0x08100000, 'ro', 32, True,  '', ],
[ 0x20000000, 0x20020000, 'rw', 32, False, 'IRAM 128k', ],
[ 0x10000000, 0x10010000, 'rw', 32, False, 'CCM data RAM 64k', ],
[ 0x60000000, 0x64000000, 'rw', 16, False, 'FSMC/bank1', ],
[ 0x64000000, 0x68000000, 'rw', 16, False, 'FSMC/bank2', ],
[ 0x68000000, 0x6C000000, 'rw', 16, False, 'FSMC/bank3', ],
[ 0x6C000000, 0x70000000, 'rw', 16, False, 'FSMC/bank4', ],
],

'lpc4337': [
[ 0x1A000000, 0x1A080000, 'ro', 32, True,   'FLASH A', ],
[ 0x1B000000, 0x1B080000, 'ro', 32, True,   'FLASH B', ],
[ 0x10000000, 0x10008000, 'rw', 32, False,  'SRAM', ],
[ 0x10080000, 0x1008A000, 'rw', 32, False,  '', ],
[ 0x20000000, 0x20010000, 'rw', 32, False,  '', ],
[ 0x28000000, 0x30000000, 'rw', 32, False,  'SDRAM', ],
[ 0x10400000, 0x10410000, 'ro', 32, True,   'ROM', ],
],

}

if chip in chip_mem_configs.keys():
    for saddr, eaddr, mode, bits, cached, descr in chip_mem_configs[chip]:
        cmd = 'mem 0x%08X 0x%08X %s %d %s'% (saddr, eaddr, mode, bits, 'cache' if cached else 'nocache')
        #print cmd
        gdb.execute(cmd)
        
else:
    print 'warning: no chip memory config found'
end
end


define connect
shell killall -q st-util 
#shell killall -q openocd
shell sleep 0.5 
# gdbserver port:
#   4242 - st-util
#   3333 - openocd
#target remote localhost:4242  
target remote localhost:3333
monitor reset halt
monitor halt
end


define load_file
python
if dbg_file:
    print "load file %s..."% dbg_file
    gdb.execute('file %s'% dbg_file)
end
end


define program
python 
if dbg_file:
    f = os.path.abspath(dbg_file)
    print "erase..."
    gdb.execute('monitor flash erase_sector 0 0 last')
    print "erase done"
    print "program %s..."% f
    gdb.execute('monitor flash write_image erase %s'% f)
    print "program done"
end
end

define halt
monitor halt
end

define reset
monitor halt
monitor reset halt
monitor halt
end

define run2main
monitor reset halt
monitor halt
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


init_regs
load_file
connect
#program

set pagination off
#source .syslog.py
#source .utils.py

#run2main


