from Arm.Stm32 import *

env = Stm32f1md()
env.setLinkfile( '/ld/stm32f103x8_min.ld' )
env.appendDriver(STM32_USB_FS_Driver())

env.appendDefineFlags( [
    'USE_CMD_HELP=0',
    #'USE_CMD_SCPI_IDN=0',
    'USE_CMD_SCPI_RST=0',
    'USE_CMD_RESET=0',
    #'USE_CMD_GPIO=0',
    #'USE_CMD_LED=0',
    #'USE_CMD_DUMP=0',
    'USE_CMD_WRITE=0',
    'USE_CMD_MFILL=0',
    'USE_CMD_WAIT=0',
    'USE_CMD_WDG=0',
    #'USE_CMD_UPTIME=0',
    #'USE_CMD_SYSTEM=0',
    'USE_CMD_MAPI=0',
    'USE_CMD_MKBUF=0',
    'MCUSH_FREERTOS_PEEK_API=0',
    'configUSE_TIMERS=0',
    'configUSE_MUTEX=0',
    'configCHECK_FOR_STACK_OVERFLOW=0',
    'configTOTAL_HEAP_SIZE=10240',
    #'MCUSH_STACK_SIZE=10240',
    'SHELL_QUOTE_PARSE_ENALBE=0',
    ] )

