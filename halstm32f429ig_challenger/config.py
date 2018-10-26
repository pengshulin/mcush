from Arm.Stm32 import *

env = Stm32f429xx( use_hal_driver=hal_config.use_hal_driver )
env.setLinkfile( '/ld/stm32f429ig_sdram.ld' )
env.appendDefineFlags( [ 'HSE_VALUE=25000000', 'NEED_FMC' ] )
env.appendDefineFlags( [ 'HAL_RNG=1' ] )

hal_config.paths += ['common']
hal_config.sources += ['common/*.c']
hal_dir = 'hal' if hal_config.use_hal_driver else 'std'
hal_config.paths += [hal_dir]
hal_config.sources += [hal_dir+'/*.c']

if hal_config.use_vcp:
    hal_config.paths += [hal_dir+'/vcp']
    hal_config.sources += [hal_dir+'/vcp/*.c']
    env.appendDriver(STM32_USB_DEVICE_CDC_Driver())
    #env.appendDefineFlags( [ 'SUSPEND_ENABLED=0' ] )
    #env.appendDefineFlags( [ 'USE_ST_VID_PID=1' ] )
else:
    hal_config.paths += [hal_dir+'/uart']
    hal_config.sources += [hal_dir+'/uart/*.c']


if hal_config.use_spiffs:
    env.appendDefineFlags( [ 'SPIFLASH_AUTO_DETECT=1' ] )
    hal_config.paths += [hal_dir+'/spiffs']
    hal_config.sources += [hal_dir+'/spiffs/*.c']


if hal_config.use_fcfs:
    # FCFS from 0x080E0000 ~ 0x080FFFFF contains 128Kbytes, this wastes a lot
    env.appendDefineFlags( [ 'FCFS_ADDR=0x080E0000', ] )

if hal_config.use_eth:
    env.appendDefineFlags( [ 'USE_ETH=1' ] )
    hal_config.paths += [hal_dir+'/LAN8742A']
    hal_config.sources += [hal_dir+'/LAN8742A/*.c']

