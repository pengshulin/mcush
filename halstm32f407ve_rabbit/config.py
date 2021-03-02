from Arm.Stm32 import *

if hal_config.use_hal_driver is None:
    hal_config.use_hal_driver = True# False  # use std_driver as default
#    if hal_config.use_vcp is None:
#        hal_config.use_vcp = False  # use uart as default
if hal_config.use_vcp is None:
    if hal_config.use_hal_driver:
        hal_config.use_vcp = True  # use vcp as default

if (not hal_config.use_hal_driver) and hal_config.use_vcp:
    hal_config.use_vcp = False  # vcp does not support in std mode


env = Stm32f407xx( use_hal_driver=hal_config.use_hal_driver  )
env.setLinkfile( '/ld/stm32f407xe_ccmsram.ld' )
env.appendDefineFlags( [ 'HSE_VALUE=25000000' ] )
env.appendDefineFlags( [ 'HAL_RNG=1' ] )
env.appendDefineFlags( [ 'HAL_REBOOT_COUNTER=1' ] )
env.appendDefineFlags( [ 'HAL_TEST_CLK_OUTPUT=1' ] )  # output clock to lan8720 chip

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

if hal_config.use_spiffs:
    env.appendDefineFlags( [ 'SPIFLASH_AUTO_DETECT=1' ] )
    hal_config.paths += [hal_dir+'/spiffs']
    hal_config.sources += [hal_dir+'/spiffs/*.c']

if hal_config.use_eth:
    env.appendDefineFlags( [ 'HAL_ETH=1' ] )
    hal_config.paths += [hal_dir+'/LAN8742A']
    hal_config.sources += [hal_dir+'/LAN8742A/*.c']

if hal_config.use_can:
    env.appendDefineFlags( [ 'HAL_CAN=1' ] )


