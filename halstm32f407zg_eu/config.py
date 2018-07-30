from Arm.Stm32 import *
env = Stm32f407xx()
env.setLinkfile( '/ld/stm32f407xg_min.ld' )
env.appendDefineFlags( [ 'HAL_RNG=1' ] )
env.appendDefineFlags( [ 'USE_CMD_UPGRADE=1' ] )
env.appendDefineFlags( [ 'HAL_REBOOT_COUNTER=1' ] )

if hal_config.use_fcfs:
    # FCFS from 0x080E0000 ~ 0x080FFFFF contains 128Kbytes, this wastes a lot
    env.appendDefineFlags( [ 'FCFS_ADDR=0x080E0000', ] )
    # FCFS from 0x1FFF7800 ~ 0x1FFF79FF contains 512bytes (OTP area)
    #env.appendDefineFlags( [ 'FCFS_ADDR=0x1FFF7800', ] )

if hal_config.use_eth:
    env.appendDefineFlags( [ 'USE_ETH=1' ] )
    hal_config.paths += ['LAN8742A']
    hal_config.sources += ['LAN8742A/*.c']

if hal_config.use_emwin:
    env.appendDefineFlags( [ 'NEED_FSMC=1' ] )
    hal_config.paths += ['emwin_driver']
    hal_config.sources += ['emwin_driver/*.c']

