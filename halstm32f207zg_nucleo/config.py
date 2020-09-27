from Arm.Stm32 import *

env = Stm32f2( use_hal_driver=hal_config.use_hal_driver )
env.setLinkfile( '/ld/stm32f207xg.ld' )
#env.appendDefineFlags( [ 'HSE_VALUE=8000000' ] )  # not soldered 
env.appendDefineFlags( [ 'HSE_DISABLED' ] )


hal_config.paths += ['common']
hal_config.sources += ['common/*.c']
hal_dir = 'hal' if hal_config.use_hal_driver else 'std'
hal_config.paths += [hal_dir]
hal_config.sources += [hal_dir+'/*.c']


if hal_config.use_vcp:
    hal_config.paths += [hal_dir+'/vcp']
    hal_config.sources += [hal_dir+'/vcp/*.c']
    if hal_dir == 'std':
        env.appendDriver(STM32_USB_FS_Driver() )
    else:
        env.appendDriver(STM32_USB_DEVICE_CDC_Driver())
else:
    hal_config.paths += [hal_dir+'/uart']
    hal_config.sources += [hal_dir+'/uart/*.c']

