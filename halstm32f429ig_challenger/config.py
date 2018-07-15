from Arm.Stm32 import *

env = Stm32f429xx()
env.setLinkfile( '/ld/stm32f429ig_sdram.ld' )
env.appendDefineFlags( [ 'HSE_VALUE=25000000', 'NEED_FMC' ] )
env.appendDefineFlags( [ 'HAL_RNG=1' ] )

if hal_config.use_fcfs:
    # FCFS from 0x080E0000 ~ 0x080FFFFF contains 128Kbytes, this wastes a lot
    env.appendDefineFlags( [ 'FCFS_ADDR=0x080E0000', ] )

# additional hal path/sources
paths = []
sources = []

if hal_config.use_eth:
    env.appendDefineFlags( [ 'USE_ETH=1' ] )
    paths += ['LAN8742A']
    sources += ['LAN8742A/*.c']

