from Arm.Stm32 import *

env = Stm32f429xx()
env.setLinkfile( '/ld/stm32f429ig_sdram.ld' )
env.appendDefineFlags( [ 'HSE_VALUE=25000000', 'NEED_FMC' ] )
env.appendDefineFlags( [ 'HAL_RNG=1' ] )

if hal_config.use_spiffs:
    env.appendDefineFlags( [ 'SPIFLASH_AUTO_DETECT=1' ] )

if hal_config.use_fcfs:
    # FCFS from 0x080E0000 ~ 0x080FFFFF contains 128Kbytes, this wastes a lot
    env.appendDefineFlags( [ 'FCFS_ADDR=0x080E0000', ] )

if hal_config.use_eth:
    env.appendDefineFlags( [ 'USE_ETH=1' ] )
    hal_config.paths += ['LAN8742A']
    hal_config.sources += ['LAN8742A/*.c']

