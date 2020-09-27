from Arm.Stm32 import *
env = Stm32f407xx( use_hal_driver=hal_config.use_hal_driver  )
#env.setLinkfile( '/ld/stm32f407xe.ld' )
env.setLinkfile( '/ld/stm32f407xe_ccmsram.ld' )
env.appendDefineFlags( [ 'HSE_VALUE=8000000' ] )
env.appendDefineFlags( [ 'HAL_RNG=1' ] )
env.appendDefineFlags( [ 'HAL_REBOOT_COUNTER=1' ] )

hal_config.paths += ['common']
hal_config.sources += ['common/*.c']
hal_dir = 'hal' if hal_config.use_hal_driver else 'std'
hal_config.paths += [hal_dir]
hal_config.sources += [hal_dir+'/*.c']

if hal_config.use_vcp is None:
    hal_config.use_vcp = True  # use vcp as default

if hal_config.use_vcp:
    hal_config.paths += [hal_dir+'/vcp']
    hal_config.sources += [hal_dir+'/vcp/*.c']
    env.appendDriver(STM32_USB_DEVICE_CDC_Driver())
else:
    hal_config.paths += [hal_dir+'/uart']
    hal_config.sources += [hal_dir+'/uart/*.c']
