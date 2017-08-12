from Arm.Stm32 import *

env = Stm32f1md()
env.setLinkfile( '/ld/stm32f103xb_min.ld' )
env.appendDriver(STM32_USB_FS_Driver())
port = 'ARM_CM3'

    
env.appendDefineFlags( [ 'MCUSH_VFS=1', 'MCUSH_ROMFS=1' ] )
