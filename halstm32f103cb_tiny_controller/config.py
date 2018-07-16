from Arm.Stm32 import *
env = Stm32f1md()
env.setLinkfile( '/ld/stm32f103xb_min.ld' )
env.appendDriver(STM32_USB_FS_Driver())
env.appendDefineFlags( [ 'SUSPEND_ENABLED=0' ] )


