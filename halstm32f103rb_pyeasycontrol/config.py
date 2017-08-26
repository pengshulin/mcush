from Arm.Stm32 import *

env = Stm32f1md()
env.setLinkfile( '/ld/stm32f103xb_min.ld' )
env.appendDriver(STM32_USB_FS_Driver())
port = 'ARM_CM3'

    
#env.appendDefineFlags( [ 'SUSPEND_ENABLED=1' ] )
env.appendDefineFlags( [ 'MCUSH_VFS=1', 'MCUSH_ROMFS=1' ] )

env.appendDefineFlags( [ 'HAL_LED_NUM=2', 'HAL_LED_REV', 'HAL_LED_PORTS={GPIOC,GPIOC}', 'HAL_LED_PINS={GPIO_Pin_0,GPIO_Pin_1}' ] )
