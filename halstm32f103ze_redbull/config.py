from Arm.Stm32 import *
env = Stm32f103xe( use_hal_driver=hal_config.use_hal_driver )
env.setLinkfile( '/ld/stm32f103ze_redbull.ld' )
env.appendDefineFlags( [ 'HSE_VALUE=8000000' ] )
#env.appendDefineFlags( ['DATA_IN_ExtSRAM'] )

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
    if hal_dir == 'std':
        env.appendDriver(STM32_USB_FS_Driver() )
    else:
        env.appendDriver(STM32_USB_DEVICE_CDC_Driver())
else:
    hal_config.paths += [hal_dir+'/uart']
    hal_config.sources += [hal_dir+'/uart/*.c']



env.appendDefineFlags( [
    #'USE_CMD_HELP=0',
    #'USE_CMD_SCPI_IDN=0',
    'USE_CMD_SCPI_RST=0',
    #'USE_CMD_REBOOT=0',
    'USE_CMD_RESET=0',
    #'USE_CMD_GPIO=0',
    #'USE_CMD_LED=0',
    #'USE_CMD_DUMP=0',
    #'USE_CMD_WRITE=0',
    #'USE_CMD_MFILL=0',
    #'USE_CMD_WAIT=0',
    'USE_CMD_WDG=0',
    #'USE_CMD_UPTIME=0',
    #'USE_CMD_SYSTEM=0',
    'USE_CMD_RM=0',
    'USE_CMD_RENAME=0',
    'USE_CMD_CP=0',
    'USE_CMD_LOAD=0',
    #'USE_CMD_MAPI=0',
    #'USE_CMD_MKBUF=0',
    #'MCUSH_FREERTOS_PEEK_API=0',
    #'configUSE_TIMERS=0',
    #'configUSE_MUTEX=0',
    #'configCHECK_FOR_STACK_OVERFLOW=0',
    #'configTOTAL_HEAP_SIZE=10240',
    #'MCUSH_STACK_SIZE=10240',
    #'SHELL_QUOTE_PARSE_ENALBE=0',
    #'SUSPEND_ENABLED=0',
    'USE_CMD_PWM=%s'% (int(hal_dir=='std')),
    'USE_CMD_ADC=%s'% (int(hal_dir=='std')),
    ] )



