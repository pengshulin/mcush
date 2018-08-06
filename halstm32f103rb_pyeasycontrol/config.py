from Arm.Stm32 import *

env = Stm32f1md()
env.setLinkfile( '/ld/stm32f103xb_min.ld' )
env.appendDefineFlags( [ 'HSE_VALUE=8000000' ] )
env.appendDriver(STM32_USB_FS_Driver())
#env.appendDefineFlags( [ 'SUSPEND_ENABLED=1' ] )

env.appendDefineFlags( [
    'CMD_I2C_SDA_PORT=2', 'CMD_I2C_SDA_PIN=0',
    'CMD_I2C_SCL_PORT=2', 'CMD_I2C_SCL_PIN=1',
    'CMD_SPI_SDI_PORT=2', 'CMD_SPI_SDI_PIN=0',
    'CMD_SPI_SDO_PORT=2', 'CMD_SPI_SDO_PIN=1',
    'CMD_SPI_SCK_PORT=2', 'CMD_SPI_SCK_PIN=2',
    'CMD_SPI_CS_PORT=2',  'CMD_SPI_CS_PIN=3',
     ] )
    
env.appendDefineFlags( [
    'USE_CMD_SCPI_RST=0',
    'USE_CMD_RESET=0',
    'USE_CMD_WAIT=0',
    'USE_CMD_WDG=0',
    'USE_CMD_RM=0',
    'USE_CMD_RENAME=0',
    'USE_CMD_CP=0',
    'USE_CMD_LOAD=0',
    ] )

