from Arm.Stm32 import *

env = Stm32f407xx()
env.setLinkfile( '/ld/stm32f407xg_min.ld' )
port = 'ARM_CM4F'
env.appendDefineFlags( [ 'HSE_VALUE=8000000' ] )
env.appendDefineFlags( [ 'HAL_RNG=1' ] )
env.appendDefineFlags( [ 'USE_CMD_UPGRADE=1' ] )
env.appendDefineFlags( [ 'HAL_REBOOT_COUNTER=1' ] )
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
        env.appendPath([ '/libspiffs' ])
        env.appendGlobSource([ '/libspiffs/*.c' ])
    else:
        env.appendDefineFlags( [ 'MCUSH_SPIFFS=0' ] )



 


