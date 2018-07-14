from Arm.Stm32 import *

env = Stm32f1md()
haldir='stm32f103xb'
#env.setLinkfile( '/ld/stm32f103x8_min.ld' )
env.setLinkfile( '/ld/stm32f103xb_min.ld' )


