from Arm.Stm32 import *
env = Stm32f411xe( use_hal_driver=True  )
env.setLinkfile( '/ld/stm32f411xe_min.ld' )
env.appendDefineFlags( [ 'HSE_VALUE=25000000' ] )
env.appendDefineFlags( [ 'HAL_REBOOT_COUNTER=1' ] )

hal_config.paths += ['common']
hal_config.sources += ['common/*.c']
hal_dir = 'hal'
hal_config.paths += [hal_dir]
hal_config.sources += [hal_dir+'/*.c']


if env.getBoolEnv('USE_UART'):
    hal_config.use_vcp = False  # use PA9/10 UART1
if hal_config.use_vcp is None:
    hal_config.use_vcp = True  # use vcp as default

if hal_config.use_vcp:
    hal_config.paths += [hal_dir+'/vcp']
    hal_config.sources += [hal_dir+'/vcp/*.c']
    env.appendDriver(STM32_USB_DEVICE_CDC_Driver())
else:
    hal_config.paths += [hal_dir+'/uart']
    hal_config.sources += [hal_dir+'/uart/*.c']

if hal_config.use_spiffs:
    env.appendDefineFlags( [ 'SPIFLASH_AUTO_DETECT=1' ] )
    hal_config.paths += [hal_dir+'/spiffs']
    hal_config.sources += [hal_dir+'/spiffs/*.c']

hal_config.use_fatfs = False
 
env.appendDefineFlags( [
    #'USE_CMD_SCPI_RST=0',
    #'USE_CMD_DUMP=0',
    #'USE_CMD_WRITE=0',
    #'USE_CMD_MFILL=0',
    'USE_CMD_WAIT=0',
    'USE_CMD_WDG=0',
    'USE_CMD_ECHO=0',
    #'USE_CMD_UPTIME=0',
    #'USE_CMD_SYSTEM=0',
    #'USE_CMD_RM=0',
    #'USE_CMD_RENAME=0',
    #'USE_CMD_CP=0',
    'USE_CMD_LOAD=0',
    #'USE_CMD_MAPI=0',
    'USE_CMD_MKBUF=0',

    'USE_CMD_SPI2=1',
    'USE_CMD_SPI3=1',
    'USE_CMD_SPI4=1',
    'USE_CMD_I2C2=1',
    'USE_CMD_I2C3=1',
    'USE_CMD_I2C4=1',
    'USE_CMD_PULSE2=1',
    'USE_CMD_PULSE3=1',
    'USE_CMD_PULSE4=1',
    'USE_CMD_DS1W2=1',
    'USE_CMD_DS1W3=1',
    'USE_CMD_DS1W4=1',

    'USE_CMD_SGPIO=0',
    'USE_CMD_PWM=1',
    'USE_CMD_ADC=1',
    #'USE_CMD_CRC=0',
    ] )


