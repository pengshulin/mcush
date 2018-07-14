from Arm.Stm32 import *

env = Stm32f2()
env.setLinkfile( '/ld/stm32f207xg_min.ld' )

env.appendDefineFlags( [ 'HSE_DISABLED' ] )

