# arm-none-eabi-gdb debug utility
# designed by PengShulin <trees_peng@163.com>

python 
import os, os.path
_bool_true_list = ['1', 'Y', 'y', 'T', 't', 'yes', 'Yes', 'YES', 'true', 'True', 'TRUE']
def getenv_bool( key, default=None ):
    ret = os.getenv(key, default)
    return False if ret is None else bool(ret in _bool_true_list)
DEBUG=getenv_bool("DEBUG")
CHIP=os.getenv("CHIP")
chip = CHIP
if chip is None:
    raise Exception('CHIP not assigned')
else:
    print('set chip as %s'% chip)

# use scons assigned elf file
from subprocess import check_output
dbg_file = check_output(['scons','--quiet','name']).strip()+'.elf'
if os.path.isfile( dbg_file ):
    print('found debug file %s'% dbg_file)
else:
    dbg_file=None
    print('not debug file found.')
    
class dump_array(gdb.Command):
    fname = '/dev/shm/float_array'
    def __init__ (self):
        super (dump_array, self).__init__ ("dump_array", gdb.COMMAND_USER)

    def invoke (self, arg, from_tty):
        args = arg.split()
        try:
            var = args[0]
        except:
            print('error varname parms')
            return
        try:
            size = int(args[1])
        except IndexError:
            size = None  # not assigned
        except:
            print('error size parms')
            return
        f = open(self.fname, 'w+')
        print('dump %s...'% var)
        v = gdb.parse_and_eval(var)
        #print(v) 
        if size is None:
            # now check the actual size
            _t, _s = str(v.type).split()
            assert _t == 'float'
            size = int(_s[1:-1])
            assert size > 0
            #print('size=%d'% size)
        for i in range(size):
            f.write( '%s\n'% v[i] )
        f.close()
dump_array()


class sptk_watch(gdb.Command):
    def __init__ (self):
        super (sptk_watch, self).__init__ ("sptk_watch", gdb.COMMAND_USER)

    def invoke (self, arg, from_tty):
        args = arg.split()
        if len(args) < 2:
            print('error parms')
            return 
        var = args[0]
        location = ' '.join(args[1:])
        print(var, '@', location)
        existing=False
        bps = gdb.breakpoints()
        if bps:
            for b in bps:
                b.enabled = False
                if b.location == location:
                    existing = True
                    bp = b
        if existing:
            bp.enabled = True
        else:
            gdb.Breakpoint(location) 
        while True:
            gdb.execute( 'c' )
            gdb.execute( 'dump_array %s'% var )
            open('/dev/shm/sptk_dsp.fifo','a').write('update\n')

sptk_watch()

end

######################################################################

define init_common_regs
mem 0xE0000000 0xE000FFFC rw 32 nocache
end

define init_chip_regs
python
chip_mem_configs = {

# Start addr  End addr    RW   BITS CACHED
'stm32f030': [
[ 0x08000000, 0x0803FFFC, 'ro', 32, True,  'Flash 256k' ],
[ 0x20000000, 0x20007FFC, 'rw', 32, False, 'SRAM 32k' ],
[ 0x40000000, 0x4007FFFC, 'rw', 32, False, 'Peripherals', ],
],

'stm32l152': [
[ 0x08000000, 0x0801FFFC, 'ro', 32, True,  'Flash 128k' ],
[ 0x20000000, 0x20003FFC, 'rw', 32, False, 'SRAM 16k' ],
[ 0x22000000, 0x23FFFFFC, 'rw', 32, False, '' ],
[ 0x40000000, 0x4007FFFC, 'rw', 32, False, 'Peripherals', ],
[ 0x08080000, 0x08080FFF, 'rw', 8,  False, 'EEPROM 4k' ],
],

'stm32f103': [
[ 0x08000000, 0x0807FFFC, 'ro', 32, True,  'Flash 512k' ],
[ 0x20000000, 0x20013FFC, 'rw', 32, False, 'SRAM 80k' ],
[ 0x22000000, 0x23FFFFFC, 'rw', 32, False, '', ],
[ 0x40000000, 0x4007FFFC, 'rw', 32, False, 'Peripherals', ],
[ 0x1FFF0000, 0x1FFF7A10, 'ro', 32, False, 'System Memory', ],
[ 0x1FFF7A10, 0x1FFF7A1C, 'ro', 32, True,  'UID', ],
[ 0x1FFFC000, 0x1FFFC008, 'ro', 32, False, 'Option Bytes', ],
[ 0xE0000000, 0xFFFFFFFC, 'rw', 32, False, 'Cortex-M3 Peripherals', ],
[ 0x60000000, 0x63FFFFFE, 'rw', 16, False, 'FSMC/bank1' ],
[ 0x64000000, 0x67FFFFFE, 'rw', 16, False, 'FSMC/bank2' ],
[ 0x68000000, 0x6BFFFFFE, 'rw', 16, False, 'FSMC/bank3' ],
[ 0x6C000000, 0x6FFFFFFE, 'rw', 16, False, 'FSMC/bank4' ],
],

'stm32f207': [
[ 0x08000000, 0x081FFFFC, 'ro', 32, True,  'Flash 2M' ],
[ 0x20000000, 0x2001FFFC, 'rw', 32, False, 'SRAM 128k' ],
[ 0x40000000, 0x4007FFFC, 'rw', 32, False, 'Peripherals', ],
],

'stm32f407': [
[ 0x08000000, 0x081FFFFC, 'ro', 32, True,  'Flash 2M', ],
[ 0x20000000, 0x2001FFFC, 'rw', 32, False, 'SRAM 128k', ],
[ 0x10000000, 0x1000FFFC, 'rw', 32, False, 'CCM SRAM 64k', ],
[ 0x40000000, 0x4007FFFC, 'rw', 32, False, 'Peripherals', ],
[ 0x60000000, 0x63FFFFFE, 'rw', 16, False, 'FSMC/bank1', ],
[ 0x64000000, 0x67FFFFFE, 'rw', 16, False, 'FSMC/bank2', ],
[ 0x68000000, 0x6BFFFFFE, 'rw', 16, False, 'FSMC/bank3', ],
[ 0x6C000000, 0x6FFFFFFE, 'rw', 16, False, 'FSMC/bank4', ],
],

'stm32f412': [
[ 0x08000000, 0x08100000, 'ro', 32, True,  'Flash 1M', ],
[ 0x20000000, 0x20040000, 'rw', 32, False, 'SRAM 256k', ],
[ 0x40000000, 0x40080000, 'rw', 32, False, 'Peripherals', ],
],

'stm32f429': [
[ 0x08000000, 0x081FFFFC, 'ro', 32, True,  'Flash 2M', ],
[ 0x20000000, 0x2002FFFC, 'rw', 32, False, 'SRAM 192k', ],
[ 0x10000000, 0x1000FFFC, 'rw', 32, False, 'CCM SRAM 64k', ],
[ 0x40000000, 0x4007FFFC, 'rw', 32, False, 'Peripherals', ],
[ 0xC0000000, 0xCFFFFFFE, 'rw', 16, False, 'FMS/Bank5 SDRAM 256M', ],
[ 0xD0000000, 0xDFFFFFFE, 'rw', 16, False, 'FMS/Bank6 SDRAM 256M', ],
[ 0xA0000000, 0xA0000FFE, 'rw', 16, False, 'FMC control register', ],
],

'stm32f767': [
[ 0x08000000, 0x081FFFFC, 'ro', 32, True,  'Flash 2M', ],
[ 0x20000000, 0x20080000, 'rw', 32, False, 'SRAM 512k', ],
[ 0x40000000, 0x4007FFFC, 'rw', 32, False, 'Peripherals', ],
],

'lpc4337': [
[ 0x1A000000, 0x1A07FFFC, 'ro', 32, True,   'FLASH A', ],
[ 0x1B000000, 0x1B07FFFC, 'ro', 32, True,   'FLASH B', ],
[ 0x20040000, 0x20043FFC, 'ro', 32, True,   'EEPROM', ],
[ 0x10000000, 0x10007FFC, 'rw', 32, False,  'SRAM', ],
[ 0x10080000, 0x10089FFC, 'rw', 32, False,  '', ],
[ 0x20000000, 0x2000FFFC, 'rw', 32, False,  '', ],
[ 0x28000000, 0x2FFFFFFC, 'rw', 32, False,  'SDRAM', ],
[ 0x10400000, 0x1040FFFC, 'ro', 32, True,   'ROM', ],
[ 0x40000000, 0x44FFFFFC, 'rw', 32, False,  'Peripherals', ],
],

'mm32f0': [
[ 0x08000000, 0x0801FFFC, 'ro', 32, True,  'Flash 128k' ],
[ 0x20000000, 0x20001FFC, 'rw', 32, False, 'SRAM 8k' ],
[ 0x40000000, 0x4007FFFC, 'rw', 32, False, 'Peripherals', ],
],

'mm32f1': [
[ 0x08000000, 0x0801FFFC, 'ro', 32, True,  'Flash 128k' ],
[ 0x20000000, 0x20004FFC, 'rw', 32, False, 'SRAM 20k' ],
[ 0x40000000, 0x4007FFFC, 'rw', 32, False, 'Peripherals', ],
],

'lpc84x': [
[ 0x00000000, 0x0000FFFC, 'ro', 32, True,  'Flash 64k' ],
[ 0x0F000000, 0x0F003FFC, 'ro', 32, True, 'ROM 16k' ],
[ 0x10000000, 0x10003FFC, 'rw', 32, False, 'SRAM 16k' ],
[ 0x40000000, 0x4007FFFC, 'rw', 32, False, 'Peripherals', ],
]


}

if chip in chip_mem_configs.keys():
    for saddr, eaddr, mode, bits, cached, descr in chip_mem_configs[chip]:
        cmd = 'mem 0x%08X 0x%08X %s %d %s'% (saddr, eaddr, mode, bits, 'cache' if cached else 'nocache')
        #print cmd
        gdb.execute(cmd)
else:
    print('warning: no chip memory config found')
end
end

define update_sptk_dsp
echo update sptk_dsp_app
python open('/dev/shm/sptk_dsp.fifo','a').write('update\n')
end

define connect
set remotetimeout 5000
shell killall -q st-util 
shell killall -q openocd
shell sleep 1
# gdbserver port:
#   4242 - st-util
#   3333 - openocd
#target remote localhost:4242  
target remote localhost:3333
#monitor reset halt
#monitor halt
end


define load_file
python
if dbg_file:
    print("load file %s..."% dbg_file)
    gdb.execute('file %s'% dbg_file)
end
end


define program
python 
if dbg_file:
    f = os.path.abspath(dbg_file)
    print("program %s..."% f)
    gdb.execute('program %s'% f)
    print("program done")
end
end

define halt
monitor halt
end

define reset
monitor halt
monitor reset init
monitor reset halt
end

define run2main
tbreak main 
continue
end

define loopstep
set verbose off
set confirm off
while (1)
    step
end
end


alias sw = sptk_watch

source .gdbextra

######################################################################
#set pagination off

init_chip_regs
init_common_regs
load_file
connect
#program
#reset
#run2main

