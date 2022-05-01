from VEnvironment import *

env = VEnvironment()

hal_config.paths += ['common']
hal_config.sources += ['common/*.c']

env.appendCompilerFlag( [
    '-I/usr/include/dbus-1.0',
    '-I/usr/lib/x86_64-linux-gnu/dbus-1.0/include',
    '-I/usr/include/glib-2.0',
    '-I/usr/lib/x86_64-linux-gnu/glib-2.0/include',
    ] )

env.Append( LIBS=['dbus-glib-1', 'dbus-1', 'gobject-2.0', 'glib-2.0'] )


env.appendDefineFlags( [
    'HAL_DBUS=1',
    'SUPPORT_WS2812=1',
    'SUPPORT_DS1W=1',
    'USE_CMD_DS1W=0',
    #'HAL_MODBUS=1',
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
    'USE_CMD_PWM=1',
    'USE_CMD_ADC=1',
    #'USE_CMD_DAQ=1',
    #'USE_CMD_CRC=0',
    'MCUSH_VFS_STATISTICS=1',
    'USE_CMD_SYSTEM_VFS=1',
    ] )

