from VEnvironment import *

env = VEnvironment()

hal_config.paths += ['common']
hal_config.sources += ['common/*.c']


env.appendDefineFlags( [
    #'USE_CMD_HELP=0',
    #'USE_CMD_SCPI_IDN=0',
    #'USE_CMD_SCPI_RST=0',
    #'USE_CMD_REBOOT=0',
    #'USE_CMD_DUMP=0',
    #'USE_CMD_WRITE=0',
    #'USE_CMD_MFILL=1',
    #'USE_CMD_ECHO=1',
    #'USE_CMD_UPTIME=0',
    #'USE_CMD_SYSTEM=0',
    #'USE_CMD_RM=0',
    #'USE_CMD_RENAME=0',
    #'USE_CMD_CP=0',
    #'USE_CMD_LOAD=0',
    #'USE_CMD_MAPI=0',
    #'USE_CMD_MKBUF=1',
    #'SHELL_QUOTE_PARSE_ENABLE=0',
    #'USE_CMD_BEEP=0',
    #'USE_CMD_SGPIO=0',
    #'USE_CMD_PWM=0',
    #'USE_CMD_ADC=0',
    #'USE_CMD_CRC=0',
    ] )

