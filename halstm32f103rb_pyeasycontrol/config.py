from Arm.Stm32 import *

env = Stm32f1md()
env.setLinkfile( '/ld/stm32f103xb_min.ld' )
env.appendDriver(STM32_USB_FS_Driver())

env.appendDefineFlags( [
    'CMD_I2C_SDA_PORT=2', 'CMD_I2C_SDA_PIN=0',
    'CMD_I2C_SCL_PORT=2', 'CMD_I2C_SCL_PIN=1',
    'CMD_SPI_SDI_PORT=2', 'CMD_SPI_SDI_PIN=0',
    'CMD_SPI_SDO_PORT=2', 'CMD_SPI_SDO_PIN=1',
    'CMD_SPI_SCK_PORT=2', 'CMD_SPI_SCK_PIN=2',
    'CMD_SPI_CS_PORT=2',  'CMD_SPI_CS_PIN=3',
     ] )
    
#env.appendDefineFlags( [ 'SUSPEND_ENABLED=1' ] )

#env.appendDefineFlags( [ 'HAL_LED_NUM=2', 'HAL_LED_REV', 'HAL_LED_PORTS={GPIOC,GPIOC}', 'HAL_LED_PINS={GPIO_Pin_0,GPIO_Pin_1}' ] )
