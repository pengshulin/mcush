from Arm.Stm32 import *

env = Stm32f407xx()
env.setLinkfile( '/ld/stm32f407xe_min.ld' )
env.appendDefineFlags( [ 'HSE_VALUE=8000000' ] )

