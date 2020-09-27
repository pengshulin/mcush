from Arm.Stm32 import *

if hal_config.use_hal_driver is None:
    hal_config.use_hal_driver = True  # use hal_driver as default
    if hal_config.use_vcp is None:
        hal_config.use_vcp = True  # use vcp as default

env = Stm32f407xx( use_hal_driver=hal_config.use_hal_driver )
env.setLinkfile( '/ld/stm32f407xg.ld' )
env.appendDefineFlags( [ 'HSE_VALUE=8000000' ] )
env.appendDefineFlags( [ 'HAL_RNG=1' ] )
#env.appendDefineFlags( [ 'USE_CMD_UPGRADE=1' ] )
env.appendDefineFlags( [ 'HAL_REBOOT_COUNTER=1' ] )
#env.appendDefineFlags( [ 'HAL_WDG_ENABLE=1' ] )

hal_config.paths += ['common']
hal_config.sources += ['common/*.c']
hal_dir = 'hal' if hal_config.use_hal_driver else 'std'
hal_config.paths += [hal_dir]
hal_config.sources += [hal_dir+'/*.c']

if hal_config.use_vcp:
    hal_config.paths += [hal_dir+'/vcp']
    hal_config.sources += [hal_dir+'/vcp/*.c']
    env.appendDriver(STM32_USB_DEVICE_CDC_Driver())
else:
    hal_config.paths += [hal_dir+'/uart']
    hal_config.sources += [hal_dir+'/uart/*.c']

if hal_config.use_fcfs:
    # FCFS from 0x080E0000 ~ 0x080FFFFF contains 128Kbytes, this wastes a lot
    env.appendDefineFlags( [ 'FCFS_ADDR=0x080E0000', ] )
    # FCFS from 0x1FFF7800 ~ 0x1FFF79FF contains 512bytes (OTP area)
    #env.appendDefineFlags( [ 'FCFS_ADDR=0x1FFF7800', ] )

if hal_config.use_spiffs:
    env.appendDefineFlags( [ 'SPIFLASH_AUTO_DETECT=1' ] )
    hal_config.paths += [hal_dir+'/spiffs']
    hal_config.sources += [hal_dir+'/spiffs/*.c']

if hal_config.use_eth:
    env.appendDefineFlags( [ 'USE_ETH=1' ] )
    hal_config.paths += [hal_dir+'/LAN8742A']
    hal_config.sources += [hal_dir+'/LAN8742A/*.c']

if hal_config.use_emwin:
    env.appendDefineFlags( [ 'NEED_FSMC=1' ] )
    hal_config.paths += [hal_dir+'emwin_driver']
    hal_config.sources += [hal_dir+'emwin_driver/*.c']

if hal_config.use_hal_driver:
    # disable those functions that are not finished
    env.appendDefineFlags( [
        'USE_CMD_SCPI_RST=0',
        'USE_CMD_PWM=0',
        'USE_CMD_ADC=0',
        'USE_CMD_SGPIO=0',
        'USE_CMD_UPGRADE=0',
        ] )

