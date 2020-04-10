from Arm.Stm32 import *

env = Stm32f030xx()
env.setLinkfile( '/ld/stm32f030x8_min.ld' )
env.appendDefineFlags( [ 'HSE_VALUE=8000000' ] )

hal_config.paths += ['common']
hal_config.sources += ['common/*.c']
#hal_dir = 'hal' if hal_config.use_hal_driver else 'std'
hal_dir = 'std'
hal_config.paths += [hal_dir]
hal_config.sources += [hal_dir+'/*.c']
hal_config.paths += [hal_dir+'/uart']
hal_config.sources += [hal_dir+'/uart/*.c']

hal_config.freertos_heap = 4  # use heap_4.c instead of newlib
env.appendDefineFlags( ['configTOTAL_HEAP_SIZE=2000'] )

env.appendDefineFlags( [
    'configSUPPORT_STATIC_ALLOCATION=1',
    'MCUSH_STACK_SIZE=2*1024',
    'MCUSH_STACK_ALLOC_STATIC=1',
    'SHELL_LINE_BUF_SIZE=256',
    'MCUSH_VFS_STATISTICS=0',
    #'USE_CMD_HELP=0',
    #'USE_CMD_SCPI_IDN=0',
    #'USE_CMD_SCPI_RST=0',
    'USE_CMD_REBOOT=1',
    'USE_CMD_RESET=0',
    'USE_CMD_DUMP=1',
    'USE_CMD_WRITE=0',
    'USE_CMD_MFILL=0',
    'USE_CMD_WAIT=0',
    'USE_CMD_WDG=0',
    'USE_CMD_ECHO=0',
    'USE_CMD_UPTIME=1',
    'USE_CMD_RM=0',
    'USE_CMD_RENAME=0',
    'USE_CMD_CP=0',
    'USE_CMD_CAT=1',
    'CMD_CAT_READONLY=1',
    'USE_CMD_LOAD=0',
    'USE_CMD_MAPI=1',
    'USE_CMD_MKBUF=0',
    'configUSE_TIMERS=0',
    'configUSE_MUTEX=0',
    'configCHECK_FOR_STACK_OVERFLOW=0',  # 1-border, 2-pattern
    'MCUSH_FREERTOS_PEEK_API=1',
    'configUSE_TRACE_FACILITY=1',
    'USE_CMD_SYSTEM=1',
    'SHELL_QUOTE_PARSE_ENALBE=0',
    'SUSPEND_ENABLED=0',
    'USE_CMD_BEEP=0',
    'USE_CMD_SGPIO=0',
    'USE_CMD_PWM=0',
    'USE_CMD_ADC=0',
    'USE_CMD_CRC=0',
    'USE_CMD_SPI=1',
    'USE_CMD_I2C=1',
    'USE_CMD_DS1W=1',
    'USE_CMD_PULSE=1',
    ] )

