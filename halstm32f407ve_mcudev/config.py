from Arm.Stm32 import *
env = Stm32f407xx()
env.setLinkfile( '/ld/stm32f407xe_min.ld' )
env.appendDefineFlags( [ 'HSE_VALUE=8000000' ] )
env.appendDefineFlags( [ 'HAL_RNG=1' ] )
env.appendDefineFlags( [ 'HAL_REBOOT_COUNTER=1' ] )


