from Arm.Stm32 import *

env = Stm32f1hd()
env.setLinkfile( '/ld/stm32f103ze_redbull.ld' )
#env.appendDefineFlags( ['DATA_IN_ExtSRAM'] )


if hal_config.use_vcp == True:
    hal_config.paths += ['vcp']
    hal_config.sources += ['vcp/*.c']
    env.appendDriver(STM32_USB_FS_Driver())
    env.appendDefineFlags( [ 'SUSPEND_ENABLED=0' ] )
else:
    hal_config.paths += ['uart']
    hal_config.sources += ['uart/*.c']


#if hal_config.use_spiffs:
#    env.appendDefineFlags( [ 'SPIFLASH_AUTO_DETECT=1' ] )


env.appendDefineFlags( [
    'USE_CMD_SGPIO=1',
    'USE_CMD_ADC=1',
    ] )

