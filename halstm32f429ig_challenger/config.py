from Arm.Stm32 import *

env = Stm32f429xx()
env.setLinkfile( '/ld/stm32f429ig_sdram.ld' )
env.appendDefineFlags( [ 'HSE_VALUE=25000000', 'NEED_FMC' ] )
env.appendDefineFlags( [ 'HAL_RNG=1' ] )
# NOTE:
# FCFS from 0x080E0000 ~ 0x080FFFFF contains 128Kbytes, this wastes a lot
env.appendDefineFlags( [ 'MCUSH_FCFS=1', 'FCFS_ADDR=0x080E0000', ] )



USE_VFS=False
USE_VFS=True

USE_SPIFFS=False
USE_SPIFFS=True

if USE_VFS:
    env.appendDefineFlags( [ 'MCUSH_VFS=1' ] )
    env.appendDefineFlags( [ 'MCUSH_ROMFS=1' ] )
    if USE_SPIFFS:
        env.appendDefineFlags( [ 'MCUSH_SPIFFS=1' ] )
        env.appendDefineFlags( [ 'SPIFLASH_CFG_PHYS_SZ=1024*1024*16' ] )
        env.appendPath([ '/libspiffs' ])
        env.appendGlobSource([ '/libspiffs/*.c' ])
    else:
        env.appendDefineFlags( [ 'MCUSH_SPIFFS=0' ] )



 


