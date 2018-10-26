from Arm.Stm32 import *

env = Stm32f767xx()
env.setLinkfile( '/ld/stm32f767zi_min.ld' )

hal_config.paths += ['common']
hal_config.sources += ['common/*.c']

hal_dir = 'hal'

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


# disable those functions that are not finished
env.appendDefineFlags( [
    'USE_CMD_SCPI_RST=0',
    'USE_CMD_REBOOT=0',
    'USE_CMD_REBOOT_COUNTER=0',
    'USE_CMD_RESET=0',
    'USE_CMD_WDG=0',
    'USE_CMD_PWM=0',
    'USE_CMD_ADC=0',
    'USE_CMD_SGPIO=0',
    'USE_CMD_UPGRADE=0',
    'HAL_RTC=0',
    'HAL_RNG=0',
    ] )


