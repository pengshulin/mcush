from Arm.Stm32 import *

env = Stm32f1md()
env.setLinkfile( '/ld/stm32f103xb_min.ld' )
env.appendDriver(STM32_USB_FS_Driver())
port = 'ARM_CM3'

env.appendDefineFlags( [ 'MCUSH_VFS=1', 'MCUSH_ROMFS=1' ] )

USE_SPIFFS=True
USE_SPIFFS=False

if USE_SPIFFS:
    env.appendDefineFlags( [ 'MCUSH_SPIFFS=1' ] )
    env.appendPath([ '/libspiffs' ])
    env.appendGlobSource([ '/libspiffs/*.c' ])


    
env.appendDefineFlags( [ 
    #'USE_CMD_HELP=0',
    #'USE_CMD_SCPI_IDN=0',
    #'USE_CMD_SCPI_RST=0',
    #'USE_CMD_RESET=0',
    #'USE_CMD_GPIO=0',
    #'USE_CMD_LED=0',
    #'USE_CMD_DUMP=0',
    #'USE_CMD_WRITE=0',
    #'USE_CMD_MFILL=0',
    #'USE_CMD_WAIT=0',
    #'USE_CMD_WDG=0',
    #'USE_CMD_UPTIME=0',
    #'USE_CMD_SYSTEM=0',
    #'USE_CMD_MAPI=0',
    #'USE_CMD_MKBUF=0',
    #'MCUSH_FREERTOS_PEEK_API=0',
    #'configUSE_TIMERS=0',
    #'configUSE_MUTEX=0',
    #'configCHECK_FOR_STACK_OVERFLOW=0',
    ##'MCUSH_STACK_SIZE=10240',
    #'SHELL_QUOTE_PARSE_ENALBE=0',
    ] )



