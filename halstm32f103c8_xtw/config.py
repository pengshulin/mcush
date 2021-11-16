from Arm.Stm32 import *

if hal_config.use_hal_driver is None:
    hal_config.use_hal_driver = True  # use hal_driver as default
if hal_config.use_vcp is None:
    hal_config.use_vcp = True  # use vcp as default

if hal_config.use_hal_driver:
    env = Stm32f103xb( use_hal_driver=True )
else:
    env = Stm32f1md( use_hal_driver=False )
env.setLinkfile( '/ld/stm32f103x8.ld' )
env.appendDefineFlags( [ 'HSE_VALUE=8000000' ] )

hal_config.paths += ['common']
hal_config.sources += ['common/*.c']
hal_dir = 'hal' if hal_config.use_hal_driver else 'std'
hal_config.paths += [hal_dir]
hal_config.sources += [hal_dir+'/*.c']

if env.getBoolEnv('USE_UART'):
    hal_config.use_vcp = False  # use UART
if hal_config.use_vcp:
    hal_config.paths += [hal_dir+'/vcp']
    hal_config.sources += [hal_dir+'/vcp/*.c']
    if hal_dir == 'std':
        env.appendDriver(STM32_USB_FS_Driver() )
        env.appendDefineFlags( [ 'SUSPEND_ENABLED=0' ] )
    else:
        env.appendDriver(STM32_USB_DEVICE_CDC_Driver())
    env.appendDefineFlags( ['HAL_RESET_USB_PINS=1'] )
else:
    hal_config.paths += [hal_dir+'/uart']
    hal_config.sources += [hal_dir+'/uart/*.c']

if hal_config.use_hid:
    if hal_config.use_hal_driver:
        hal_config.paths += [hal_dir+'/hid']
        hal_config.sources += [hal_dir+'/hid/*.c']
        env.appendDriver(STM32_USB_DEVICE_HID_Driver())
        env.appendDefineFlags( ['SUPPORT_HID=1'] )
        #env.appendDefineFlags( ['USE_ST_VID_PID=1'] )
        #env.appendDefineFlags( ['HAL_RESET_USB_PINS=1'] )

hal_config.freertos_heap = 4  # use heap_4.c instead of newlib
#env.appendDefineFlags( ['configTOTAL_HEAP_SIZE=8*1024'] )
env.appendDefineFlags( ['configTOTAL_HEAP_SIZE=7*1024'] )
#env.appendDefineFlags( ['configTOTAL_HEAP_SIZE=4*1024'] )



if env.getBoolEnv('BLACK_BOARD'):
    env.appendDefineFlags( ['BLACK_BOARD'] )
else:
    env.appendDefineFlags( ['BLUE_BOARD'] )

env.appendDefineFlags( [
    'configSUPPORT_STATIC_ALLOCATION=1',
    'MCUSH_VFS_STATISTICS=0',
    #'USE_CMD_HELP=0',
    #'USE_CMD_SCPI_IDN=0',
    #'USE_CMD_SCPI_RST=0',
    'USE_CMD_REBOOT=0',
    'USE_CMD_RESET=0',
    'USE_CMD_DUMP=0',
    'USE_CMD_WRITE=0',
    'USE_CMD_MFILL=0',
    'USE_CMD_WAIT=0',
    'USE_CMD_ECHO=0',
    'USE_CMD_UPTIME=0',
    #'USE_CMD_SYSTEM=0',
    'USE_CMD_SYSTEM_KERNEL=0',
    'USE_CMD_SYSTEM_IDLE=0',
    'USE_CMD_CAT_WRITE=0',
    'USE_CMD_RM=0',
    'USE_CMD_RENAME=0',
    'USE_CMD_CP=0',
    'USE_CMD_LOAD=0',
    'USE_CMD_MAPI=0',
    'USE_CMD_MKBUF=0',
    'configUSE_TIMERS=0',
    'configUSE_MUTEX=0',
    'configCHECK_FOR_STACK_OVERFLOW=0',
    'MCUSH_FREERTOS_PEEK_API=1',
    'configUSE_TRACE_FACILITY=1',
    #'MCUSH_STACK_SIZE=10240',
    'SHELL_QUOTE_PARSE_ENABLE=0',
    'SUSPEND_ENABLED=0',
    'USE_CMD_BEEP=0',
    'USE_CMD_SGPIO=0',
    'USE_CMD_PWM=1',
    'USE_CMD_ADC=1',
    'USE_CMD_CRC=0',
    ] )

